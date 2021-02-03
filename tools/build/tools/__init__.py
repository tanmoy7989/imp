import glob
import os
import os.path
import copy
import itertools
import shutil
import sys
import difflib
import subprocess
import signal
import weakref

# cmake paths are always /-separated; on platforms where the path is not /
# (e.g. Windows) convert a path to or from a form cmake will like
if os.sep == '/':
    def to_cmake_path(p):
        return p

    def from_cmake_path(p):
        return p
else:
    def to_cmake_path(p):
        return p.replace(os.sep, '/')

    def from_cmake_path(p):
        return p.replace('/', os.sep)


def get_existing_links(path):
    """Get any symlinks in the given directory"""
    return [f for f in glob.glob(os.path.join(path, "*")) if os.path.islink(f)]


def mkdir(path, clean=True):
    if os.path.isdir(path):
        if clean:
            # remove any old links
            for f in glob.glob(os.path.join(path, "*")):
                if os.path.islink(f):
                    os.unlink(f)
            # Remove old lists of Python tests
            for f in glob.glob(os.path.join(path, "*.pytests")):
                os.unlink(f)
        else:
            # clean broken links
            for f in glob.glob(os.path.join(path, "*")):
                if os.path.islink(f) and not os.path.exists(os.readlink(f)):
                    os.unlink(f)
        return
    if os.path.isfile(path):
        os.unlink(path)
    os.makedirs(path)


def quote(st):
    out = st.replace(
        "\\f",
        ""). replace(
        "\\b",
        "").replace(
        "\\",
        "\\\\").replace(
        "\"",
        "\\\"")
    return out


def get_glob(patterns):
    ret = []
    for p in patterns:
        c = sorted(glob.glob(p))
        ret += c
    return ret


def rewrite(filename, contents, verbose=True):
    if os.path.islink(filename):
        os.unlink(filename)
        if verbose:
            print("    Was symlink - now new file: " + filename)
    try:
        old = open_utf8(filename, "r").read()
        if old == contents:
            return
        elif verbose:
            print("    Different " + filename)
            for l in difflib.unified_diff(old.split("\n"), contents.split("\n")):
                stl = str(l)
                if (stl[0] == '-' or stl[0] == '+') and stl[1] != '-' and stl[1] != '+':
                    print("    " + stl)
    except:
        pass
        # print "Missing", filename
    dirpath = os.path.split(filename)[0]
    if dirpath != "":
        mkdir(dirpath, False)
    open_utf8(filename, "w").write(contents)

class FileGenerator(object):
    """Auto-generate an output file.
       The file is marked as being auto-generated with a suitable comment.
       `template_file` names an input file containing a suitable template
       (or None) and `start_comment` specifies the string used to start
       a single-line comment (e.g. #, ;, //)
    """
    def __init__(self, template_file, start_comment):
        self.template_file = template_file
        self.start_comment = start_comment
        if template_file:
            self.template = open(template_file).read()
            self.template_file = os.path.relpath(template_file,
                                                 self.get_imp_top())

    def get_imp_top(self):
        ourdir = os.path.dirname(__file__)
        return os.path.abspath(os.path.join(ourdir, '..', '..', '..'))

    def get_gen_prog(self):
        """Get the name of the program that generated the file"""
        return os.path.relpath(sys.argv[0], self.get_imp_top())

    def get_output_file_contents(self, output):
        if self.template_file:
            return self.template % output
        else:
            return output

    def _write_file(self, outfile, contents, show_diff):
        if show_diff:
            rewrite(outfile, contents)
        else:
            with open(outfile, 'w') as fh:
                fh.write(contents)

    def write(self, outfile, output, show_diff=True):
        """Write the output file `outfile` using `output`.
           If `template_file` specified a file above, read the contents of that
           file and make Python-style substitutions using output (which should
           be a dict). If `template_file` is None, simply write `output` to the
           file."""
        if self.template_file:
            autogen = '%s Autogenerated by %s\n%s from %s\n' \
                      '%s Do not edit - any changes will be lost!\n\n' \
                      % (self.start_comment, self.get_gen_prog(),
                         self.start_comment, self.template_file,
                         self.start_comment)
        else:
            autogen = '%s Autogenerated by %s\n' \
                      '%s Do not edit - any changes will be lost!\n\n' \
                      % (self.start_comment, self.get_gen_prog(),
                         self.start_comment)
        self._write_file(outfile,
                         autogen + self.get_output_file_contents(output),
                         show_diff)


