# yuild/autoconf.py

# Copyright (c) 2011 Minor Gordon
# All rights reserved

# This source file is part of the yuild project.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
# * Neither the name of the yuild project nor the
# names of its contributors may be used to endorse or promote products
# derived from this software without specific prior written permission.

# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL Minor Gordon BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

from hashlib import md5

from yutil import quote, quotestrlist, rpad, strlist, typelist


_all_ = [
    "Check",
    "Checks",
    "DeclarationCheck",
    "FunctionCheck",
    "HeaderCheck",
    "LibraryCheck",
]


#if sys.platform == "win32":
#    # Set some environment variables to get distutils to use MSVC 10 correctly
#    os.environ["DISTUTILS_USE_SDK"] = "1"
#    os.environ["MSSdk"] = "1"


class Check(object):
    def __init__(self, name):
        self.__name = name

    def __call__(self):
        # print repr(self)
        exec repr(self)
        return eval(self.name + "()")

    @property
    def name(self):
        return self.__name


class Checks(list):
    def __init__(self, checks=None, name=None):
        if checks is None:
            list.__init__(self)
        else:
            list.__init__(self, checks)
        self.__name = name

    def __call__(self):
        for check in self:
            if not check():
                return False
        return True

    @property
    def name(self):
        name = self.__name
        if name is None:
            name = "checks_" + "_and_".join([check.name for check in self])
        return name

    def __repr__(self):
        check_calls = " and ".join([check.name + "()" for check in self])
        check_reprs = "\n\n".join([repr(check) for check in self])
        name = self.name
        return """\
%(check_reprs)s
def %(name)s():
    return %(check_calls)s
""" % locals()


class _CompileCheck(Check):
    class Include(object):
        def __init__(self, path):
            self.__path = path

        def __str__(self):
            path = self.__path.strip()
            if path.startswith("#include"):
                return path
            else:
                return "#include " + quote(path)

    class Includes(list):
        def __init__(self, paths):
            list.__init__(self, typelist(paths, _CompileCheck.Include))

        def __str__(self):
            return '\n'.join([str(include) for include in self])


    def __init__(self, source, includes=None, language="C", name=None):
        if name is None:
            name = "check_" + md5(source).hexdigest()
        Check.__init__(self, name)
        language = language.lower()
        self.__includes = self.Includes(includes)
        self.__source = source
        if language == "c++": source_file_ext = ".cpp"
        elif language == "c": source_file_ext = ".c"
        else: raise NotImplementedError, language
        self.__source_file_path = name + source_file_ext

    @property
    def includes(self):
        return self.__includes

    def __repr__(self):
        includes = rpad(str(self.includes), "\n\n")
        name = self.name
        source = self.source
        source_file_path = self.source_file_path
        return """\
def %(name)s():
    import distutils.ccompiler, os, sys, traceback
    
    source_file = open("%(source_file_path)s", "w+b")
    source_file.write(\"\"\"\\
%(includes)s%(source)s
\"\"\")
    source_file.close()
    
    try:
        os.unlink(
            distutils.ccompiler.new_compiler().compile(
                ["%(source_file_path)s"],
                extra_preargs=(sys.platform == "win32" and ["/EHsc"] or None)
            )[0]
        )
        return True
    except:
        traceback.print_exc()
        return False
    finally:
        os.unlink("%(source_file_path)s")
""" % locals()

    @property
    def source(self):
        return self.__source

    @property
    def source_file_path(self):
        return self.__source_file_path


class _CompileLinkCheck(_CompileCheck):
    def __init__(
        self,
        source,
        includes=None,
        language="C",
        libraries=None,
        name=None
    ):
        _CompileCheck.__init__(self, source, includes, language, name)
        self.__libraries = strlist(libraries)

    @property
    def libraries(self):
        return self.__libraries

    def __repr__(self):
        includes = rpad(str(self.includes), "\n\n")
        libraries = '[' + ', '.join(quotestrlist(self.libraries)) + ']'
        name = self.name
        source = self.source
        source_file_path = self.source_file_path
        return """\
def %(name)s():
    import distutils.ccompiler, os.path, sys, tempfile, traceback
    
    try:
        source_file = open("%(source_file_path)s", "w+b")
        source_file.write(\"\"\"\\
%(includes)s%(source)s
\"\"\")
        source_file.close()

        cc = distutils.ccompiler.new_compiler()
        
        object = \
            cc.compile(
                ["%(source_file_path)s"],
                extra_preargs=(sys.platform == "win32" and ["/EHsc"] or None)
            )[0]
    
        try:
            bin_file = \
                tempfile.NamedTemporaryFile(
                    suffix=(sys.platform == "win32" and ".exe" or "")
                )
            
            try:
                cc.link_executable(
                    [object],
                    bin_file.name,
                    extra_preargs=(sys.platform == "win32" and ["/MANIFEST"] or None),
                    libraries=%(libraries)s
                )
                if sys.platform == "win32":
                    os.unlink(bin_file.name + ".manifest")
                return True
            finally:
                bin_file.close()
        finally:
            os.unlink(object)
    except:
        traceback.print_exc()
        return False
    finally:
        os.unlink("%(source_file_path)s")
""" % locals()


class DeclarationCheck(_CompileCheck):
    def __init__(self, symbol, includes=None, language="C"):
        _CompileCheck.__init__(
            includes=includes,
            language=language,
            source="""\
int main() {
#ifndef %(symbol)s
    ( void )%(symbol)s;
#endif
    ;
    return 0;
}            
""")


class FunctionCheck(_CompileLinkCheck):
    def __init__(self, function, includes=None, language="C"):
        _CompileLinkCheck(
            includes=includes,
            language=language,
            source="""\
#include <assert.h>

int main() {
#if defined (_stub_%(function)s) || defined(_stub__%(function)s)
  fail fail fail
#else
  %(function)s();
#endif
  return 0;
}
""")


class HeaderCheck(_CompileCheck):
    def __init__(self, include, language="C"):
        _CompileCheck.__init__(
            self,
            includes=include,
            language=language,
            source="int main(int argc, char** argv) { }"
        )


class LibraryCheck(_CompileLinkCheck):
    def __init__(
        self,
        library,
        call=None,
        includes=None,
        language="C",
        symbol=None
    ):
        source = []
        if symbol is not None and symbol != "main":
            if includes is None or len(includes) == 0:
                source.append("""\
#ifdef _cplusplus
extern "C"
#endif
char %(symbol)s();
""" % locals())

                if call is None:
                    call = "%(symbol)s(); " % locals()
        if call is None:
            call = ""
        source.append("""\
int main(int argc, char** argv) { %(call)sreturn 0; }
""" % locals())

        _CompileLinkCheck.__init__(
            self,
            includes=includes,
            language=language,
            libraries=[library],
            source='\n'.join(source)
        )


#def Type(name, includes=None, language="C"):
#    includes = _str_includes(includes, language)
#    return _compile_link_run_c("""\
#%(includes)sint main( int argc, char** argv )
#{
#  if ( sizeof( %(name)s ) > 0 )
#    return 0;
#  else
#    return 1;
#}
#""" % locals(), language=language) == 0
#
#
#def TypeSize(name, includes=None, language="C", expect=0):
#    includes = _str_includes(includes, language)
#    return _compile_link_run_c("""\
#%(includes)stypedef %(name)s check_type;
#
#int main( int argc, char** argv )
#{
#  static int test_array[1 - 2 * !(((long int) (sizeof(check_type))) == %(expect)u)];
#  test_array[0] = 0;
#  return 0;
#}
#""" % locals(), language=language) == 0
