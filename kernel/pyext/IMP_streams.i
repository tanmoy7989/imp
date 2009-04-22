// Typemaps to allow Python director class methods to write to C++ std::ostream
// objects as if they were Python file-like objects; should only be included
// directly by the kernel so that the class is only defined there
%import "IMP_streams_kernel.i"

// Typemaps to allow Python file-like objects to be used for C++ code that
// expects a std::ostream

%typemap(in) std::ostream& (PyOutFileAdapter *tmp=NULL) {
  tmp = new PyOutFileAdapter($input);
  $1 = new std::ostream(tmp);
  $1->exceptions(std::ostream::badbit);
}
%typemap(freearg) std::ostream& {
  if ($1) delete $1;
  if (tmp$argnum) delete tmp$argnum;
}

%typemap(typecheck) std::ostream& = PyObject *;

%{
// Adapter class that acts like a std::streambuf but delegates to a Python
// file-like object, p
class PyOutFileAdapter : public std::streambuf
{
  PyObject *p_;
public:
  PyOutFileAdapter(PyObject *p) : p_(p) { Py_XINCREF(p_); }

  virtual ~PyOutFileAdapter() { Py_XDECREF(p_); }

protected:
  virtual int_type overflow(int_type c) {
    if (c != EOF) {
      char buf[2];
      buf[0] = static_cast<char>(c); buf[1] = '\0';
      xsputn(buf, 1);
    }
    return c;
  }

  virtual std::streamsize xsputn(const char *s, std::streamsize num) {
    // Python API uses char* arguments rather than const char*, so create
    // here to quell the compiler warning
    static char method[] = "write";
    static char fmt[] = "(s#)";
    PyObject *result = PyObject_CallMethod(p_, method, fmt, s, num);
    if (!result) {
      // Python exception will be reraised when SWIG method finishes
      throw std::ostream::failure("Python error on write");
    } else {
      Py_DECREF(result);
    }
    return num;
  }
};
%}


// Typemaps to allow Python file objects to be used for C++ code that
// expects a std::istream
// It is rather expensive to call the Python 'read' method for every character
// (and we cannot read multiple bytes from the Python file, since there is no
// way to put them back if we read too many; even if the stream is seekable
// there is no guarantee we can restore the file position unless it is opened
// in binary mode). Thus, we try to use the underlying FILE pointer (only
// available for real files, not for-like objects) if possible. This may fail
// on Windows where different C runtimes can make FILE pointers unusable:
// http://www.python.org/doc/faq/windows/#pyrun-simplefile-crashes-on-windows-but-not-on-unix-why

%typemap(in) std::istream& (std::streambuf *tmp=NULL) {
  bool real_file;
  try {
    real_file = (PyFile_Check($input) && ftell(PyFile_AsFile($input)) != -1);
  } catch(...) {
    real_file = false;
  }
  if (real_file) {
    tmp = new PyInFileAdapter(PyFile_AsFile($input));
  } else {
    tmp = new PyInFilelikeAdapter($input);
  }
  $1 = new std::istream(tmp);
  $1->exceptions(std::istream::badbit);
}

%typemap(freearg) std::istream& {
  if ($1) delete $1;
  if (tmp$argnum) delete tmp$argnum;
}

%typemap(typecheck) std::istream& = PyObject *;

// Adapter class that acts like a std::streambuf but delegates to C-style
// stdio via a FILE pointer
%{
class PyInFileAdapter : public std::streambuf
{
  FILE *fh_;
public:
  PyInFileAdapter(FILE *fh) : fh_(fh) {}

  virtual ~PyInFileAdapter() {}

protected:
  virtual int_type uflow() {
    return getc(fh_);
  }

  virtual int_type underflow() {
    int c = getc(fh_);
    if (c != EOF) {
      ungetc(c, fh_);
    }
    return c;
  }

  virtual int_type pbackfail(int c) {
    return c == EOF ? EOF : ungetc(c, fh_);
  }

  virtual int_type sync() {
    return fflush(fh_);
  }
};
%}

// Adapter class that acts like a std::streambuf but delegates to a Python
// file-like object
%{
class PyInFilelikeAdapter : public std::streambuf
{
  PyObject *p_;
  // Last character peeked from the stream by underflow(), or -1
  int peeked_;
public:
  PyInFilelikeAdapter(PyObject *p) : p_(p), peeked_(-1) {}

  virtual ~PyInFilelikeAdapter() {
    if (peeked_ != -1) {
      IMP_WARN("One excess character read from Python stream - "
               "cannot be put back.")
    }
  }

protected:
  virtual int_type uflow() {
    int c;
    c = (peeked_ == -1 ? underflow() : peeked_);
    peeked_ = -1;
    return c;
  }

  virtual int_type underflow() {
    static char method[] = "read";
    static char fmt[] = "(i)";
    peeked_ = -1;
    PyObject *result = PyObject_CallMethod(p_, method, fmt, 1);
    if (!result) {
      // Python exception will be reraised when SWIG method finishes
      throw std::ostream::failure("Python error on read");
    } else {
      if (PyString_Check(result)) {
        if (PyString_Size(result) == 1) {
          int c = peeked_ = PyString_AsString(result)[0];
          Py_DECREF(result);
          return c;
        } else {
          Py_DECREF(result);
          return EOF;
        }
      } else {
        Py_DECREF(result);
        PyErr_SetString(PyExc_TypeError, "Python file-like object read method "
                        "should return a string");
        throw std::ostream::failure("Python error on read");
      }
    }
  }
};
%}
