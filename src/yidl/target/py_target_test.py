# yidl/target/py_target_test.py

# Copyright (c) 2010 Minor Gordon
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


from cStringIO import StringIO
from unittest import TestSuite

from yidl.compiler import compile
from yidl.target.py_target import PyTarget
from yidl.target.target_test import *


class PyTargetTest(TargetTest):
    def get_py( self ):
        py = repr( compile( self.get_idl(), PyTarget() ) )
        py = StringIO( py ).readlines()
        py = [py_line.strip() for py_line in py]
        return py


class PyTargetCompileTest(PyTargetTest):
    def runTest( self ):
        py = repr( compile( self.get_idl(), PyTarget() ) )
        import __builtin__
        __builtin__.compile( py, "<string>", "exec" )


class PyTargetConstantTest(PyTargetTest):
    def runTest( self ):
        assert "%(TEST_CONSTANT_NAME)s = %(TEST_CONSTANT_VALUE)s"\
                % globals() in self.get_py()


class PyTargetExceptionTest(PyTargetTest):
    def runTest( self ):
        assert "class %(TEST_EXCEPTION)s(Exception): pass"\
                   % globals() in self.get_py()


class PyTargetInterfaceTest(PyTargetTest):
    def runTest( self ):
        assert "class %(TEST_INTERFACE)s(object):" % globals() in self.get_py()


class PyTargetOperationTest(PyTargetTest):
    def runTest( self ):
        assert "def %(TEST_OPERATION)s( self ): raise NotImplementedError"\
                   % globals() in self.get_py()


class PyTargetSequenceTest(PyTargetTest):
    def runTest( self ):
        assert "class %(TEST_SEQUENCE)s(list):" % globals() in self.get_py()


class PyTargetStructTest(PyTargetTest):
    def runTest( self ):
        assert "class %(TEST_STRUCT)s(object): pass"\
                   % globals() in self.get_py()


suite = TestSuite()
suite.addTest( PyTargetCompileTest() )
suite.addTest( PyTargetConstantTest() )
suite.addTest( PyTargetExceptionTest() )
suite.addTest( PyTargetInterfaceTest() )
suite.addTest( PyTargetOperationTest() )
suite.addTest( PyTargetSequenceTest() )
suite.addTest( PyTargetStructTest() )
