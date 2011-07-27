# yuild/vcproj.py

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

from glob import glob
from os import getcwd, listdir, sep as os_sep
from os.path import dirname, join, split, splitext
from pprint import pprint
from uuid import uuid4

from yutil import fnmatch, indent, ntpath, ntpaths, relpath, treepaths

from yuild.constant import C_CXX_INCLUDE_FILE_EXTENSIONS, \
                           C_CXX_SOURCE_FILE_FNMATCH_PATTERNS, \
                           INCLUDE_FILE_EXTENSIONS
from yuild.project import Project


__all__ = ["VCProj"]


# Templates
CONFIGURATION = """\
<Configuration
    Name="%(ConfigurationName)s"
    OutputDirectory="%(OutputDirectory)s"
    IntermediateDirectory="%(IntermediateDirectory)s"
    ConfigurationType="%(ConfigurationType)u"
    InheritedPropertySheets="$(VCInstallDir)VCProjectDefaults\UpgradeFromVC71.vsprops"
    CharacterSet="2"
    >
%(tools)s
</Configuration>
"""

EXCLUDED_FILE = """\
<File RelativePath="%(RelativePath)s">
    <FileConfiguration Name="Debug|Win32" ExcludedFromBuild="true" />
    <FileConfiguration Name="Release|Win32" ExcludedFromBuild="true" />
</File>
"""

INCLUDE_FILE = """\
<File RelativePath="%(RelativePath)s" />
"""

SOURCE_FILE = """\
<File RelativePath="%(RelativePath)s">
    <FileConfiguration Name="Debug|Win32">
        <Tool Name="VCCLCompilerTool" ObjectFile="%(ObjectFile)s" />
    </FileConfiguration>
    <FileConfiguration Name="Release|Win32">
        <Tool Name="VCCLCompilerTool" ObjectFile="%(ObjectFile)s" />
    </FileConfiguration>
</File>
"""

VC_CL_COMPILER_TOOL = """\
<Tool
    Name="VCCLCompilerTool"
    Optimization="%(Optimization)u"
    AdditionalIncludeDirectories="%(AdditionalIncludeDirectories)s"
    PreprocessorDefinitions="%(PreprocessorDefinitions)s"
    %(BasicRuntimeChecks)s
    RuntimeLibrary="%(RuntimeLibrary)u"
    RuntimeTypeInfo="false"
    UsePrecompiledHeader="0"
    WarningLevel="4"
    DebugInformationFormat="%(DebugInformationFormat)u"
    ProgramDataBaseFileName="$(TargetDir)$(TargetName).pdb"
    DisableSpecificWarnings="4100;4127;4355;4512"
/>
"""

VC_LIBRARIAN_TOOL = """\
<Tool
    Name="VCLibrarianTool"
    UseLibraryDependencyInputs="true"
    AdditionalDependencies="%(AdditionalDependencies)s"
    AdditionalLibraryDirectories="%(AdditionalLibraryDirectories)s"
    AdditionalOptions="/ignore:4221"
    OutputFile="%(OutputFile)s"
    IgnoreAllDefaultLibraries="false"
    GenerateDebugInformation="%(GenerateDebugInformation)s"
    RandomizedBaseAddress="1"
    DataExecutionPrevention="0"
/>
"""

VC_LINKER_TOOL = """\
<Tool
    Name="VCLinkerTool"
    UseLibraryDependencyInputs="true"
    AdditionalDependencies="%(AdditionalDependencies)s"
    AdditionalLibraryDirectories="%(AdditionalLibraryDirectories)s"
    AdditionalOptions="/ignore:4221"
    OutputFile="%(OutputFile)s"
    IgnoreAllDefaultLibraries="false"
    GenerateDebugInformation="%(GenerateDebugInformation)s"
    RandomizedBaseAddress="1"
    DataExecutionPrevention="0"
    SubSystem="%(SubSystem)s"
/>
"""


