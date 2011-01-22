# yidl/target/target_test.py

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


from os.path import abspath, dirname, join
import sys
from unittest import TestCase


__all__ = \
[
    "TargetTest",
    "TEST_CONSTANT_NAME",
    "TEST_CONSTANT_VALUE",
    "TEST_EXCEPTION",
    "TEST_INTERFACE",
    "TEST_MODULE",
    "TEST_OPERATION",
    "TEST_SEQUENCE",
    "TEST_STRUCT",
]


# Constants
TEST_CONSTANT_NAME = "TEST_CONSTANT"
TEST_CONSTANT_VALUE = "1"
TEST_EXCEPTION = "TestException"
TEST_INTERFACE = "TestInterface"
TEST_MODULE = "test_module"
TEST_OPERATION = "TestOperation"
TEST_SEQUENCE = "TestSequence"
TEST_STRUCT = "TestStruct"



class TargetTest(TestCase):
    def __init__(self):
        TestCase.__init__(self)
        my_dir_path = abspath(dirname(sys.modules[__name__].__file__))
        self.__idl_file_path = join(my_dir_path, "target_test.idl")

    def get_idl(self):
        return self.__idl_file_path