class CMakeFileGenerator(FileGenerator):
    def __init__(self, template_file=None):
        FileGenerator.__init__(self, template_file, '#')

class CPPFileGenerator(FileGenerator):
    def __init__(self, template_file=None):
        FileGenerator.__init__(self, template_file, '//')

class SWIGFileGenerator(FileGenerator):
    def __init__(self, template_file=None):
        FileGenerator.__init__(self, template_file, '//')

class DoxFileGenerator(FileGenerator):
    def __init__(self, template_file=None):
        FileGenerator.__init__(self, template_file, '//')

class PythonFileGenerator(FileGenerator):
    def __init__(self, template_file=None):
        FileGenerator.__init__(self, template_file, '#')


def rmdir(path):
    try:
        shutil.rmtree(path)
    except:
        pass


def link(source, target, verbose=False):
    tpath = os.path.abspath(target)
    spath = os.path.abspath(source)
    # print tpath, spath
    if not os.path.exists(spath):
        if verbose:
            print("no source", spath)
        return
    if os.path.islink(tpath):
        if os.readlink(tpath) == spath:
            return
        else:
            os.unlink(tpath)
    elif os.path.isdir(tpath):
        shutil.rmtree(tpath)
    elif os.path.exists(tpath):
        os.unlink(tpath)
    if verbose:
        print("linking", spath, tpath)
    if hasattr(os, 'symlink') and sys.platform != 'win32':
        # Python 3 on Windows has os.symlink but it doesn't always work
        os.symlink(spath, tpath)
    # Copy instead of link on platforms that don't support symlinks (Windows)
    elif os.path.isdir(spath):
        def excludes(src, names):
            return ['.svn', 'data']
        shutil.copytree(spath, tpath, ignore=excludes)
    else:
        shutil.copy(spath, tpath)

def has_python_hashbang(fname):
    line = open(fname).readline()
    return line.startswith('#!') and 'python' in line

def filter_pyapps(fname):
    """A Python application ends in .py, or starts with #!(something)python;
       exclude dependencies.py."""
    return os.path.isfile(fname) and not fname.endswith('dependencies.py') \
           and (fname.endswith('.py') or has_python_hashbang(fname))

def link_dir(source_dir, target_dir, match=["*"], exclude=[],
             clean=True, verbose=False, filt=None, make_subdirs=False):
    if not isinstance(match, list):
        raise TypeError("Expecting a list object for match")
    exclude = exclude + ["SConscript", "CMakeLists.txt", "Files.cmake", ".svn"]
    # print "linking", source_dir, target_dir
    mkdir(target_dir, clean=False)
    if clean:
        existing_links = get_existing_links(target_dir)
    # Don't clean links here, as that forces any valid symlinks to be
    # recreated (potentially forcing a useless rebuild). We'll handle them
    # at the end of this function.
    files = []
    targets = {}
    for m in match:
        files.extend(get_glob([os.path.join(source_dir, m)]))
    if filt:
        files = [x for x in files if filt(x)]
    for g in files:
        name = os.path.split(g)[1]
        if name not in exclude:
            target = os.path.join(target_dir, name)
            targets[target] = None
            if os.path.isdir(g) and make_subdirs:
                if os.path.islink(target):
                    os.unlink(target)
                link_dir(g, target, match, exclude, clean=clean,
                         verbose=verbose, filt=filt, make_subdirs=True)
            else:
                link(g, target, verbose=verbose)
    if clean:
        # Remove any old links that are no longer valid
        for ln in existing_links:
            if ln not in targets:
                os.unlink(ln)

