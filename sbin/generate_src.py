#!/usr/bin/env python

# generate_src.py

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


from os import chdir, listdir, unlink
from os.path import abspath, dirname, exists, join, split, splitext
from optparse import OptionParser
from subprocess import call
import sys

from yuild.constant import C_CXX_INCLUDE_FILE_FNMATCH_PATTERNS, \
                           C_CXX_FILE_FNMATCH_PATTERNS, \
                           INDENT_SPACES, \
                           PLATFORM_GUARDS
from yuild import SourceFiles
from yutil import indent, list_subdirectories, rglob, strlistsort, write_file

from config import *



# Parse options
option_parser = OptionParser()
option_parser.add_option("-a", "--all", action="store_true")
option_parser.add_option("-d", "--debug", action="store_true")
option_parser.add_option("-f", "--force", action="store_true")
option_parser.add_option("--idl", action="store_true")
option_parser.add_option("--public", action="store_true")
option_parser.add_option("--test-main-cpp", action="store_true")
option_parser.add_option("--test-py", action="store_true")
option_parser.add_option("--ragel", action="store_true")
options, ignore = option_parser.parse_args()



if options.all or options.idl:
    from yidl.compiler import compile
    from yidl.target.yield_cpp_target import YieldCXXTarget

    # Generate source from IDL definitions
    for dir_path in\
        (
            join(YIELD_DIR_PATH, "include"),
            join(YIELD_DIR_PATH, "src", "yield"),
        ):

        for idl_file_path in rglob(join(dir_path, "**", "*.idl")):
            hpp_file_path = \
                join(
                    dirname(idl_file_path),
                    split(idl_file_path)[1][:-3] + "hpp"
                )

            if options.debug:
                print "compiling", idl_file_path, "into", hpp_file_path

            compile(
                idl_file_path,
                YieldCXXTarget(
                    force=options.force,
                    output_file_path=hpp_file_path
                )
            )

if options.all or options.ragel:
    # Generate source from Ragel state machine definitions
    for rl_file_path in rglob(join(YIELD_DIR_PATH, "src", "yield", "**", "*.rl")):
        cpp_file_path = \
            join(
                dirname(rl_file_path),
                split(rl_file_path)[1][:-2] + "cpp"
            )

        if exists(cpp_file_path):
            args = ["ragel"]
            args.append("-L")
            args.extend(("-o", cpp_file_path))
            args.append(rl_file_path)
            call(args)
            print "wrote", cpp_file_path


if options.all or options.public:
    YIELD_PUBLIC_DIR_PATH = \
        abspath(join(YIELD_DIR_PATH, "..", "yield_public"))
    if exists(YIELD_PUBLIC_DIR_PATH):
        assert exists(join(YIELD_PUBLIC_DIR_PATH, "include", "yield"))
        assert exists(join(YIELD_PUBLIC_DIR_PATH, "src", "yield"))

        for project_name in listdir(join(YIELD_PUBLIC_DIR_PATH, "include", "yield")):
            if project_name.endswith(".hpp"):
                project_name = splitext(project_name)[0]

                hpp = []
                cpp = ['#include "yield/%(project_name)s.hpp"\n\n\n' % locals()]

                for project_reference in PROJECT_BUILD_ORDER[:PROJECT_BUILD_ORDER.index(project_name) + 1]:
                    include_files = SourceFiles(INCLUDE_FILE_PATHS[project_reference])
                    include_files = include_files.filter(C_CXX_INCLUDE_FILE_FNMATCH_PATTERNS)
                    hpp.append(include_files.get_combined_repr())
                    source_files = SourceFiles(SOURCE_FILE_PATHS[project_reference])
                    source_files = source_files.filter(C_CXX_FILE_FNMATCH_PATTERNS)
                    source_files = source_files.exclude("test_*")
                    source_files = source_files.exclude("*_test*")
                    cpp.append(source_files.get_combined_repr())

                write_file(
                    join(
                        YIELD_PUBLIC_DIR_PATH,
                        "include",
                        "yield",
                        project_name + ".hpp"
                    ),
                    hpp,
                    force=options.force
                )

                write_file(
                    join(
                        YIELD_PUBLIC_DIR_PATH,
                        "src",
                        "yield",
                        project_name + ".cpp"
                    ),
                    cpp,
                    force=options.force
                )


