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

from os.path import join as path_join

from yuild.constant import C_CXX_SOURCE_FILE_FNMATCH_PATTERNS, \
                           INDENT_SPACES, \
                           PY_SOURCE_FILE_FNMATCH_PATTERNS, \
                           SYS_PLATFORM_CHECKS
from yuild.project import Project

from yutil import lpad, pad, posixpath, posixpaths, quote, quotestrlist, relpath, treepaths


# Constants
INDENT_SPACES = INDENT_SPACES["py"]


class SetupPy(Project):
    def __init__(
        self,
        author=None,
        author_email=None,
        description=None,
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
        self.__url = url
        self.__version = version

    def __str__(self):
        INDENT_SPACES = globals()["INDENT_SPACES"]
        name = self.get_name()
        source_dir_path = self.get_source_dir_path()['*']
        version = self.__version

        kwds = {}

        if self.__author is not None:
            kwds["author"] = quote(self.__author)

        if self.__author_email is not None:
            kwds["author_email"] = quote(self.__author_email)

        if self.__url is not None:
            kwds["url"] = quote(self.__url)

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
                def visit_source_file_tree(node):
                    if isinstance(node, dict):
                        for src_path, node in node.iteritems():
                            visit_source_file_tree(node)
                    else:
                        ext_source_file_paths.append(
                            quote(posixpath(path_join(source_dir_path, node)))
                        )
                visit_source_file_tree(c_cxx_source_file_tree[ext_module_name])
                if len(ext_source_file_paths) > 0:
                    ext_source_file_paths = \
                        ", ".join(posixpaths(ext_source_file_paths))
                    ext_module = \
                        """\
Extension("%(ext_module_name)s", [%(ext_source_file_paths)s], define_macros=define_macros, include_dirs=include_dirs, libraries=libraries)""" % locals()
                    ext_modules.append(ext_module)
            if len(ext_modules) > 0:
                Extension_kwds = []
                for Extension_kwd, platform_dict in (
                    ("define_macros", self.get_cxxdefines()),
                    ("extra_compile_args", self.get_cxxflags()),
                    ("extra_link_args", self.get_ldflags()),
                    ("include_dirs", self.get_cxxpath()),
                    ("libraries", self.get_libs()),
                    ("library_dirs", self.get_libpath()),
                ):
                    Extension_kwd_values = []
                    for platform, str_values in platform_dict.iteritems():
                        sys_platform_check = SYS_PLATFORM_CHECKS[platform]
                        values = ", ".join(quotestrlist(str_values))
                        Extension_kwd_values.append("""\
if %(sys_platform_check)s:
    %(Extension_kwd)s.extend([%(values)s])""" % locals())
                    Extension_kwd_values = lpad('\n', '\n'.join(Extension_kwd_values))
                    Extension_kwds.append("""\
%(Extension_kwd)s = []%(Extension_kwd_values)s""" % locals())
                Extension_kwds = pad("\n\n\n", "\n\n".join(Extension_kwds), '\n')
                kwds["ext_modules"] = '[' + ", ".join(ext_modules) + ']'
        else:
            Extension_kwds = ""

        py_source_files = \
            self.get_source_files().filter(PY_SOURCE_FILE_FNMATCH_PATTERNS)
        if len(py_source_files) > 0:
            py_source_file_tree = \
                treepaths([
                    relpath(py_source_file.get_path(), source_dir_path)
                    for py_source_file in py_source_files
                ])
            kwds["package_dir"] = '{"": "%s"}' % posixpath(source_dir_path)
            kwds["packages"] = \
                '[' + \
                ", ".join([quote(package)
                           for package in py_source_file_tree.keys()]) + \
                ']'

        kwds = \
            '\n'.join([
                INDENT_SPACES + key + '=' + kwds[key] + ','
                for key in sorted(kwds.keys())
            ])

        return """\
from distutils.core import setup, Extension
import sys%(Extension_kwds)s

setup(
%(INDENT_SPACES)sname="%(name)s",
%(INDENT_SPACES)sversion="%(version)s",
%(kwds)s
)

""" % locals()