class Module(object):
    """An IMP module"""
    _info = None

    def __init__(self, name, path, finder):
        self.name, self.path = name, path
        # finder holds a reference to us, so we need a weak
        # reference here to break cycles
        self._finder = weakref.ref(finder)

    def get_all_modules(self):
        """Get a list of all other Modules this one depends on.
           The list is sorted, the most derived Modules last."""
        ret = []
        stack = [self]
        while len(stack) > 0:
            cur = stack.pop()
            for m in cur.required_modules + cur.optional_modules:
                if m not in ret:
                    ret.append(m)
                    stack.append(m)
        return self._finder().get_ordered(ret)

    def _modules_split(self, s):
        """Split the given string into a list of Module objects"""
        finder = self._finder()
        def get_module(name):
            try:
                return finder[name]
            except KeyError:
                raise KeyError("module %s depends on module %s, which "
                               "does not exist" % (self.name, name))
        return [get_module(x) for x in split(s)]


class ConfiguredModule(Module):
    """An IMP module that has already been configured (no source code)"""
    build_info_file = property(lambda self: os.path.join(self.path,
                                                         "IMP.%s" % self.name))
    # Already configured
    configured = property(lambda self: self)

    def _read_bi_file(self, attr):
        """Read the build_info file if necessary and return the given
           attribute"""
        if self._info is not None:
            return self._info[attr]
        d = {'modules': "", 'unfound_modules': "",
             'dependencies': "", 'unfound_dependencies': "",
             'swig_wrapper_includes': "", 'swig_includes': "",
             'swig_path': "", 'include_path': "", 'lib_path': "", 'ok': False,
             'python_only': False}
        exec(open(self.build_info_file).read(), d)
        self._info = {"ok": d['ok'],
               "modules": self._modules_split(d['modules']),
               "unfound_modules": self._modules_split(d['unfound_modules']),
               "dependencies": split(d['dependencies']),
               "unfound_dependencies": split(d['unfound_dependencies']),
               "swig_includes": split(d['swig_includes']),
               "swig_wrapper_includes": split(d['swig_wrapper_includes']),
               "python_only": d['python_only']}
        return self._info[attr]

    ok = property(lambda self: self._read_bi_file('ok'))
    modules = property(lambda self: self._read_bi_file('modules'))
    unfound_modules = property(lambda self:
                                  self._read_bi_file('unfound_modules'))
    required_modules = modules
    dependencies = property(lambda self: self._read_bi_file('dependencies'))
    unfound_dependencies = property(lambda self:
                                 self._read_bi_file('unfound_dependencies'))
    swig_wrapper_includes = property(lambda self:
                                 self._read_bi_file('swig_wrapper_includes'))
    swig_includes = property(lambda self: self._read_bi_file('swig_includes'))
    python_only = property(lambda self: self._read_bi_file('python_only'))
    required_dependencies = dependencies
    optional_modules = []
    optional_dependencies = []


class ExternalModule(ConfiguredModule):
    """A configured module that has been built externally.
       These are usually used to refer to core IMP modules from
       code being built outside of the IMP tree."""
    pass


class SourceModule(Module):
    """An IMP module that exists in an IMP source checkout"""
    depends_file = property(lambda self: os.path.join(self.path,
                                                      "dependencies.py"))

    def __get_configured_module(self):
        if not hasattr(self, '_configured'):
            self._configured = ConfiguredModule(self.name, 'build_info',
                                                self._finder())
        return self._configured

    configured = property(__get_configured_module,
                          doc="Configured version of this module")

    def _read_dep_file(self, attr):
        """Read the depends file if necessary and return the given
           attribute"""
        if self._info is not None:
            return self._info[attr]
        d = {'required_modules': "", 'optional_modules': "",
             'required_dependencies': "", 'optional_dependencies': "",
             'lib_only_required_modules': "", 'python_only': False}
        exec(open(self.depends_file).read(), d)
        self._info = {"required_modules":
                             self._modules_split(d['required_modules']),
                      "lib_only_required_modules":
                           self._modules_split(d['lib_only_required_modules']),
                      "optional_modules":
                             self._modules_split(d['optional_modules']),
                      "required_dependencies":
                             split(d['required_dependencies']),
                      "optional_dependencies":
                             split(d['optional_dependencies']),
                      "python_only": d['python_only']}
        return self._info[attr]

    required_modules = property(
                        lambda self: self._read_dep_file('required_modules'))
    lib_only_required_modules = property(
                lambda self: self._read_dep_file('lib_only_required_modules'))
    optional_modules = property(
                        lambda self: self._read_dep_file('optional_modules'))
    required_dependencies = property(
                      lambda self: self._read_dep_file('required_dependencies'))
    optional_dependencies = property(
                      lambda self: self._read_dep_file('optional_dependencies'))
    python_only = property(lambda self: self._read_dep_file('python_only'))