if options.all or options.test_main_cpp:
    # Generate *_test_main.cpp's
    for project_name in PROJECT_NAMES:
        if project_name == "common":
            project_src_dir_path = join(YIELD_DIR_PATH, "src", "yield")
            test_cpp_file_paths = list(rglob(join(project_src_dir_path, "*_test.cpp")))
        else:
            project_src_dir_path = join(YIELD_DIR_PATH, "src", "yield", project_name)
            test_cpp_file_paths = list(rglob(join(project_src_dir_path, "**", "*_test.cpp")))

        test_suite_decls = []
        test_suite_runs = []

        for test_cpp_file in SourceFiles(test_cpp_file_paths):
            test_suite_names = []
            for test_cpp_file_line in open(test_cpp_file.get_path()).readlines():
                test_cpp_file_line = test_cpp_file_line.strip()
                if test_cpp_file_line.startswith("TEST_SUITE_EX"):
                    test_suite_name = test_cpp_file_line.split(' ')[1][:-1]
                elif test_cpp_file_line.startswith("TEST_SUITE"):
                    test_suite_name = test_cpp_file_line.split(' ')[1]
                else:
                    continue

                if not test_suite_name.endswith("TestSuite"):
                    test_suite_name += "TestSuite"
                test_suite_names.append(test_suite_name)
                break

            if len(test_suite_names) > 0:
                test_suite_names = strlistsort(test_suite_names)

                for test_suite_name in test_suite_names:
                    assert test_suite_name.endswith("TestSuite")
                    test_suite_stem = test_suite_name[:-9]

                    test_suite_decl = \
                        "extern yunit::TestSuite& %(test_suite_name)s();" % locals()

                    test_suite_run = """\
// %(test_suite_stem)s
std::cout << "%(test_suite_stem)s:" << std::endl;
failed_test_case_count += %(test_suite_name)s().run();
std::cout << std::endl;""" % locals()

                    if test_cpp_file.get_platform() != "*":
                        platform_guard = PLATFORM_GUARDS[test_cpp_file.get_platform()]

                        test_suite_decl = indent(INDENT_SPACES["cpp"], test_suite_decl)
                        test_suite_decl = "%(platform_guard)s\n%(test_suite_decl)s\n#endif" % locals()

                        test_suite_run = indent(INDENT_SPACES["cpp"], test_suite_run)
                        test_suite_run = "%(platform_guard)s\n%(test_suite_run)s\n#endif" % locals()

                    test_suite_decls.append(test_suite_decl)
                    test_suite_runs.append(test_suite_run)

        test_suite_decls = '\n'.join(test_suite_decls)
        test_suite_runs = \
            indent(INDENT_SPACES["cpp"], '\n\n'.join(test_suite_runs))

        write_file(
            join(project_src_dir_path, "yield_" + project_name + "_test_main.cpp"),
        """\
#include "yunit.hpp"

#include <iostream>


%(test_suite_decls)s


int main( int, char** )
{
  int failed_test_case_count = 0;

%(test_suite_runs)s

  return failed_test_case_count;
}

""" % locals(), force=options.force)


if options.all or options.test_py:
    if not '.' in sys.path:
        sys.path.append('.')

    for py_file_path in rglob(join(YIELD_DIR_PATH, "src", "yield", "**", "*_test.py")):
        py_dir_path, py_module_name = split(py_file_path)
        chdir(py_dir_path)
        py_module_name = splitext(py_module_name)[0]
        try: unlink(py_module_name + ".pyc")
        except: pass
        __import__(py_module_name)
