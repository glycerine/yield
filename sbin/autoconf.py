# autoconf.py

# Copyright (c) 2011 Minor Gordon
# All rights reserved

# This source file is part of the Yield project.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
# * Neither the name of the Yield project nor the
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

assert __name__ == "__main__"

from optparse import OptionParser
from os.path import abspath, dirname, join
import sys

try:
    from yutil.autoconf import *
except ImportError:
    MY_DIR_PATH = dirname(abspath(sys.modules[__name__].__file__))
    YIDL_DIR_PATH = abspath(join(MY_DIR_PATH, ".."))
    sys.path.append(join(YIDL_DIR_PATH, "src", "py"))
    from yutil.autoconf import *


# Helper functions
def __Check(check_function, description, *args, **kwds):
    print "Checking for", description, "..."
    if check_function(*args, **kwds):
        print "yes"
        return 0
    else:
        print "no"
        return 1


def __CheckHeader(language, check_function, header):
    print "Checking for", language, "header", header, "..."
    if check_function(header):
        print "yes"
        return 0
    else:
        print "no"
        return 1


option_parser = OptionParser()

option_parser.add_option("--check-header")
option_parser.add_option("--check-c-header")
option_parser.add_option("--check-cxx-header")
option_parser.add_option("--check-declaration")
option_parser.add_option("--check-func")
option_parser.add_option("--check-lib")
option_parser.add_option("--check-lib-with-header")
option_parser.add_option("--check-type")
option_parser.add_option("--check-type-size")
option_parser.add_option("--expect-type-size", type="int", default=0)
option_parser.add_option("--include", action="append", dest="includes")
option_parser.add_option("--language", default="C")

if len(sys.argv) > 1:
    options, ignore = option_parser.parse_args()
else:
    option_parser.print_help()
    sys.exit(0)


ret = 0

if options.check_c_header:
    ret += __CheckHeader("C", CheckCHeader, options.check_c_header)
if options.check_cxx_header:
    ret += __CheckCXXHeader("C++", CheckCXXHeader, options.check_cxx_header)
if options.check_header:
    ret += __CheckHeader(options.language, CheckHeader, options.check_header)

if options.check_declaration:
    ret += __Check(
               CheckDeclaration,
               "declaration " + options.check_declaration,
               options.check_declaration,
               includes=options.includes,
               language=options.language
           )

if options.check_func:
    ret += __Check(
               CheckFunc,
               "function " + options.check_func,
               options.check_func,
               includes=options.includes,
               language=options.language
           )

if options.check_lib:
    ret += __Check(
                CheckLib,
                "library " + options.check_lib,
                options.check_lib,
                includes=options.includes,
                language=options.language
           )

if options.check_lib_with_header:
    ret += __Check(
                CheckLibWithHeader,
                "library " + options.check_lib_with_header + " and header " + ' '.join(options.includes),
                options.check_lib_with_header,
                includes=options.includes,
                language=options.language
           )

if options.check_type:
    ret += __Check(
               CheckType,
               "type " + options.check_type,
               options.check_type,
               includes=options.includes,
               language=options.language
           )

if options.check_type_size:
    ret += __Check(
               CheckTypeSize,
               "type " + options.check_type_size + " with size " + str(options.expect_type_size),
               options.check_type_size,
               expect=options.expect_type_size,
               includes=options.includes
           )
