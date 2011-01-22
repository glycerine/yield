# yuild/autoconf.py

# Copyright (c) 2010 Minor Gordon
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


import os, subprocess, sys, traceback
from glob import glob
from hashlib import md5
from os.path import abspath, join, splitdrive, splitext

if sys.platform == "win32":
    # Set some environment variables to get distutils to use MSVC 10 correctly
    os.environ["DISTUTILS_USE_SDK"] = "1"
    os.environ["MSSdk"] = "1"

from distutils import ccompiler


__all__ = \
[
    "CheckCHeader",
    "CheckCXXHeader",
    "CheckDeclaration",
    "CheckFunc",
    "CheckHeader",
    "CheckLib",
    "CheckLibWithHeader",
    "CheckType",
    "CheckTypeSize",
]


# Constants
DEBUG = False


def CheckCHeader(includes, include_quotes='""'):
    return CheckHeader(includes, include_quotes, "C")

def CheckCXXHeader(includes, include_quotes='""'):
    return CheckHeader(includes, include_quotes, "C++")

def CheckDeclaration(symbol, includes=None, language="C"):
    includes = __make_includes(includes, language)
    return __compile_c("""\
%(includes)sint main()
{
#ifndef %(symbol)s
    ( void )%(symbol)s;
#endif
    ;
    return 0;
}
""" % locals(), language=language)

def CheckFunc(name, includes=None, language="C"):
    includes = __make_includes(includes)
    return __compile_link_c("""\
%(includes)s#include <assert.h>

int main()
{
#if defined ( __stub_%(name)s ) || defined( __stub___%(name)s )
  fail fail fail
#else
  %(name)s();
#endif
  return 0;
}

""" % locals(), language=language)

def CheckHeader(includes, include_quotes='""', language="C"):
    includes = __make_includes(includes, language, quotes=include_quotes)
    if language == "C" or language == "C++":
        return __compile_c("""\
%(includes)sint main( int argc, char** argv ) { }
""" % locals(), language=language)
    else:
        raise NotImplementedError

def CheckLib(
    library,
    call=None,
    includes=None,
    language="C",
    symbol=None,
    *args,
    **kwds
):
    includes = __make_includes(includes)

    c = []
    if symbol is not None and symbol != "main":
        if len(includes) == 0:
            c.append("""\
#ifdef __cplusplus
extern "C"
#endif
char %(symbol)s();
""" % locals())

        if call is None:
            call = "%(symbol)s(); " % locals()

    if call is None:
        call = ""

    c.append("""\
%(includes)sint main( int argc, char** argv ) { %(call)sreturn 0; }
""" % locals())

    c = '\n'.join(c)

    return __compile_link_c(c, language=language, libraries=[library])

def CheckLibWithHeader(
    library,
    includes,
    call=None,
    language="C",
    *args,
    **kwds
):
    return CheckLib(
               library,
               call=call,
               includes=includes,
               language=language,
           )

def CheckType(name, includes=None, language="C"):
    includes = __make_includes(includes, language)
    return __compile_link_run_c("""\
%(includes)sint main( int argc, char** argv )
{
  if ( sizeof( %(name)s ) > 0 )
    return 0;
  else
    return 1;
}
""" % locals(), language=language) == 0

def CheckTypeSize(name, includes=None, language="C", expect=0):
    includes = __make_includes(includes, language)
    return __compile_link_run_c("""\
%(includes)stypedef %(name)s check_type;

int main( int argc, char** argv )
{
  static int test_array[1 - 2 * !(((long int) (sizeof(check_type))) == %(expect)u)];
  test_array[0] = 0;
  return 0;
}
""" % locals(), language=language) == 0

def __compile_c(c, language="C", last_step=True):
    try:
        c_file_path = md5(c).hexdigest()
        if language == "C": c_file_path += ".c"
        elif language == "C++": c_file_path += ".cpp"
        else: raise NotImplementedError, language
        c_file = open(c_file_path, "w")
        c_file.write(c)
        c_file.close()

        if DEBUG:
            print __name__, "compiling"
            print c
            print "in", c_file_path

        if sys.platform == "win32":
            extra_preargs = ["/EHsc"]
        else:
            extra_preargs = None

        cc = ccompiler.new_compiler()
        try:
            objects = cc.compile([c_file_path], extra_preargs=extra_preargs)
            assert len(objects) == 1
            if last_step:
                os.unlink(objects[0])
                return True
            else:
                return cc, c_file_path, objects[0]
        finally:
            if last_step:
                os.unlink(c_file_path)
    except:
        if DEBUG: traceback.print_exc()
        return False

def __compile_link_c(c, language="C", last_step=True, libraries=None):
    if libraries is None: libraries = []

    compile_c_ret = __compile_c(c, language=language, last_step=False)
    if compile_c_ret:
        cc, c_file_path, o_file_path = compile_c_ret
        exe_file_path = splitext(c_file_path)[0]

        if DEBUG:
            print __name__, "linking", o_file_path, "into", exe_file_path

        if sys.platform == "win32":
            extra_preargs = ("/MANIFEST",) # Get around a bug in msvc9compiler
        else:
            extra_preargs = None

        try:
            cc.link_executable(
                [o_file_path],
                exe_file_path,
                extra_preargs=extra_preargs,
                libraries=libraries
            )

            if sys.platform == "win32" and not exe_file_path.endswith(".exe"):
                exe_file_path += ".exe"
            else:
                exe_file_path = abspath(exe_file_path)

            if last_step:
                os.unlink(exe_file_path)
                if sys.platform == "win32":
                    os.unlink(exe_file_path + ".manifest")
                return True
            else:
                return exe_file_path
        except:
            if DEBUG: traceback.print_exc()
            return False
        finally:
            os.unlink(c_file_path)
            os.unlink(o_file_path)

    else:
        return False

def __compile_link_run_c(c, language="C", libraries=None):
    exe_file_path = __compile_link_c(c, language=language, last_step=False, libraries=libraries)
    if exe_file_path:
        if DEBUG:
            print __name__, "running", exe_file_path

        try:
            return subprocess.call(exe_file_path)
        except:
            if DEBUG: traceback.print_exc()
            return 1
        finally:
            os.unlink(exe_file_path)
            if sys.platform == "win32":
                os.unlink(exe_file_path + ".manifest")
    else:
        return False

def __make_include(include, language="C", quotes='""'):
    if include is not None:
        include = include.strip()

        if language == "C" or language == "C++":
            if include.startswith("#include"):
                return include
            else:
                return '#include ' + quotes[0] + include.replace('"', '\\"') + quotes[1]
        else:
            raise NotImplementedError
    else:
        return ""

def __make_includes(includes, language="C", quotes='""'):
    if includes is None:
        includes = []
    elif isinstance(includes, basestring):
        includes = [__make_include(includes, language, quotes)]
    elif isinstance(includes, list) or isinstance(includes, tuple):
        includes = [__make_include(include, language, quotes) for include in includes]
    else:
        raise TypeError, str(type(includes))

    includes = "\n".join(includes)
    if len(includes) > 0: includes += "\n\n"
    return includes