class ModulesFinder(object):
    """Class for finding IMP modules.
       Acts like a dict with module names as keys and Module objects as values.

       `source_dir`, if given, is the relative path to search for source
                     modules
       `configured_dir`, if given, is the relative path to search for configured
                     modules
       `external_dir`, if given, is the relative path to search for external
                     modules
       `module_name`, if given, overrides the automatically-determined name
                     of the single module in `source_dir` when `external_dir`
                     is set
    """
    def __init__(self, source_dir=None, configured_dir=None, external_dir=None,
                 module_name=None):
        self.source_dir = source_dir
        self.configured_dir = configured_dir
        self.external_dir = external_dir
        self.module_name = module_name
        # If False, search for modules in subdirectories under
        # `source_dir`; if True, `source_dir` is a single module
        self.one_module = (external_dir is not None and source_dir is not None
                           and os.path.exists(os.path.join(source_dir,
                                                           'dependencies.py')))
        self._mod_by_name = None
        self._ordered = None

    def get_dependency_info(self, dependency):
        """Get a dict describing the named third-party dependency"""
        return get_dependency_info(dependency, self.external_dir or '')

    def get_all_dependencies(self, modules):
        """Return the dependencies of the given modules (plus any modules
           they in turn depend on)"""
        seen_deps = set()
        for m in modules:
            for d in m.required_dependencies + m.optional_dependencies:
                if d not in seen_deps:
                    seen_deps.add(d)
                    yield d

    def get_dependent_modules(self, modules, root='.'):
        """Get the given modules plus any modules they depend on,
           sorted by dependency (most derived modules first)"""
        sorted_order = get_sorted_order(root)
        new_modules = modules[:]
        all_modules = modules[:]
        while len(new_modules) > 0:
            m = new_modules.pop()
            cur_modules = [x for x in m.configured.modules
                           if x not in all_modules]
            all_modules += cur_modules
            new_modules += cur_modules
        return sorted(all_modules, key=lambda x: sorted_order.index(x.name),
                      reverse=True)

    def get_dependent_dependencies(self, modules, dependencies):
        """Get the given dependencies plus those of all given modules"""
        ret_names = []
        for m in modules:
            ret_names.extend(m.configured.dependencies)
        return list(set(ret_names + dependencies))

    def __getitem__(self, name):
        self.__get_all()
        return self._mod_by_name[name]

    def keys(self):
        self.__get_all()
        return self._mod_by_name.keys()

    def values(self):
        self.__get_all()
        return self._mod_by_name.values()

    def items(self):
        self.__get_all()
        return self._mod_by_name.items()

    def get_ordered(self, modules=None):
        """Get Modules sorted by dependencies. Any Module x that depends on
           another Module y will be sorted after y in the list.
           If modules is None, sort all known modules."""
        if self._ordered is None:
            data = {}
            for m in self.values():
                data[m.name] = [x.name for x in m.required_modules
                                                + m.optional_modules]
            self._ordered = [self._mod_by_name[name]
                             for name in toposort2(data)]
        if modules is None:
            return self._ordered
        else:
            return sorted(modules, key=lambda x: self._ordered.index(x))

    def __get_all(self):
        """Get all modules, as Module objects"""
        if self._mod_by_name is not None:
            return
        self._mod_by_name = {}
        for m in itertools.chain(self._get_all_source(),
                                 self._get_all_configured(),
                                 self._get_all_external()):
            # Ignore duplicated modules (a source module should take
            # precedence over an external one)
            if m.name not in self._mod_by_name:
                self._mod_by_name[m.name] = m

    def _get_all_external(self):
        """Get all external modules"""
        if self.external_dir is None:
            return
        for g in glob.glob(os.path.join(self.external_dir, "IMP.*")):
            yield ExternalModule(os.path.split(g)[1][4:], self.external_dir,
                                 self)

    def _get_all_configured(self):
        """Get all configured modules"""
        if self.configured_dir is None:
            return
        for g in glob.glob(os.path.join(self.configured_dir, "IMP.*")):
            yield ConfiguredModule(os.path.split(g)[1][4:], self.configured_dir,
                                   self)

    def _get_all_source(self):
        """Get all source modules"""
        if self.source_dir is None:
            return
        if self.one_module:
            yield SourceModule(self.module_name
                         or os.path.split(os.path.abspath(self.source_dir))[1],
                         self.source_dir, self)
        else:
            for g in glob.glob(os.path.join(self.source_dir, "modules", "*")):
                if (os.path.isdir(g)
                    and os.path.exists(os.path.join(g, "dependencies.py"))):
                    yield SourceModule(os.path.split(g)[1], g, self)


