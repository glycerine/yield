# yuild/sconscript_test.py

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


from unittest import TestSuite

from yutil import quotestrlist

from sconscript import SConscript
from project_test import *


class SConscriptTest(ProjectTest):
    def get_sconscript( self ):
        return self._get_project( SConscript )

    def get_sconscript_lines( self ):
        return self._get_project_lines( SConscript )


class SConscriptCompileTest(SConscriptTest):
    def runTest( self ):
        compile( repr( self.get_sconscript() ), "<string>", "exec" )


class SConscriptEnvironmentTest(SConscriptTest):
    def __init__( self, key, values ):
        SConscriptTest.__init__( self )
        self.__key = key
        self.__values = values

    def runTest( self ):
        key = self.__key
        values = ", ".join( quotestrlist( self.__values ) )
        assert 'build_env["%(key)s"].extend( [%(values)s] )'\
                   % locals() in self.get_sconscript_lines()

    def shortDescription( self ):
        return self.__class__.__name__ + '("' + self.__key + '")'


class SConscriptOutputFilePathTest(SConscriptTest):
    def runTest( self ):
        assert '"%(TEST_OUTPUT_FILE_PATH)s",'\
            % globals() in self.get_sconscript_lines()


class SConscriptSourceFilePathsTest(SConscriptTest):
    def runTest( self ):
        sconscript_lines = self.get_sconscript_lines()
        for source_file_path in TEST_SOURCE_FILE_PATHS:
            assert '"%(source_file_path)s",' % locals() in sconscript_lines


class SConscriptTypeTest(SConscriptTest):
    def runTest( self ):
        assert "build_env.Library(" in self.get_sconscript_lines()


suite = TestSuite()
suite.addTest( SConscriptCompileTest() )
suite.addTest( SConscriptEnvironmentTest( "CXXDEFINES", TEST_CXXDEFINES ) )
suite.addTest( SConscriptEnvironmentTest( "CXXFLAGS", TEST_CXXFLAGS ) )
suite.addTest( SConscriptEnvironmentTest( "CXXPATH", TEST_CXXPATH ) )
suite.addTest( SConscriptEnvironmentTest( "LIBPATH", TEST_LIBPATH ) )
suite.addTest( SConscriptEnvironmentTest( "LIBS", TEST_LIBS ) )
suite.addTest( SConscriptEnvironmentTest( "LDFLAGS", TEST_LDFLAGS ) )
suite.addTest( SConscriptOutputFilePathTest() )
suite.addTest( SConscriptSourceFilePathsTest() )
suite.addTest( SConscriptTypeTest() )
