# yuild/source_files_test.py

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

from os import unlink
from random import randint
from unittest import TestCase, TestSuite

from source_files import SourceFiles


class SourceFilesTest(TestCase):
    def _get_source_files(self):
        return SourceFiles("test1.cpp", "test2.cpp")

    def runTest(self):
        source_files = self._get_source_files()
        assert len(source_files) == 1, len(source_files)
        assert source_files.keys()[0] == '*', source_files.keys()
        assert isinstance(source_files.values()[0], list), source_files.values()
        assert len(source_files.values()[0]) == 2, source_files.values()

    def setUp(self):
        self.tearDown()
        for file_paths in self._get_source_files().itervalues():
            for file_path in file_paths:
                test_file = open(file_path, "w+")
                i = randint(0, 11151980)
                test_file.write("""\
#include <somefile%(i)u.hpp>
#include "otherfile%(i)u.hpp>"

extern int errno;
""" % locals())
            test_file.close()

    def tearDown(self):
        for file_paths in self._get_source_files().itervalues():
            for file_path in file_paths:
                try: unlink(file_path)
                except: pass


class SourceFilesReprTest(SourceFilesTest):
    def runTest(self):
        repr_ = repr(self._get_source_files())


suite = TestSuite()
suite.addTest(SourceFilesTest())
suite.addTest(SourceFilesReprTest())