def get_modules(source):
    path = os.path.join(source, "modules", "*")
    globs = get_glob([path])
    mods = [(os.path.split(g)[1], g) for g in globs if (os.path.split(g)[1] != "SConscript")
            and os.path.exists(os.path.join(g, "dependencies.py"))]
    return mods


def get_dependency_description(path):
    d = {'pkg_config_name':  None, 'headers': "", 'libraries': "",
         'extra_libraries': "", 'version_cpp': "", 'version_headers': "",
         'body': "", 'python_module': "", 'is_cmake': False,
         'name': os.path.splitext(os.path.split(path)[1])[0]}
    exec(open(path, "r").read(), d)
    passlibs = split(d['libraries'])
    passheaders = split(d['headers'])
    extra_libs = split(d['extra_libraries'])
    cmakef = os.path.splitext(path)[0] + ".cmake"
    if os.path.exists(cmakef):
        cmake = "include(\"${PROJECT_SOURCE_DIR}/%s\")" % (
            to_cmake_path(os.path.splitext(path)[0] + ".cmake"))
    else:
        cmake = ""
    if d['pkg_config_name'] is None:
        pkgconfigs = (d['name'].lower(),)
    else:
        pkgconfigs = split(d['pkg_config_name'])
    return {"name": d['name'],
            "full_name": d.get('full_name', d['name']),
            "pkg_config_name": pkgconfigs,
            "headers": passheaders,
            "libraries": passlibs,
            "extra_libraries": extra_libs,
            "body": d['body'],
            "version_cpp": split(d['version_cpp']),
            "version_headers": split(d['version_headers']),
            "cmake": cmake,
            "python_module": d['python_module']}

dependency_info_cache = {}

def get_dependency_info(dependency, extra_data_path, root="."):
    global dependency_info_cache
    if dependency in dependency_info_cache:
        return dependency_info_cache[dependency]
    df = os.path.join(root, "build_info", dependency)
    if not os.path.exists(df) and extra_data_path != "":
        df = os.path.join(extra_data_path, dependency)
    d = {'libraries': "", 'version': "", 'includepath': "",
         'libpath': "", 'swigpath': "", 'ok': False,
         'python_only': False}
    # try:
    exec(open(df, "r").read(), d)
    # except:
    #    print >> sys.stderr, "Error reading dependency", dependency, "at", df
    ret = {"ok": d['ok'],
           "libraries": split(d['libraries']),
           "version": split(d['version']),
           "includepath": d['includepath'],
           "libpath": d['libpath'],
           "swigpath": d['swigpath'],
           "python_only": d['python_only']}
    dependency_info_cache[dependency] = ret
    return ret

module_info_cache = {}


