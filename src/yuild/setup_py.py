# yuild/setup_py.py

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

from os.path import dirname, join as path_join, sep as path_sep

from yuild.constant import C_CXX_INCLUDE_FILE_FNMATCH_PATTERNS, \
                           C_CXX_SOURCE_FILE_FNMATCH_PATTERNS, \
                           INDENT_SPACES, \
                           PY_SOURCE_FILE_FNMATCH_PATTERNS, \
                           SYS_PLATFORM_CHECKS
from yuild.platform_dict import PlatformDict
from yuild.project import Project

from yutil import lpad, \
                  pad, \
                  posixpath, \
                  posixpaths, \
                  quote, \
                  quotestrlist, \
                  relpath, \
                  strlist, \
                  treepaths


# Constants
INDENT_SPACES = INDENT_SPACES["py"]


class SetupPy(Project):
    def __init__(
        self,
        author=None,
        author_email=None,
        description=None,
        license=None,
        platforms=None,
        scripts=None,
        url=None,
        version="1.0",
        **kwds
     ):
        Project.__init__(self, **kwds)

        assert len(self.get_source_dir_path()) == 1

        self.__author = author
        self.__author_email = author_email
        if description is None:
            description = self.get_name()
        self.__description = description
        self.__license = license
        self.__platforms = strlist(platforms)
        self.__scripts = PlatformDict(scripts)
        self.__url = url
        self.__version = version

    def __str__(self):
        INDENT_SPACES = globals()["INDENT_SPACES"]
        source_dir_path = self.get_source_dir_path()['*']

        constants = []
        for constant, platform_dict in (
            ("DEFINE_MACROS", self.get_cxxdefines()),
            ("EXTRA_COMPILE_ARGS", self.get_cxxflags()),
            ("EXTRA_LINK_ARGS", self.get_ldflags()),
            ("INCLUDE_DIRS", self.get_cxxpath()),
            ("LIBRARIES", self.get_libs()),
            ("LIBRARY_DIRS", self.get_libpath()),
            ("SCRIPTS", self.__scripts)
        ):
            constant_values = [constant + " = []" % locals()]
            for platform, values in platform_dict.iteritems():
                values = strlist(values)
                if (constant == "SCRIPTS" or constant.endswith("_DIRS")):
                    values = posixpaths(values)
                values = ", ".join(quotestrlist(values))
                if platform == '*':
                    if len(constant_values) == 1:
                        constant_values = [constant + "= [%(values)s]" % locals()]
                    else:
                        constant_values.append("%(constant)s.extend([%(values)s])" % locals())
                else:
                    sys_platform_check = SYS_PLATFORM_CHECKS[platform]
                    constant_values.append("""\
if %(sys_platform_check)s:
%(INDENT_SPACES)s%(constant)s.extend([%(values)s])""" % locals())
            constants.append('\n'.join(constant_values))
        constants = "\n\n".join(constants)


        setup_kwds = {
            "description": quote(self.__description),
            "long_description": quote(self.__description),
            "name": quote(self.get_name()),
            "version": quote(str(self.__version))
        }

        if self.__author is not None:
            setup_kwds["author"] = quote(self.__author)

        if self.__author_email is not None:
            setup_kwds["author_email"] = quote(self.__author_email)

        if self.__license is not None:
            setup_kwds["license"] = quote(self.__license)

        if len(self.__platforms) > 0:
            setup_kwds["platforms"] = '[' + ", ".join(quotestrlist(self.__platforms)) + ']'

        if len(self.__scripts) > 0:
            setup_kwds["scripts"] = "SCRIPTS"

        if self.__url is not None:
            setup_kwds["url"] = quote(self.__url)

        c_cxx_include_files = \
            self.get_source_files().filter(C_CXX_INCLUDE_FILE_FNMATCH_PATTERNS)
        if len(c_cxx_include_files) > 0:
            setup_kwds["headers"] = \
                '[' + \
                ", ".join([
                    quote(posixpath(c_cxx_include_file.get_path()))
                    for c_cxx_include_file in c_cxx_include_files
                ]) + \
                ']'

        c_cxx_source_files = \
            self.get_source_files().filter(C_CXX_SOURCE_FILE_FNMATCH_PATTERNS)
        if len(c_cxx_source_files) > 0:
            c_cxx_source_file_tree = \
                treepaths([
                    relpath(c_cxx_source_file.get_path(), source_dir_path)
                    for c_cxx_source_file in c_cxx_source_files
                ])

            ext_modules = []
            for ext_module_name in c_cxx_source_file_tree.iterkeys():
                ext_source_file_paths = []
                def visit_c_cxx_source_file_tree(node):
                    if isinstance(node, dict):
                        for src_path, node in node.iteritems():
                            visit_c_cxx_source_file_tree(node)
                    else:
                        ext_source_file_paths.append(
                            quote(posixpath(path_join(source_dir_path, node)))
                        )
                visit_c_cxx_source_file_tree(c_cxx_source_file_tree[ext_module_name])
                if len(ext_source_file_paths) > 0:
                    ext_source_file_paths.sort()
                    ext_source_file_paths = \
                        ", ".join(posixpaths(ext_source_file_paths))
                    ext_module = \
                        """\
Extension("%(ext_module_name)s", [%(ext_source_file_paths)s], **Extension_kwds)""" % locals()
                    ext_modules.append(ext_module)
            if len(ext_modules) > 0:
                setup_kwds["ext_modules"] = '[' + ", ".join(ext_modules) + ']'

        py_source_files = \
            self.get_source_files().filter(PY_SOURCE_FILE_FNMATCH_PATTERNS)
        if len(py_source_files) > 0:
            packages = []
            for py_source_file in py_source_files:
                py_source_file_relpath = relpath(py_source_file.get_path(), source_dir_path)
                py_source_dir_relpath = dirname(py_source_file_relpath)
                package = quote('.'.join(py_source_dir_relpath.split(path_sep)))
                if package not in packages:
                    packages.append(package)
            packages.sort()
            setup_kwds["packages"] = '[' + ", ".join(packages) + ']'
            setup_kwds["package_dir"] = '{"": "%s"}' % posixpath(source_dir_path)

        setup_kwds = \
            '\n'.join([
                INDENT_SPACES + key + '=' + setup_kwds[key] + ','
                for key in sorted(setup_kwds.keys())
            ])

        return """\
from distutils.core import setup, Extension
import sys


# Platform-specific constants
%(constants)s


# **kwds for Extensions
Extension_kwds = {
    "define_macros": DEFINE_MACROS,
    "extra_compile_args": EXTRA_COMPILE_ARGS,
    "extra_link_args": EXTRA_LINK_ARGS,
    "include_dirs": INCLUDE_DIRS,
    "library_dirs": LIBRARY_DIRS,
    "libraries": LIBRARIES    
}


setup(
%(setup_kwds)s
)

""" % locals()