class VCProj(Project):
    CONFIGURATION_TYPE = {
        Project.TYPE_GUI: 1,
        Project.TYPE_PROGRAM: 1,
        Project.TYPE_PYTHON_EXTENSION: 2,
        Project.TYPE_SHARED_LIBRARY: 2,
        Project.TYPE_STATIC_LIBRARY: 4
    }
    assert len(CONFIGURATION_TYPE) == len(Project.TYPES)

    SUBSYSTEM = {
        Project.TYPE_GUI: 2,
        Project.TYPE_PROGRAM: 0,
        Project.TYPE_PYTHON_EXTENSION: 0,
        Project.TYPE_SHARED_LIBRARY: 0,
        Project.TYPE_STATIC_LIBRARY: 0
    }
    assert len(SUBSYSTEM) == len(Project.TYPES)

    OUTPUT_FILE_EXT = {
        Project.TYPE_GUI: "exe",
        Project.TYPE_PROGRAM: ".exe",
        Project.TYPE_PYTHON_EXTENSION: ".pyd",
        Project.TYPE_SHARED_LIBRARY: ".dll",
        Project.TYPE_STATIC_LIBRARY: ".lib"
    }
    assert len(OUTPUT_FILE_EXT) == len(Project.TYPES)

    def get_AdditionalDependencies(self, sep=' '):
        AdditionalDependencies = []
        for lib in self.get_libs().get("win32", combine_platforms=True, default=[]):
            if len(splitext(lib)[1]) == 0: lib += ".lib"
            AdditionalDependencies.append(lib)
        return sep.join(AdditionalDependencies)

    def get_AdditionalIncludeDirectories(self):
        cxxpath = self.get_cxxpath().get("win32", combine_platforms=True)
        cxxpath = ntpaths(cxxpath)
        return ';'.join(["$(ProjectDir)" + path for path in cxxpath])

    def get_AdditionalLibraryDirectories(self):
        libpath = self.get_libpath().get("win32", combine_platforms=True)
        libpath = ntpaths(libpath)
        return ';'.join(["$(ProjectDir)" + path for path in libpath])

    def get_IntermediateDirectory(self):
        build_dir_path = \
            self.get_build_dir_path().get(
                "win32",
                combine_platforms=True,
                default=None
            )

        if build_dir_path is None:
            IntermediateDirectory = "$(SolutionDir)build\\$(Configuration)\\$(ProjectName)\\"
        else:
            IntermediateDirectory = "$(ProjectDir)" + ntpath(build_dir_path)
            if not IntermediateDirectory.endswith('\\'):
                IntermediateDirectory += '\\'
        return IntermediateDirectory

    def get_Name(self):
        return self.get_name()

    def get_OutputDirectory(self):
        OutputDirectory = split(self.get_output_file_path()["win32"])[0]
        if len(OutputDirectory) == 0:
            OutputDirectory = "$(ProjectDir)"
        else:
            OutputDirectory = "$(ProjectDir)" + ntpath(OutputDirectory)
            if OutputDirectory[-1] != '\\':
                OutputDirectory += '\\'
        return OutputDirectory

    def get_OutputFile(self):
        OutputFile = split(self.get_output_file_path()["win32"])[1]
        if len(splitext(OutputFile)[1]) == 0:
            OutputFile += self.OUTPUT_FILE_EXT[self.get_type()]
        return OutputFile

    def get_PreprocessorDefinitions(self):
        cxxdefines = self.get_cxxdefines().get("win32", combine_platforms=True, default=[])
        return ';'.join(cxxdefines)

    def get_RootNamespace(self):
        return self.get_name()

    def __str__(self):
        AdditionalDependencies = self.get_AdditionalDependencies()
        AdditionalIncludeDirectories = self.get_AdditionalIncludeDirectories()
        AdditionalLibraryDirectories = ntpaths(self.get_libpath()["win32"])
        AdditionalLibraryDirectories = ';'.join(AdditionalLibraryDirectories)
        ConfigurationType = self.CONFIGURATION_TYPE[self.get_type()]
        IntermediateDirectory = self.get_IntermediateDirectory()
        Name = self.get_Name()
        OutputDirectory = self.get_OutputDirectory()
        OutputFile = self.get_OutputFile()
        PreprocessorDefinitions = self.get_PreprocessorDefinitions()
        RootNamespace = self.get_RootNamespace()
        SubSystem = self.SUBSYSTEM[self.get_type()]

        try:
            vcproj_file = \
                open(
                    join(
                        self.get_project_dir_path(),
                        self.get_name() + ".vcproj"
                    )
                )
            vcproj_file_contents = vcproj_file.read()
            vcproj_file.close()
            guid_i = vcproj_file_contents.index("ProjectGUID=\"{")
            ProjectGUID = vcproj_file_contents[guid_i + 13:vcproj_file_contents.index("}", guid_i + 1) + 1]
        except IOError:
            ProjectGUID = '{' + str(uuid4()) + '}'

        header_files, source_files = \
            self.__get_files(self.get_source_file_tree(), [])

        header_files = "".join(header_files)
        if len(header_files) > 0:
            header_files = """\
            <Filter
                Name="Header Files"
                Filter="h;hpp;hxx;hm;inl;inc;xsd"
                UniqueIdentifier="{93995380-89BD-4b04-88EB-625FBE52EBFB}"
                >
                %(header_files)s
            </Filter>
    """ % locals()

        source_files = "".join(source_files)
        if len(source_files) > 0:
            source_files = """\
            <Filter
                Name="Source Files"
                Filter="cpp;c;cxx;def;odl;idl;hpj;bat;asm;asmx"
                UniqueIdentifier="{4FC737F1-C7A5-4376-A066-2A32D752A2FF}"
                >
                %(source_files)s
            </Filter>
    """ % locals()

        configurations = []
        for ConfigurationName in ("Debug|Win32", "Release|Win32"):
            if ConfigurationName == "Debug|Win32":
                BasicRuntimeChecks = 'BasicRuntimeChecks="3"'
                DebugInformationFormat = 4
                GenerateDebugInformation = "true"
                Optimization = 0
                RuntimeLibrary = 3
            else:
                BasicRuntimeChecks = ""
                DebugInformationFormat = 0
                GenerationDebugInformation = "false"
                Optimization = 2
                RuntimeLibrary = 2

            tools = [VC_CL_COMPILER_TOOL % locals()]
            if self.get_type() == self.TYPE_STATIC_LIBRARY:
                tools.append(VC_LIBRARIAN_TOOL % locals())
            else:
                tools.append(VC_LINKER_TOOL % locals())
            tools = indent("\t\t", '\n'.join(tools))

            configurations.append(CONFIGURATION % locals())
        configurations = indent("\t\t", "\r\n".join(configurations))

        return """\
<?xml version="1.0" encoding="Windows-1252"?>
<VisualStudioProject
    ProjectType="Visual C++"
    Version="9.00"
    Name="%(Name)s"
    ProjectGUID="%(ProjectGUID)s"
    RootNamespace="%(RootNamespace)s"
    Keyword="Win32Proj"
    TargetFrameworkVersion="131072"
    >
    <Platforms>
        <Platform
            Name="Win32"
        />
    </Platforms>
    <ToolFiles>
    </ToolFiles>
    <Configurations>
%(configurations)s
    </Configurations>
    <References>
    </References>
    <Files>
        %(header_files)s
        %(source_files)s
    </Files>
    <Globals>
    </Globals>
</VisualStudioProject>
""" % locals()

    def __get_files(
        self,
        platform_source_file_tree,
        seen_source_files
    ):
        header_files = []
        source_files = []

        platforms = platform_source_file_tree.keys()
        platforms.sort() # So that * comes first

        for platform in platforms:
            source_file_tree = platform_source_file_tree[platform]
            # First pass: get all files
            source_paths = source_file_tree.keys()
            source_paths.sort()

            for source_path in source_paths:
                if not isinstance(source_file_tree[source_path], dict): # source_path is a file
                    source_file_path = source_path
                    if source_file_path in seen_source_files: continue
                    seen_source_files.append(source_file_path)
                    RelativePath = ntpath(source_file_path)

                    source_file_ext = splitext(source_file_path)[1]
                    if source_file_ext in INCLUDE_FILE_EXTENSIONS:
                        # Check if we're in a directory with only headers
                        # If so, add this file to header_files.
                        # If not, and there's source in the directory, add this
                        # file to source_files.
                        is_header_file = True
                        for other_source_path in source_paths:
                            if not isinstance(source_file_tree[other_source_path], dict):
                                if fnmatch(other_source_path, C_CXX_SOURCE_FILE_FNMATCH_PATTERNS):
                                    is_header_file = False
                                    break

                        if is_header_file:
                            if source_file_ext in C_CXX_INCLUDE_FILE_EXTENSIONS:
                                header_files.append(INCLUDE_FILE % locals())
                            else:
                                header_files.append(EXCLUDED_FILE % locals())

                            continue
                        elif source_file_ext in C_CXX_INCLUDE_FILE_EXTENSIONS:
                            source_file = INCLUDE_FILE % locals()
                        else:
                            source_file = EXCLUDED_FILE % locals()
                    elif source_file_ext in C_CXX_SOURCE_FILE_EXTENSIONS and\
                         platform in ('*', "win32") and\
                         not source_file_path in \
                             self.get_exclude_files().get(platform, []):
                            ObjectFile = "$(IntDir)"
                            ObjectFile += \
                                ntpath(relpath(source_file_path, self.get_source_dir_path()[platform]))
                            ObjectFile = ObjectFile[:-len(source_file_ext)] + ".obj"
                            source_file = SOURCE_FILE % locals()
                    else:
                        source_file = EXCLUDED_FILE % locals()

                    source_files.append(source_file)

            # Second pass: get all subdirectories
            # If there are no files in this source_file_tree and only one subdirectory,
            # then fold the subdirectory into this source_file_tree
            for source_path in source_paths:
                if isinstance(source_file_tree[source_path], dict): # source_path is a directory
                    child_source_file_tree = {}
                    for platform, other_source_file_tree in platform_source_file_tree.iteritems():
                        if other_source_file_tree.has_key(source_path):
                            child_source_file_tree[platform] = other_source_file_tree[source_path]

                    child_header_files, child_source_files = \
                        self.__get_files(child_source_file_tree, seen_source_files)

                    if len(child_header_files) > 0 or len(child_source_files) > 0:
                        filter_name = split(relpath(source_path, self.get_source_dir_path[platform]()))[-1]
                        if filter_name != "..":
                            filter_start = """<Filter Name="%(filter_name)s">\r\n""" % locals()
                            filter_end = """</Filter>\r\n"""

                            if len(child_header_files) > 0:
                                if len(header_files) > 0:
                                    header_files.append(filter_start)
                                    header_files.extend(child_header_files)
                                    header_files.append(filter_end)
                                else:
                                    header_files.extend(child_header_files)
                            if len(child_source_files) > 0:
                                if len(source_files) > 0:
                                    source_files.append(filter_start)
                                    source_files.extend(child_source_files)
                                    source_files.append(filter_end)
                                else:
                                    source_files.extend(child_source_files)
                        else:
                            header_files.extend(child_header_files)
                            source_files.extend(child_source_files)

        return header_files, source_files
