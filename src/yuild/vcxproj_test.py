# yuild/vcxproj_test.py

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
from xml.dom.minidom import parseString as parse_xml

from yutil import ntpaths

from vcxproj import VCXProj
from project_test import *


class VCXProjTest(ProjectTest):
    def get_vcxproj(self):
        return self._get_project(VCXProj)

    def get_vcxproj_lines(self):
        return self._get_project_lines(VCXProj)


class VCXProjAdditionalDependenciesTest(VCXProjTest):
    def runTest(self):
        test_libs = ' '.join(TEST_LIBS)
        assert "<AdditionalDependencies>%(test_libs)s</AdditionalDependencies>"\
               % locals() in self.get_vcxproj_lines()


class VCXProjAdditionalLibraryDirectoriesTest(VCXProjTest):
    def runTest(self):
        test_libpath = ntpaths(TEST_LIBPATH)
        test_libpath = ["$(ProjectDir)" + path for path in test_libpath]
        test_libpath = ';'.join(test_libpath)
        assert "<AdditionalLibraryDirectories>%(test_libpath)s</AdditionalLibraryDirectories>"\
               % locals() in self.get_vcxproj_lines()


class VCXProjAdditionalIncludeDirectoriesTest(VCXProjTest):
    def runTest(self):
        test_cxxpath = ntpaths(TEST_CXXPATH)
        test_cxxpath = ["$(ProjectDir)" + path for path in test_cxxpath]
        test_cxxpath = ';'.join(test_cxxpath)
        assert "<AdditionalIncludeDirectories>%(test_cxxpath)s</AdditionalIncludeDirectories>"\
               % locals() in self.get_vcxproj_lines()


class VCXProjConfigurationsTest(VCXProjTest):
    def runTest(self):
        vcxproj_lines = self.get_vcxproj_lines()
        for Configuration, Platform in \
            (
                ("Debug", "Win32"),
                ("Release", "Win32"),
            ):

            assert '<ProjectConfiguration Include="%(Configuration)s|%(Platform)s">'\
                   % locals() in vcxproj_lines
            assert "<Configuration>%(Configuration)s</Configuration>" % locals()\
                   in vcxproj_lines
            assert "<Platform>%(Platform)s</Platform>" % locals() in vcxproj_lines
            Condition = "'$(Configuration)|$(Platform)'=='%(Configuration)s|%(Platform)s'" % locals()
            assert '<PropertyGroup Condition="%(Condition)s">' % locals() in vcxproj_lines
            assert '<ItemDefinitionGroup Condition="%(Condition)s">' % locals() in vcxproj_lines


class VCXProjIntDirTest(VCXProjTest):
    def runTest(self):
        assert "<IntDir>$(SolutionDir)build\$(Configuration)\$(ProjectName)\</IntDir>"\
               in self.get_vcxproj_lines()


class VCXProjNameTest(VCXProjTest):
    def runTest(self):
        assert "<RootNamespace>test</RootNamespace>" % globals()\
               in self.get_vcxproj_lines()


class VCXProjOutDirTest(VCXProjTest):
    def runTest(self):
        assert "<OutDir>$(ProjectDir)</OutDir>" in self.get_vcxproj_lines()


class VCXProjPreprocessorDefinitionsTest(VCXProjTest):
    def runTest(self):
        test_cxxdefines = ';'.join(TEST_CXXDEFINES)
        assert "<PreprocessorDefinitions>%(test_cxxdefines)s</PreprocessorDefinitions>"\
               % locals() in self.get_vcxproj_lines()


class VCXProjParseTest(VCXProjTest):
    def runTest(self):
        parse_xml(repr(self.get_vcxproj()))


class VCXProjRTTITest(VCXProjTest):
    def runTest(self):
        assert "<RuntimeTypeInfo>false</RuntimeTypeInfo>"\
               in self.get_vcxproj_lines()


class VCXProjSourceFilePathsTest(VCXProjTest):
    def runTest(self):
        vcxproj_lines = self.get_vcxproj_lines()
        for source_file_path in TEST_SOURCE_FILE_PATHS:
            assert '<ClCompile Include="%(source_file_path)s">' % locals()\
                   in vcxproj_lines


class VCXProjTypeTest(VCXProjTest):
    def runTest(self):
        assert "<ConfigurationType>StaticLibrary</ConfigurationType>"\
               in self.get_vcxproj_lines()


suite = TestSuite()
suite.addTest(VCXProjAdditionalDependenciesTest())
suite.addTest(VCXProjAdditionalIncludeDirectoriesTest())
suite.addTest(VCXProjAdditionalLibraryDirectoriesTest())
suite.addTest(VCXProjConfigurationsTest())
suite.addTest(VCXProjIntDirTest())
suite.addTest(VCXProjNameTest())
suite.addTest(VCXProjPreprocessorDefinitionsTest())
suite.addTest(VCXProjOutDirTest())
suite.addTest(VCXProjRTTITest())
suite.addTest(VCXProjParseTest())
suite.addTest(VCXProjSourceFilePathsTest())
suite.addTest(VCXProjTypeTest())
