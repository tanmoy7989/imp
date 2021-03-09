#!/usr/bin/env python

"""Parse the manual's XML output to get the contents. Then patch each page
   in the HTML output to add navigation links (next page, previous page,
   edit on GitHub).

   This also applies minor fixes to the HTML to fix places where doxygen
   generates wonky output.
"""

import xml.etree.ElementTree as ET
import glob
import os
import sys
import re


def get_basename_from_xml(f):
    return os.path.basename(f)[:-4]


def get_version():
    version = "develop"
    versionpath = "VERSION"
    if os.path.exists(versionpath):
        with open(versionpath, "r") as fh:
            version = fh.read().strip()
    return version


def get_source_file_name(module, subdir, f, example):
    """Get the relative path to the source for `f` in module `module`."""
    if example:
        return os.path.join('modules', module, 'examples', subdir, f)
    elif f.endswith('.py'):
        return os.path.join('modules', module, 'pyext', 'src', subdir, f)
    else:
        return os.path.join('modules', module, 'include', subdir, f)


def patch_html(content):
    """Fix wonky HTML generated by doxygen"""
    # 1. Cannot use \example in markup since that is a doxygen special
    #    command; but \\example generates '\\example' in the HTML output. So
    #    replace all \\ with \.
    # 2. Have to use %IMP in the text to prevent doxygen auto-linking it
    #    to the IMP namespace docs, but this puts literal %IMP in titles.
    # 3. Trying to show a literal end-comment with `*/` doesn't work.
    for line in content:
        yield line.replace('\\\\', '\\') \
                  .replace('%IMP', 'IMP') \
                  .replace('`` blocks', '<code>*/</code> blocks')


def get_all_listitems(top_element, parent=None):
    """Recursively get all listitem objects in an ElementTree.
       Yield tuples of a listitem object and its immediate parent."""
    for child in top_element:
        if child.tag == 'listitem':
            yield child, parent
            for x in get_all_listitems(child, child):
                yield x
        else:
            for x in get_all_listitems(child, parent):
                yield x


def get_all_links(item):
    """Get all hyperlinks that are children of an ElementTree object."""
    for para in item.findall("para"):
        for ref in para.findall("ref"):
            if ref.attrib['kindref'] == 'compound' \
               and 'external' not in ref.attrib:
                yield ref


def get_page_from_source(source, id_re, id_to_page):
    """Given a source .md or .dox file, return the Page object."""
    with open(source) as fh:
        for line in fh:
            if '\\mainpage' in line:
                return id_to_page['indexpage']
            m = id_re.search(line)
            if m:
                return id_to_page[m.group(1)]
    raise ValueError("Cannot find page id in %s" % source)


class Page(object):
    """Representation of a single page in the documentation"""
    def __init__(self, id):
        self.id = id
        self.next_page = self.prev_page = self.parent_page = None
        self.children = []

    def write_children(self, out):
        """Print out an HTML list of all children of this Page"""
        out.write('<ul>\n')
        for c in self.children:
            out.write('<li><a class="el" href="%s.html">%s</a></li>\n'
                      % (c.out_file_name, c.title))
        out.write('</ul>\n')

    def make_link(self, caption, img, dest):
        """Return an HTML fragment to link to another Page"""
        return '<a href="%s.html" title="%s: %s"><img src="%s.png" ' \
               'alt="%s"/></a>' % (dest.out_file_name, caption, dest.title,
                                   img, img)

    def map_location_to_source(self, top_source_dir):
        """Try to map the XML location to a real file in the source dir"""
        if not hasattr(self, 'location'):
            return
        if 'IMP' in self.location:
            ps = self.location[self.location.find('IMP')+4:].split(os.path.sep)
            example = False
        elif '/examples/' in self.location:
            ps = self.location[
                self.location.find('/examples/')+10:].split(os.path.sep)
            example = True
        else:
            return
        if len(ps) == 1 or ps[0] == 'internal':
            ps.insert(0, 'kernel')
        source = get_source_file_name(ps[0], os.path.sep.join(ps[1:-1]),
                                      ps[-1], example)
        if os.path.exists(os.path.join(top_source_dir, source)):
            self.source_file_name = source

    def github_edit(self):
        """Return an HTML fragment to edit this Page on GitHub"""
        if not hasattr(self, 'source_file_name'):
            return ''
        sp = self.source_file_name.split(os.path.sep)
        if sp[:2] == ['modules', 'pmi']:
            root = 'https://github.com/salilab/pmi/blob/develop/'
            del sp[:2]
        elif sp[:2] == ['modules', 'pmi1']:
            root = 'https://github.com/salilab/pmi/blob/legacy-pmi1/'
            del sp[:2]
        elif sp[:2] == ['modules', 'npctransport']:
            root = 'https://github.com/salilab/npctransport/blob/develop/'
            del sp[:2]
        else:
            root = 'https://github.com/salilab/imp/blob/develop/'
        return('      <li><a href="%s%s"><i class="fab fa-github">'
               '</i> Edit on GitHub</a></li>\n' % (root, os.path.sep.join(sp)))


