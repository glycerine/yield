# yuild/project_test.py

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


from cStringIO import StringIO
from unittest import TestCase


__all__ = \
[
    "ProjectTest",
    "TEST_CXXDEFINES",
    "TEST_CXXFLAGS",
    "TEST_CXXPATH",
    "TEST_LIBPATH",
    "TEST_LIBS",
    "TEST_LDFLAGS",
    "TEST_NAME",
    "TEST_OUTPUT_DIR_PATH",
    "TEST_OUTPUT_FILE_NAME",
    "TEST_OUTPUT_FILE_PATH",
    "TEST_SOURCE_FILE_PATHS",
]


# Constants
TEST_NAME = "test"
TEST_CXXDEFINES = ["DEBUG"]
TEST_CXXFLAGS = ["-g"]
TEST_CXXPATH = ["include"]
TEST_LIBPATH = ["lib"]
TEST_LIBS = ["other.lib"]
TEST_LDFLAGS = ["--coverage"]
TEST_OUTPUT_DIR_PATH = "."
TEST_OUTPUT_FILE_NAME = "test.lib"
TEST_OUTPUT_FILE_PATH = "test.lib"
TEST_SOURCE_FILE_PATHS = \
[
    "test.cpp"
]


class ProjectTest(TestCase):
    def _get_project(self, project_class):
        return project_class(
                   cxxdefines=TEST_CXXDEFINES,
                   cxxflags=TEST_CXXFLAGS,
                   cxxpath=TEST_CXXPATH,
                   libpath=TEST_LIBPATH,
                   libs=TEST_LIBS,
                   ldflags=TEST_LDFLAGS,
                   name=TEST_NAME,
                   output_file_path=TEST_OUTPUT_FILE_PATH,
                   source_file_paths=TEST_SOURCE_FILE_PATHS,
               )

    def _get_project_lines(self, project_class):
        project = self._get_project(project_class)
        project_lines = StringIO(repr(project)).readlines()
        project_lines = [project_line.strip()
                            for project_line in project_lines]
        return project_lines
