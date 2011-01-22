# yuild/vcproj_test.py

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
from xml.dom.minidom import parseString as parse_xml

from vcproj import VCProj
from project_test import *


class VCProjTest(ProjectTest):
    def get_vcproj(self):
        return self._get_project(VCProj)

    def get_vcproj_lines(self):
        return self._get_project_lines(VCProj)


class VCProjOutputFilePathTest(VCProjTest):
    def runTest(self):
        vcproj_lines = self.get_vcproj_lines()
        assert 'OutputDirectory="$(ProjectDir)"' in vcproj_lines
        assert 'OutputFile="%(TEST_OUTPUT_FILE_NAME)s"'\
                   % globals() in vcproj_lines


class VCProjParseTest(VCProjTest):
    def runTest(self):
        parse_xml(repr(self.get_vcproj()))


class VCProjSourceFilePathsTest(VCProjTest):
    def runTest(self):
        vcproj_lines = self.get_vcproj_lines()
        for source_file_path in TEST_SOURCE_FILE_PATHS:
            assert '<File RelativePath="%(source_file_path)s">'\
                       % locals() in vcproj_lines


class VCProjTypeTest(VCProjTest):
    def runTest(self):
        vcproj_lines = self.get_vcproj_lines()
        assert 'Name="VCCLCompilerTool"' in vcproj_lines
        assert 'Name="VCLibrarianTool"' in vcproj_lines


suite = TestSuite()
suite.addTest(VCProjOutputFilePathTest())
suite.addTest(VCProjParseTest())
suite.addTest(VCProjSourceFilePathsTest())
suite.addTest(VCProjTypeTest())