def get_module_info(module, extra_data_path, root="."):
    global module_info_cache
    if module in module_info_cache:
        return module_info_cache[module]
    if module.find("/") != -1:
        raise ValueError("module name invalid: " + module)
    df = os.path.join(root, "build_info", "IMP." + module)
    external = False
    if not os.path.exists(df) and extra_data_path != "":
        external = True
        df = os.path.join(extra_data_path, "build_info", "IMP." + module)
    d = {'modules': "", 'unfound_modules': "",
         'dependencies': "", 'unfound_dependencies': "",
         'swig_wrapper_includes': "", 'swig_includes': "",
         'swig_path': "", 'include_path': "", 'lib_path': "", 'ok': False}
    exec(open(df, "r").read(), d)
    ret = {"ok": d['ok'],
           "modules": split(d['modules']),
           "unfound_modules": split(d['unfound_modules']),
           "dependencies": split(d['dependencies']),
           "unfound_dependencies": split(d['unfound_dependencies']),
           "swig_includes": split(d['swig_includes']),
           "swig_wrapper_includes": split(d['swig_wrapper_includes'])}
    if external:
        ret["external"] = True
    module_info_cache[module] = ret
    return ret


# a version of split that doesn't return empty strings when there are no items


def split(string, sep=":"):
    return (
        [x.replace("@", ":")
         for x in string.replace("\:", "@").split(sep) if x != ""]
    )


def toposort2(data):
    ret = []
    while True:
        ordered = [item for item, dep in data.items() if not dep]
        if not ordered:
            break
        ret.extend(sorted(ordered))
        d = {}
        for item, dep in data.items():
            if item not in ordered:
                d[item] = [x for x in dep if x not in ordered]
        data = d
    return ret

order_cache = None


def get_sorted_order(root="."):
    global order_cache
    if order_cache:
        return order_cache
    order = split(open(os.path.join(root, "build_info",
                                    "sorted_modules"), "r").read(), "\n")
    order_cache = order
    return order


def set_sorted_order(sorted,
                     outpath=os.path.join("build_info", "sorted_modules")):
    global order_cache
    order_cache = sorted
    rewrite(outpath,
            "\n".join(sorted), verbose=False)

def get_module_version(module, source_dir):
    in_module_source = os.path.join(source_dir, "modules", module, "VERSION")
    in_module_build = os.path.join("modules", module, "VERSION")
    in_source = os.path.join(source_dir, "VERSION")
    in_build = "VERSION"
    if os.path.exists(in_module_source):
        return open(in_module_source, "r").read().strip()
    elif os.path.exists(in_module_build):
        return open(in_module_build, "r").read().strip()
    elif os.path.exists(in_source):
        return open(in_source, "r").read().strip()
    else:
        return open(in_build, "r").read().strip()


def get_disabled_modules(extra_data_path, root="."):
    all = get_glob([os.path.join(root, "build_info", "IMP.*")])
    modules = [os.path.splitext(a)[1][1:] for a in all]
    return (
        [x for x in modules if not get_module_info(
            x, extra_data_path, root)["ok"]]
    )

# Treat an open file as UTF8-encoded, regardless of the locale
if sys.version_info[0] >= 3:
    def open_utf8(fname, *args):
        return open(fname, *args, encoding='UTF8')
else:
    open_utf8 = open

_subprocesses = []


def run_subprocess(command, **kwargs):
    global _subprocesses
    # if not kwargs.has_key("stdout"):
    #    kwargs["stdout"] = subprocess.PIPE
    # if not kwargs.has_key("stderr"):
    #    kwargs["stderr"] = subprocess.PIPE
    pro = subprocess.Popen(command, stderr=subprocess.PIPE,
                           stdout=subprocess.PIPE, universal_newlines=True,
                           **kwargs)
    _subprocesses.append(pro)
    output, error = pro.communicate()
    ret = pro.returncode
    if ret != 0:
        sys.stderr.write(error + '\n')
        raise OSError("subprocess failed with return code %d: %s\n%s"
                      % (ret, " ".join(command), error))
    return output


def _sigHandler(signum, frame):
    print("starting handler")
    signal.signal(signal.SIGTERM, signal.SIG_DFL)
    global _subprocesses
    for p in _subprocesses:
        print("killing", p)
        try:
            os.kill(p.pid, signal.SIGTERM)
        except:
            pass
    sys.exit(1)

signal.signal(signal.SIGTERM, _sigHandler)