class Docs(object):
    """Representation of all doxygen documentation"""

    def __init__(self, xml_dir, html_dir, top_source_dir, source_subdirs):
        """Read in all documentation (as a list of Page objects) built from
           sources under top_source_dir and output to XML in xml_dir and
           HTML in html_dir"""
        self.html_dir = html_dir
        self._contents_page = os.path.join(xml_dir, 'indexpage.xml')
        self.pages = self._read_xml_pages(xml_dir)
        self.page_by_id = dict((p.id, p) for p in self.pages)
        self._find_page_source(top_source_dir, source_subdirs)

    def build_hierarchy(self):
        """By inspecting the contents page, determine the child-parent-sibling
           relationships between all pages in the documentation"""
        index_page = self.page_by_id['indexpage']
        # Make sure that all pages not listed in the contents have an 'up'
        # link to the main page
        for p in self.pages:
            if p is not index_page:
                p.parent_page = index_page
        prev_page = index_page
        tree = ET.parse(self._contents_page)
        root = tree.getroot()
        for item, parent in get_all_listitems(root):
            parent_page = index_page
            if parent is not None:
                links = list(get_all_links(parent))
                if len(links) > 0:
                    parent_page = self.page_by_id[links[0].attrib['refid']]
            for link in get_all_links(item):
                this_page = self.page_by_id[link.attrib['refid']]
                if prev_page:
                    this_page.prev_page = prev_page
                    prev_page.next_page = this_page
                this_page.parent_page = parent_page
                parent_page.children.append(this_page)
                prev_page = this_page

    def _read_xml_pages(self, xml_dir):
        """Read all generated doxygen XML files to determine page properties,
           such as title, doxygen ID, and output file name"""
        pages = []
        for f in glob.glob(os.path.join(xml_dir, "*.xml")):
            tree = ET.parse(f)
            root = tree.getroot()
            compounddef = root.find('compounddef')
            if (compounddef is not None
                    and compounddef.attrib['kind'] == 'page'):
                p = Page(compounddef.attrib['id'])
                # Remove .xml suffix
                p.out_file_name = get_basename_from_xml(f)
                # Handle index special case
                if p.out_file_name == 'indexpage':
                    p.out_file_name = 'index'
                p.title = compounddef.find('title').text.replace('%IMP', 'IMP')
                pages.append(p)
            elif (compounddef is not None
                  and compounddef.attrib['kind'] in ('class', 'file')):
                p = Page(compounddef.attrib['id'])
                # Remove .xml suffix
                p.out_file_name = get_basename_from_xml(f)
                p.location = compounddef.find('location').attrib['file']
                if '/examples/' in p.location:
                    p.out_file_name += '-example'
                # Exclude source of .md or .dox files
                if not p.id.endswith('_8md') and not p.id.endswith('_8dox'):
                    pages.append(p)
        return pages

    def _find_page_source(self, top_source_dir, source_subdirs):
        """Determine which source file generated each output
           documentation file"""
        for pattern, id_re in (("*.md", re.compile(r"\{#([^}]+)\}")),
                               ("*.dox", re.compile(r"\\page\s+(\S+)"))):
            for source_subdir in source_subdirs:
                for source in glob.glob(os.path.join(top_source_dir,
                                                     source_subdir, pattern)):
                    # Ignore toplevel README and CONTRIBUTING
                    if source_subdir == '.' \
                       and (source.endswith('README.md')
                            or source.endswith('CONTRIBUTING.md')):
                        continue
                    page = get_page_from_source(source, id_re, self.page_by_id)
                    if source_subdir == '.':
                        page.source_file_name = os.path.basename(source)
                    else:
                        page.source_file_name = os.path.join(
                            source_subdir, os.path.basename(source))

    def get_html_pages(self, page):
        """Return the full path(s) to a page's output HTML file(s), if any"""
        base = page.out_file_name
        pages = self._get_htmls_from_basename(base)
        # Sometimes doxygen makes this substitution, sometimes it doesn't,
        # so try both
        if '__' in base:
            pages.extend(
                self._get_htmls_from_basename(base.replace('__', '_')))
        return pages

    def _get_htmls_from_basename(self, base):
        paths = [os.path.join(self.html_dir, b + '.html')
                 for b in (base, base + '_source')]
        paths = [b for b in paths if os.path.exists(b)]
        if not paths:
            # Try with a namespace prefix
            g = glob.glob(os.path.join(self.html_dir,
                                       "*_2" + base + '.html'))
            if len(g) == 1:
                return g
        return paths

    def add_page_navigation(self, page):
        """Patch the HTML output for a given page to add navigation and other
           useful links, such as GitHub editing"""
        links = []
        if page.prev_page:
            links.append(page.make_link("Previous page", "prev",
                                        page.prev_page))
        if page.parent_page:
            links.append(page.make_link("Parent page", "up",
                                        page.parent_page))
        if page.next_page:
            links.append(page.make_link("Next page", "next",
                                        page.next_page))
        edit_link = page.github_edit()
        doxversion = '<a class="doxversion" ' \
                     + 'href="https://integrativemodeling.org/doc.html">' \
                     + 'version %s</a>' % get_version()
        toplinks = '<div class="doxnavlinks">' + doxversion \
                   + " ".join(links) + '</div>\n'
        botlinks = '<div class="doxnavlinks">' + " ".join(links) + '</div>\n'
        for fname in self.get_html_pages(page):
            with open(fname) as fh:
                content = patch_html(fh.readlines())
            with open(fname, 'w') as out:
                for line in content:
                    if line.startswith('</div><!-- top -->'):
                        out.write(line)
                        out.write(toplinks)
                    elif '<span>Other&#160;Versions</span>' in line:
                        out.write(line)
                        out.write(edit_link)
                    elif line.startswith('<hr class="footer"') and links:
                        out.write('<hr class="footer"/>')
                        out.write(botlinks)
                    elif '[SUBPAGES]' in line:
                        page.write_children(out)
                    else:
                        out.write(line)

    def patch_contents(self):
        """Apply extra patches to index.html"""
        index_html = os.path.join(self.html_dir, 'index.html')
        version = '<b>' + get_version() + '</b>'
        if 'develop' in version:
            version += ' (a nightly build)'
        with open(index_html) as fh:
            content = fh.readlines()
        with open(index_html, 'w') as out:
            for line in content:
                line = line.replace('<div class="title">Contents',
                                    '<div class="title">IMP Manual')
                line = line.replace('IMPVERSION', version)
                out.write(line)


def main():
    docs = Docs(xml_dir='doxygen/manual/xml',
                html_dir='doc/manual',
                top_source_dir=sys.argv[1],
                source_subdirs=sys.argv[2:])
    docs.build_hierarchy()
    for p in docs.pages:
        docs.add_page_navigation(p)
    docs.patch_contents()


if __name__ == '__main__':
    main()
