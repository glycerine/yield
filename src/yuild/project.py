# yuild/project.py

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

from os import getcwd
from os.path import dirname, exists, join

from yutil import abspath, \
                  abspaths, \
                  deduplist, \
                  relpath, \
                  relpaths, \
                  static_cast, \
                  strlist, \
                  treepaths

from yuild.platform_dict import PlatformDict
from yuild.source_files import SourceFiles


__all__ = ["Project"]


class Project(object):
    def __init__(
        self,
        name,
        output_file_path,
        source_file_paths,
        build_dir_path=None,
        cxxdefines=None,
        cxxflags=None,
        cxxpath=None,
        exclude_file_paths=None,
        include_file_paths=None,
        libpath=None,
        libs=None,
        ldflags=None,
        project_dir_path=None,
        project_references=None,
        type="lib",
        *args,
        **kwds
    ):
        self.__cxxdefines = PlatformDict(cxxdefines)
        self.__cxxflags = PlatformDict(cxxflags)
        self.__libs = PlatformDict(libs)
        self.__ldflags = PlatformDict(ldflags)
        self.__name = static_cast(name, str)
        self.__type = static_cast(type, str)

        if project_dir_path is not None:
            project_dir_path = static_cast(project_dir_path, str)
            # assert exists( project_dir_path )
            self.__project_dir_path = project_dir_path
        else:
            self.__project_dir_path = getcwd()

        if build_dir_path is None:
            self._build_dir_path = PlatformDict(None)
        else:
            platform_build_dir_path = PlatformDict(build_dir_path)
            for platform in platform_build_dir_path.iterkeys():
                build_dir_path = platform_build_dir_path[platform]
                # assert exists( build_dir_path ), build_dir_path
                build_dir_path = relpath(build_dir_path, self.__project_dir_path)
                platform_build_dir_path[platform] = build_dir_path
            self._build_dir_path = platform_build_dir_path

        for paths_var in ("cxxpath", "libpath"):
            paths = locals()[paths_var]
            if paths is None:
                setattr(self, '_' + paths_var, PlatformDict(None))
            else:
                platform_paths = dict(PlatformDict(paths))
                for platform in platform_paths.iterkeys():
                    paths = platform_paths[platform]
                    paths = strlist(paths)
                    rel_paths = []
                    for path in paths:
                        if exists(path):
                            rel_paths.append(relpath(path, self.__project_dir_path))
                        else:
                            rel_paths.append(path)
                    platform_paths[platform] = rel_paths
                setattr(self, '_' + paths_var, PlatformDict(platform_paths))

        platform_output_file_path = PlatformDict(output_file_path)
        for platform in platform_output_file_path.iterkeys():
            output_file_path = platform_output_file_path[platform]
            # assert exists( dirname( output_file_path ) )
            output_file_path = relpath(output_file_path, self.__project_dir_path)
            platform_output_file_path[platform] = output_file_path
        self._output_file_path = platform_output_file_path

        platform_project_references = PlatformDict(project_references)
        for platform in platform_project_references.iterkeys():
            project_references = platform_project_references[platform]
            project_references = strlist(project_references)
#            for project_reference in project_references:
#                assert exists( dirname( project_reference ) ), project_reference
            project_references = \
                relpaths(project_references, self.__project_dir_path)
            platform_project_references[platform] = project_references
        self._project_references = platform_project_references

        for paths_var in \
            (
                "exclude_file_paths",
                "include_file_paths",
                "source_file_paths",
            ):

            paths = locals()[paths_var]
            paths = strlist(paths)
            paths = deduplist(paths)
            # for path in paths: assert exists( path ), path
            rel_paths = relpaths(paths, self.__project_dir_path)
            source_files = SourceFiles(rel_paths)
            setattr(self, '_' + paths_var[:-6] + 's', source_files)
            source_file_tree = treepaths(source_files)
            setattr(self, '_' + paths_var[:-6] + "_tree", source_file_tree)

#        c_cxx_source_files =\
#             self.get_source_files().filter( C_CXX_SOURCE_FILE_FNMATCH_PATTERNS )
#        c_cxx_source_file_tree = treepaths( c_cxx_source_files )
        source_file_tree = self.get_source_file_tree()
        if len(source_file_tree) == 0:
            self.__root_source_dir_path = self.get_project_dir_path()
        elif len(source_file_tree) == 1:
            if isinstance(source_file_tree.values()[0], dict):
                self.__root_source_dir_path = source_file_tree.keys()[0]
            else:
                self.__root_source_dir_path = dirname(source_file_tree.keys()[0])
        else:
            raise NotImplementedError, source_file_tree.keys()

    def get_build_dir_path(self): return self._build_dir_path
    def get_cxxdefines(self): return self.__cxxdefines
    def get_cxxflags(self): return self.__cxxflags
    def get_cxxpath(self): return self._cxxpath
    def get_exclude_files(self): return self._exclude_files
    def get_include_files(self): return self._include_files
    def get_include_file_tree(self): return self._include_file_tree
    def get_libpath(self): return self._libpath
    def get_libs(self): return self.__libs
    def get_ldflags(self): return self.__ldflags
    def get_name(self): return self.__name
    def get_output_file_path(self): return self._output_file_path
    def get_project_dir_path(self): return self.__project_dir_path
    def get_project_references(self): return self._project_references
    def get_root_source_dir_path(self): return self.__root_source_dir_path
    def get_source_files(self): return self._source_files
    def get_source_file_tree(self): return self._source_file_tree
    def get_type(self): return self.__type
