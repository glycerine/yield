# yidl/target/cpp_target_test.py

# Copyright (c) 2011 Minor Gordon
# All rights reserved

# This source file is part of the yidl project.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
# * Neither the name of the yidl project nor the
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


import os, sys
from cStringIO import StringIO
from hashlib import md5
from unittest import TestSuite

if sys.platform == "win32":
    # Set some environment variables to get distutils to use MSVC 10 correctly
    os.environ["DISTUTILS_USE_SDK"] = "1"
    os.environ["MSSdk"] = "1"

from distutils import ccompiler

from yidl.compiler import compile
from yidl.target.cpp_target import CPPTarget
from yidl.target.target_test import *


class CPPTargetTest(TargetTest):
    def get_cpp(self):
        cpp = repr(compile(self.get_idl(), CPPTarget()))
        cpp = StringIO(cpp).readlines()
        cpp = [cpp_line.strip() for cpp_line in cpp]
        return cpp


class CPPTargetCompileTest(CPPTargetTest):
    def runTest(self):
        cpp = self.get_cpp()
        cpp.append("""\

int main( int, char** )
{
    %(TEST_MODULE)s::%(TEST_INTERFACE)s::%(TEST_EXCEPTION)s test_exception;
    %(TEST_MODULE)s::%(TEST_INTERFACE)s test_interface;
    %(TEST_MODULE)s::%(TEST_SEQUENCE)s test_sequence;
    %(TEST_MODULE)s::%(TEST_STRUCT)s test_struct;
}
""" % globals())
        cpp = '\n'.join(cpp)

        cpp_file_path = md5(cpp).hexdigest() + ".cpp"
        cpp_file = open(cpp_file_path, "w")
        cpp_file.write(cpp)
        cpp_file.close()

        cc = ccompiler.new_compiler()
        try:
            extra_preargs = sys.platform == "win32" and ["/EHsc"] or None
            objects = cc.compile([cpp_file_path], extra_preargs=extra_preargs)
            os.unlink(objects[0])
        finally:
            os.unlink(cpp_file_path)


class CPPTargetExceptionTest(CPPTargetTest):
    def runTest(self):
        cpp = self.get_cpp()
        assert "class %(TEST_EXCEPTION)s : public std::exception" % globals()
        assert "virtual ~%(TEST_EXCEPTION)s() throw() {  }" % globals() in cpp



class CPPTargetHeaderGuardTest(CPPTargetTest):
    def runTest(self):
        cpp = self.get_cpp()
        assert cpp[0].startswith("#ifndef")
        assert cpp[1].startswith("#define")
        assert cpp[-2].startswith("#endif")
        assert len(cpp[2]) == 0
        assert len(cpp[3]) == 0


class CPPTargetInterfaceTest(CPPTargetTest):
    def runTest(self):
        cpp = self.get_cpp()
        assert "class %(TEST_INTERFACE)s" % globals() in cpp
        assert "virtual ~TestException() throw() {  }" in cpp


class CPPTargetModuleTest(CPPTargetTest):
    def runTest(self):
        cpp = self.get_cpp()
        namespace_i = cpp.index("namespace %(TEST_MODULE)s" % globals())
        assert cpp[namespace_i + 1] == "{"
        assert cpp[-4] == "}"


class CPPTargetOperationTest(CPPTargetTest):
    def runTest(self):
        cpp = self.get_cpp()
        assert "virtual void %(TEST_OPERATION)s() { }" % globals() in cpp


class CPPTargetSequenceTest(CPPTargetTest):
    def runTest(self):
        cpp = self.get_cpp()
        assert "#include <string>" in cpp
        assert "#include <vector>" in cpp
        assert "class %(TEST_SEQUENCE)s : public std::vector<std::string>"\
                   % globals() in cpp
        assert "%(TEST_SEQUENCE)s() { }" % globals() in cpp
        assert "%(TEST_SEQUENCE)s( const std::string& first_value )"\
                   % globals() in cpp
        assert "%(TEST_SEQUENCE)s( size_type size )" % globals() in cpp


class CPPTargetStringTest(CPPTargetTest):
    def runTest(self):
        cpp = self.get_cpp()
        assert "#include <string>" in cpp


class CPPTargetStructTest(CPPTargetTest):
    def runTest(self):
        cpp = self.get_cpp()
        assert "class %(TEST_STRUCT)s" % globals() in cpp
        assert "bool operator==( const %(TEST_STRUCT)s& ) const { return true; }"\
                   % globals() in cpp


suite = TestSuite()
suite.addTest(CPPTargetCompileTest())
suite.addTest(CPPTargetExceptionTest())
suite.addTest(CPPTargetHeaderGuardTest())
suite.addTest(CPPTargetInterfaceTest())
suite.addTest(CPPTargetModuleTest())
suite.addTest(CPPTargetOperationTest())
suite.addTest(CPPTargetSequenceTest())
suite.addTest(CPPTargetStringTest())
suite.addTest(CPPTargetStructTest())
