#!/usr/bin/env python

# format_src.py

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


from os.path import abspath, dirname, isdir, isfile, join, split
from optparse import OptionParser

from config import *

from yuild.constant import INCLUDE_FILE_FNMATCH_PATTERNS, \
                           SOURCE_FILE_FNMATCH_PATTERNS
from yuild import SourceFiles
from yutil import fnmatch, rglob



# Parse options
option_parser = OptionParser()
option_parser.add_option("-f", "--force", action="store_true")
options, ignore = option_parser.parse_args()


SOURCE_PATHS = \
{
    "yidl": [join(YIELD_DIR_PATH, "src", "yidl")],
    "Yield":
        [
            join(YIELD_DIR_PATH, "include"),
            join(YIELD_DIR_PATH, "sbin"),
            join(YIELD_DIR_PATH, "src", "yield"),
        ],
    "yutil":
        [
            join(YIELD_DIR_PATH, "src", "yutil.py"),
            join(YIELD_DIR_PATH, "src", "yutil_test.py"),
        ],
    "yuild": [join(YIELD_DIR_PATH, "src", "yuild")],
}

for project, source_paths in SOURCE_PATHS.iteritems():
    source_files = SourceFiles()
    for source_path in source_paths:
        if isdir(source_path):
            for fnmatch_pattern in INCLUDE_FILE_FNMATCH_PATTERNS + \
                                   SOURCE_FILE_FNMATCH_PATTERNS:
                for source_file_path in rglob(join(source_path, "**", fnmatch_pattern)):
                    source_files.append(source_file_path)
        else:
            assert isfile(source_path)
            source_files.append(source_path)

    source_files = source_files.exclude(THIRD_PARTY_SOURCE_FNMATCH_PATTERNS)

    source_files.format(
        author="Minor Gordon",
        force=options.force,
        project=project,
    )
