# yuild/makefile_test.py

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

from unittest import TestSuite

from makefile import Makefile
from project_test import *


class MakefileTest(ProjectTest):
    def get_Makefile(self):
        return self._get_project(Makefile)

    def get_Makefile_lines(self):
        return self._get_project_lines(Makefile)


class MakefileCXXFLAGSTest(MakefileTest):
    def __init__(self, CXXFLAGS):
        MakefileTest.__init__(self)
        self.__CXXFLAGS = ' '.join(CXXFLAGS)

    def runTest(self):
        CXXFLAGS = self.__CXXFLAGS
        assert 'CXXFLAGS += %(CXXFLAGS)s' % locals()\
            % globals() in self.get_Makefile_lines()


class MakefileLDFLAGSTest(MakefileTest):
    def __init__(self, LDFLAGS):
        MakefileTest.__init__(self)
        self.__LDFLAGS = ' '.join(LDFLAGS)

    def runTest(self):
        LDFLAGS = self.__LDFLAGS
        assert 'LDFLAGS += %(LDFLAGS)s' % locals()\
            % globals() in self.get_Makefile_lines()


class MakefileOutputFilePathTest(MakefileTest):
    def runTest(self):
        assert '%(TEST_OUTPUT_FILE_PATH)s: $(OBJECT_FILE_PATHS)'\
            % globals() in self.get_Makefile_lines()


class MakefileSourceFilePathsTest(MakefileTest):
    def runTest(self):
        Makefile_lines = self.get_Makefile_lines()
        SOURCE_FILE_PATHS = ' '.join(TEST_SOURCE_FILE_PATHS)
        assert 'SOURCE_FILE_PATHS = %(SOURCE_FILE_PATHS)s'\
            % locals() in Makefile_lines


suite = TestSuite()
suite.addTest(MakefileCXXFLAGSTest(["-D" + cxxdefine
                                      for cxxdefine in TEST_CXXDEFINES]))
suite.addTest(MakefileCXXFLAGSTest(TEST_CXXFLAGS))
suite.addTest(MakefileCXXFLAGSTest(["-I" + cxxpath
                                      for cxxpath in TEST_CXXPATH]))
suite.addTest(MakefileLDFLAGSTest(TEST_LDFLAGS))
suite.addTest(MakefileLDFLAGSTest(["-L" + libpath
                                      for libpath in TEST_LIBPATH]))
suite.addTest(MakefileOutputFilePathTest())
suite.addTest(MakefileSourceFilePathsTest())


if __name__ == "__main__":
    from unittest import TextTestRunner
    TextTestRunner().run(suite)
