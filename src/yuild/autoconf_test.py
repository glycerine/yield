# yuild/autoconf_test.py

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

import sys, unittest
from unittest import TestCase, TestSuite

from autoconf import *


# Constants
if sys.platform == "win32":
    C_DECLARATION = "GetLastError"
    C_FUNC = "GetLastError"
    C_HEADER = "windows.h"
    C_LIB = "user32"
    C_TYPE = "DWORD"
    C_TYPE_SIZE = 4
    CXX_HEADER = "string"
else:
    C_DECLARATION = "getuid"
    C_FUNC = "getuid"
    C_HEADER = "unistd.h"
    C_LIB = "dl"
    C_TYPE = "int"
    C_TYPE_SIZE = 4
    CXX_HEADER = "string"



class CheckCHeaderTest(TestCase):
    def runTest(self):
        assert CheckCHeader(C_HEADER)


class CheckCXXHeaderTest(TestCase):
    def runTest(self):
        assert CheckCXXHeader(CXX_HEADER, include_quotes="<>")


class CheckDeclarationTest(TestCase):
    def runTest(self):
        assert CheckDeclaration(C_DECLARATION, includes=C_HEADER, language="C")


class CheckFuncTest(TestCase):
    def runTest(self):
        assert CheckFunc(C_FUNC, includes=C_HEADER, language="C")


class CheckHeaderTest(TestCase):
    def runTest(self):
        assert CheckHeader(C_HEADER, language="C")
        assert CheckHeader(CXX_HEADER, language="C++")


class CheckLibTest(TestCase):
    def runTest(self):
        assert CheckLib(C_LIB, language="C")


class CheckLibWithHeaderTest(TestCase):
    def runTest(self):
        assert CheckLibWithHeader(C_LIB, C_HEADER, language="C")


class CheckTypeTest(TestCase):
    def runTest(self):
        assert CheckType(C_TYPE, includes=C_HEADER, language="C")


class CheckTypeSizeTest(TestCase):
    def runTest(self):
        assert CheckTypeSize(C_TYPE, includes=C_HEADER, language="C", expect=C_TYPE_SIZE)


suite = TestSuite()
suite.addTest(CheckCHeaderTest())
suite.addTest(CheckCXXHeaderTest())
suite.addTest(CheckDeclarationTest())
suite.addTest(CheckFuncTest())
suite.addTest(CheckHeaderTest())
suite.addTest(CheckLibTest())
suite.addTest(CheckLibWithHeaderTest())
suite.addTest(CheckTypeTest())
suite.addTest(CheckTypeSizeTest())
