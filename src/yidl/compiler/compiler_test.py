# yidl/compiler/compiler_test.py

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


import sys
from unittest import TestCase, TestSuite

from yidl.compiler import compile
from yidl.target import Target


# Constants
ILL_FORMED_IDL =\
(
    "module { };",
)


WELL_FORMED_IDL =\
(
    # module only
    ( "module", "yidl", "{", "}" ),
    ( "module", "yidl", "{", "}", ";" ),
    ( "module", "yidl", "#", "1", "{", "}", ";" ),

    # constant
    ( "module", "yidl", "{", "const", "uint32_t", "test_constant", "=", "1", ";", "}", ),

    # interface
    # empty interface
    ( "module", "yidl", "{", "interface", "TestInterface", "{", "}", ";", "}", ";",  ),

    # operation
    # null operation
    ( "module", "yidl", "{", "interface", "TestInterface", "{", "void", "TestOperation", "(", ")", ";", "}", ";", "}", ";" ),
    # operation with one parameter
    ( "module", "yidl", "{", "interface", "TestInterface", "{", "void", "TestOperation", "(", "uint32_t", "x", ")", ";", "}", ";", "}", ";" ),
    # operation with return value
    ( "module", "yidl", "{", "interface", "TestInterface", "{", "uint32_t", "TestOperation", "(", ")", ";", "}", ";", "}", ";" ),

    # struct
    # empty struct
    ( "module", "yidl", "{", "struct", "TestStruct", "{", "}", ";", "}", ";" ),
    # forward-declared struct
    ( "module", "yidl", "{", "struct", "TestStruct", ";", "}", ";" ),
    # struct with member
    ( "module", "yidl", "{", "struct", "TestStruct", "{", "uint8_t", "byte", ";", "}", ";", "}", ";" ),
)


class CompilerTest(TestCase):
    def __init__( self, idl, well_formed=True ):
        TestCase.__init__( self )
        self.__idl = idl
        self.__well_formed = well_formed

    def runTest( self ):
        if self.__well_formed:
            compile( self.__idl, Target() )
        else:
            try:
                compile( self.__idl, Target(), print_parsing_errors=False )
                print >>sys.stderr, "Successfully parsed ill-formed IDL"
                print >>sys.stderr, self.__idl
                self.fail()
            except AssertionError:
                raise
            except:
                # traceback.print_exc()
                pass

    def shortDescription( self ):
        return self.__class__.__name__ + "( idl='" + self.__idl + "', well_formed=" + str( self.__well_formed ) + " )"


suite = TestSuite()

for idl in ILL_FORMED_IDL:
    suite.addTest( CompilerTest( idl, well_formed=False ) )

for idl in WELL_FORMED_IDL:
    suite.addTest( CompilerTest( ' '.join( idl ) ) )

for idl in WELL_FORMED_IDL:
    token_i_min = 1
    token_i_max = len( idl ) - 1
    if idl[token_i_max] == ';': token_i_max -= 1
    for token_i in xrange( token_i_min, token_i_max ):
        suite.addTest( CompilerTest( ' '.join( idl[:token_i] ), well_formed=False ) )
