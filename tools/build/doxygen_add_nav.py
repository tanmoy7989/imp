#!/usr/bin/env python

"""Parse the manual's XML output to get the contents. Then patch each page
   in the HTML output to add navigation links (next page, previous page).

   This also applies minor fixes to the HTML to fix places where doxygen
   generates wonky output.
"""

import xml.etree.ElementTree as ET
import glob
import os
import sys
import re

def get_version():
    version = "develop"
    versionpath = "VERSION"
    if os.path.exists(versionpath):
        version = open(versionpath, "r").read().strip()
    return version

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

def patch_contents(index_html):
    """Apply extra patches to index.html"""
    version = '<b>' + get_version() + '</b>'
    if 'develop' in version:
        version += ' (a nightly build)'
    content = open(index_html).readlines()
    out = open(index_html, 'w')
    for line in content:
        line = line.replace('<div class="title">Contents',
                            '<div class="title">IMP Manual')
        line = line.replace('IMPVERSION', version)
        out.write(line)

def patch_all_other_html_pages(html_dir, already_patched):
    """Patch pages not already patched"""
    for f in glob.glob(os.path.join(html_dir, "*.html")):
        if f not in already_patched:
            content = patch_html(open(f).readlines())
            out = open(f, 'w')
            for line in content:
                out.write(line)

def map_ids_to_pages(xml_dir):
    mapping = {}
    for f in glob.glob(os.path.join(xml_dir, "*.xml")):
        tree = ET.parse(f)
        root = tree.getroot()
        compounddef = root.find('compounddef')
        if compounddef is not None and compounddef.attrib['kind'] == 'page':
            # Remove .xml suffix
            mapping[compounddef.attrib['id']] = os.path.basename(f)[:-4]
    return mapping

def get_all_listitems(top_element, parent=None):
    for child in top_element:
        if child.tag == 'listitem':
            yield child, parent
            for x in get_all_listitems(child, child):
                yield x
        else:
            for x in get_all_listitems(child, parent):
                yield x

def get_all_links(item):
    for para in item.findall("para"):
        for ref in para.findall("ref"):
            if ref.attrib['kindref'] == 'compound' \
               and not 'external' in ref.attrib:
                yield ref

def get_contents(root, page_map):
    parent_pages = {}
    for item, parent in get_all_listitems(root):
        if parent is not None and parent not in parent_pages:
            links = list(get_all_links(parent))
            if len(links) > 0:
                parent_pages[parent] = page_map[links[0].attrib['refid']]
            else:
                parent_pages[parent] = None
        for link in get_all_links(item):
            yield page_map[link.attrib['refid']], link.text, parent_pages.get(parent, None)

def write_children(children, out):
    out.write('<ul>\n')
    for c in children:
        out.write('<li><a class="el" href="%s.html">%s</a></li>\n'
                  % (c[0], c[1]))
    out.write('</ul>\n')

def add_page_navigation(html_dir, pagename, children, prevpage, nextpage,
                        uppage, page_to_source, already_patched):
    def make_link(title, img, dest):
        return '<a href="%s.html" title="%s"><img src="%s.png" alt="%s"/></a>' \
               % (dest, title, img, img)
    def github_edit(dest):
        root = 'https://github.com/salilab/imp/blob/develop/'
        return('<a href="%s%s" title="Edit on GitHub"><img src="edit.png" '
               'alt="Edit on GitHub"/></a>' % (root, dest))
    links = []
    if prevpage:
        links.append(make_link("Go to previous page", "prev", prevpage))
    if uppage:
        links.append(make_link("Go to parent page", "up", uppage))
    if nextpage:
        links.append(make_link("Go to next page", "next", nextpage))
    edit_link = github_edit(page_to_source[pagename])
    doxversion = '<a class="doxversion" ' \
                 + 'href="http://integrativemodeling.org/doc.html">' \
                 + 'version %s</a>' % get_version()
    toplinks = '<div class="doxnavlinks">' + edit_link + " " + doxversion \
               + " ".join(links) + '</div>\n'
    botlinks = '<div class="doxnavlinks">' + " ".join(links) + '</div>\n'
    fname = os.path.join(html_dir, pagename + '.html')
    content = patch_html(open(fname).readlines())
    already_patched[fname] = None
    out = open(fname, 'w')
    for line in content:
        if line.startswith('</div><!-- top -->'):
            out.write(line)
            out.write(toplinks)
        elif line.startswith('<hr class="footer"'):
            out.write('<hr class="footer"/>')
            out.write(botlinks)
        elif '[SUBPAGES]' in line:
            write_children(children, out)
        else:
            out.write(line)

def get_page_children(i, contents):
    children = []
    for ind in range(i + 1, len(contents)):
        if contents[ind][2] == contents[i][0]:
            children.append(contents[ind][:2])
        else:
            break
    return children

def get_page_name_from_source(source, id_re, id_to_name):
    with open(source) as fh:
        for line in fh:
            if '\\mainpage' in line:
                return 'index'
            m = id_re.search(line)
            if m:
                return id_to_name[m.group(1)]
    raise ValueError("Cannot find page id in %s" % source)

def map_pages_to_source(top_source_dir, source_subdirs, id_to_name):
    mapping = {}
    for pattern, id_re in (("*.md", re.compile(r"\{#([^}]+)\}")),
                           ("*.dox", re.compile(r"\\page\s+(\S+)"))):
        for source_subdir in source_subdirs:
            for source in glob.glob(os.path.join(top_source_dir, source_subdir,
                                                 pattern)):
                page_name = get_page_name_from_source(source, id_re, id_to_name)
                if source_subdir == '.':
                    mapping[page_name] = os.path.basename(source)
                else:
                    mapping[page_name] = os.path.join(source_subdir,
                                                      os.path.basename(source))
    return mapping

def main():
    top_source_dir = sys.argv[1]
    source_subdirs = sys.argv[2:]
    xml_dir = 'doxygen/manual/xml'
    html_dir = 'doc/manual'
    page_map = map_ids_to_pages(xml_dir)
    page_to_source = map_pages_to_source(top_source_dir, source_subdirs,
                                         page_map)
    tree = ET.parse(os.path.join(xml_dir, 'indexpage.xml'))
    root = tree.getroot()

    contents = list(get_contents(root, page_map))
    already_patched = {}
    for i, page in enumerate(contents):
        children = get_page_children(i, contents)
        add_page_navigation(html_dir, page[0], children,
                            contents[i-1][0] if i > 0 else 'index',
                            contents[i+1][0] if i+1 < len(contents) else None,
                            contents[i][2] if contents[i][2] else 'index',
                            page_to_source, already_patched)
    if len(contents) > 0:
        add_page_navigation(html_dir, 'index', [], None, contents[0][0], None,
                            page_to_source, already_patched)
    patch_all_other_html_pages(html_dir, already_patched)
    patch_contents(os.path.join(html_dir, 'index.html'))

if __name__ == '__main__':
    main()
