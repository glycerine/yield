#!/usr/bin/env python

# check_src.py

# Copyright (c) 2011 Minor Gordon
# All rights reserved

# This source file is part of the Yield project.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
# * Neither the name of the Yield project nor the
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


assert __name__ == "__main__"


from os import sep as os_sep
from os.path import exists, join, split, splitext
from optparse import OptionParser
from zlib import crc32

from config import *

from yuild.constant import CXX_SOURCE_FILE_FNMATCH_PATTERNS
from yuild import SourceFiles
from yutil import fnmatch, rglob


# Constants
YIELD_INCLUDE_YIELD_DIR_PATH = join(YIELD_INCLUDE_DIR_PATH, "yield")
YIELD_SRC_YIELD_DIR_PATH = join(YIELD_SRC_DIR_PATH, "yield")


# Parse options
option_parser = OptionParser()
option_parser.add_option("-p", "--project")
option_parser.add_option("--namespace", action="store_true")
option_parser.add_option("--type-id", action="store_true")
option_parser.add_option("--width", action="store_true")
option_parser.add_option("--yunit", action="store_true")
options, ignore = option_parser.parse_args()


if options.project:
    assert options.project in PROJECT_NAMES, options.project
    project_names = [options.project]
else:
    project_names = PROJECT_NAMES

include_file_paths = []
source_file_paths = []
for project_name in project_names:
    for include_file_paths_ in INCLUDE_FILE_PATHS[project_name]:
        include_file_paths.extend(list(rglob(include_file_paths_)))
    for source_file_paths_ in SOURCE_FILE_PATHS[project_name]:
        source_file_paths.extend(list(rglob(source_file_paths_)))


if options.namespace or options.type_id:
    for include_file_path in include_file_paths:
        namespaces = include_file_path[len(YIELD_INCLUDE_DIR_PATH) + 1:].split(os_sep)[:-1]
        assert len(namespaces) > 0

        include_file_lines = [include_file_line.strip()
                              for include_file_line in
                              open(include_file_path).readlines()]

        for include_file_line in include_file_lines:
            if options.namespace:
                if include_file_line.startswith("namespace ") and \
                   not '{' in include_file_line:
                    namespace = include_file_line[10:]
                    if namespace == namespaces[0]:
                        del namespaces[0]
                    else:
                        print include_file_path[len(YIELD_INCLUDE_YIELD_DIR_PATH) + 1:] + ":",
                        print "expected namespace", namespaces[0],
                        print "but found", namespace
                        break

            if options.type_id:
                if include_file_line.startswith("class") and \
                   not include_file_line.endswith(';'):
                    globals()["class_"] = include_file_line.split(' ')[1]
                elif "TYPE_ID = " in include_file_line:
                    type_id = include_file_line.split(' ')[-1][:-1]
                    if type_id.endswith("UL"): type_id = type_id[:-2]
                    type_name = "::".join(namespaces + [class_])
                    expected_type_id = str(crc32(type_name) & 0xffffffff)

                    if type_id != expected_type_id:
                        print type_name + ": expected TYPE_ID =",
                        print expected_type_id, "but found", type_id

                    quoted_type_name = '"' + type_name + '"'
                    found_quoted_type_name = False
                    for other_include_file_line in include_file_lines:
                        if quoted_type_name in other_include_file_line:
                            found_quoted_type_name = True
                            break
                    if not found_quoted_type_name:
                        print type_name + ": could not find type name literal"


if options.width:
    for file_path in include_file_paths + source_file_paths:
        if not fnmatch(file_path, THIRD_PARTY_SOURCE_FNMATCH_PATTERNS):
            file_lines = open(file_path).readlines()
            for file_line_i in xrange(len(file_lines)):
                file_line = file_lines[file_line_i]
                if len(file_line) > 80:
                    print file_path + ':' + str(file_line_i + 1) + " is " + \
                          str(len(file_line)) + " columns wide"


if options.yunit:
    source_files = SourceFiles(source_file_paths)
    source_files = source_files.filter(CXX_SOURCE_FILE_FNMATCH_PATTERNS)
    source_files = source_files.exclude(THIRD_PARTY_SOURCE_FNMATCH_PATTERNS)
    for source_file_path in source_files['*']:
        source_dir_path, source_file_name = split(source_file_path)
        source_file_stem, source_file_ext = splitext(source_file_name)
        if not source_file_stem.endswith("_test") and\
           not source_file_stem.endswith("_test_main"):
            test_file_path = \
                join(
                    source_dir_path,
                    source_file_stem + "_test" + source_file_ext
                )
            if not exists(test_file_path):
                print "missing", test_file_path[len(YIELD_SRC_YIELD_DIR_PATH) + 1:]
