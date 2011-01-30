# yuild/sconscript.py

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

import os
from os.path import exists

from yutil import indent, lpad, posixpath, posixpaths, quotestrlist, rpad

from yuild.constant import C_CXX_SOURCE_FILE_FNMATCH_PATTERNS, \
                           INDENT_SPACES, \
                           SYS_PLATFORM_CHECKS
from yuild.project import Project


__all__ = ["SConscript"]


# Constants
INDENT_SPACES = INDENT_SPACES["py"]


# Helper functions
def _if_check(check):
    if isinstance(check, basestring):
        if check.endswith(".h"):
            return 'build_conf.CheckHeader( "%(check)s" )' % locals()
        else:
            return 'build_conf.CheckLib( "%(check)s" )' % locals()
    elif isinstance(check, tuple) or isinstance(check, list):
        if len(check) == 0:
            return ""
        elif len(check) == 1:
            return _if_check(check[0])
        elif len(check) == 2:
            if check[0].endswith(".h"):
                language = "C"
                header = check[0]
                library = check[1]
            elif check[0].endswith(".hpp"):
                language = "C++"
                header = check[0]
                library = check[1]
            elif check[1].endswith(".h"):
                language = "C"
                header = check[1]
                library = check[0]
            elif check[1].endswith(".hpp"):
                language = "C++"
                header = check[1]
                library = check[0]
            else:
                raise ValueError, check

            return 'build_conf.CheckLibWithHeader( "%(library)s", "%(header)s", "%(language)s" )' % locals()
        else:
            return " and ".join([_if_check(subcheck) for subcheck in check])


class SConscript(Project):
    def __repr__(self):
        INDENT_SPACES = globals()["INDENT_SPACES"]

        project_references = []
        for project_reference in self.get_project_references().get('*', default=[]):
            SConscript = posixpath(project_reference + ".SConscript")
            project_references.append('SConscript( "%(SConscript)s" )' % locals())
        project_references = lpad("\n\n\n", '\n'.join(project_references))

        populate_build_environment = []

        for var, platform_dict in \
            (
                ("CXXDEFINES", self.get_cxxdefines()),
                ("CXXFLAGS", self.get_cxxflags()),
                ("CXXPATH", self.get_cxxpath()),
                ("LIBPATH", self.get_libpath()),
                ("LIBS", self.get_libs()),
                ("LIBFLAGS", self.get_ldflags()),
            ):
                platform_values = []
                have_sys_platform_check = False

                for platform, values in platform_dict.iteritems():
                    if len(values) > 0:
                        if var.endswith("PATH"):
                            values = posixpaths(values)
                        values = quotestrlist(values)
                        values = ", ".join(values)
                        if var == "LIBS":
                            append_method = "PrependUnique"
                        else:
                            append_method = "AppendUnique"
                        append_values = \
                            'build_env.%(append_method)s( %(var)s=[%(values)s] )' % locals()
                        if platform == "*":
                            platform_values.append(append_values)
                        else:
                            sys_platform_check \
 = SYS_PLATFORM_CHECKS[platform].\
                                    replace("sys.platform", "platform")

                            if have_sys_platform_check:
                                sys_platform_check = "elif " + sys_platform_check + ':'
                            else:
                                sys_platform_check = "if " + sys_platform_check + ':'
                                have_sys_platform_check = True

                            platform_values.append(
                                sys_platform_check + '\n' + \
                                INDENT_SPACES + append_values
                            )

                if len(platform_values) > 0:
                    platform_values = '\n'.join(platform_values)
                    populate_build_environment.append("""\
# %(var)s
%(platform_values)s
""" % locals())

        populate_build_environment = \
            lpad("\n\n\n", '\n'.join(populate_build_environment))

        sys_platform_check_win32 = \
            SYS_PLATFORM_CHECKS["win32"].replace("sys.platform", "platform")
        sys_platform_check_linux = \
            SYS_PLATFORM_CHECKS["linux"].replace("sys.platform", "platform")
        sys_platform_check_sunos = \
            SYS_PLATFORM_CHECKS["sunos"].replace("sys.platform", "platform")

        autoconf = []
        for platform, cppdefine_check in self.get_autoconf().iteritems():
            autoconf_block = []

            sys_platform_check = \
                SYS_PLATFORM_CHECKS[platform].replace("sys.platform", "platform")
            autoconf_block.append("if " + sys_platform_check + ':')

            for cppdefine, check in cppdefine_check.iteritems():
                if_check = _if_check(check)
                autoconf_block.append(
                    indent(
                        INDENT_SPACES,
                        """\
if not build_env.has_key( "CXXDEFINES" ) or not "%(cppdefine)s" in build_env["CXXDEFINES"]:
%(INDENT_SPACES)sif %(if_check)s:
%(INDENT_SPACES)s%(INDENT_SPACES)sbuild_env.AppendUnique( CXXDEFINES=["%(cppdefine)s"] )
""" % locals()
                    )
                )
            autoconf.append('\n'.join(autoconf_block))
        if len(autoconf) > 0:
            autoconf = '\n'.join(autoconf)
            autoconf = """

# autoconf
try:
    Import( "build_conf" )
except:
    build_conf = build_env.Configure()
    Export( "build_conf" )

%(autoconf)s
""" % locals()
        else:
            autoconf = ""


        type = self.get_type()
        if type == "lib":
            build_method = "Library"
        else:
            if type == "dll" or type == "so" or type == "py_ext":
                build_method = "SharedLibrary"
            elif type == "exe" or type == "bin" or type == "gui":
                build_method = "Program"

        output_file_path = posixpath(self.get_output_file_path()['*'])

        build_calls = []
        source_files = self.get_source_files()
        source_files = source_files.exclude(self.get_exclude_files())
        source_files = source_files.filter(C_CXX_SOURCE_FILE_FNMATCH_PATTERNS)

        have_sys_platform_check = False
        for platform, source_files in\
            source_files.as_platform_dict().iteritems(combine_platforms=True):
            assert len(source_files) > 0
            source_files = quotestrlist(posixpaths(source_files))
            source_files = ",\n" .join(source_files) + ','
            source_files = indent(INDENT_SPACES * 2, source_files)

            build_call = """\
build_env.%(build_method)s(
    "%(output_file_path)s",
    [
%(source_files)s
    ]
)""" % locals()
            if platform == "*":
                if have_sys_platform_check:
                    build_call = indent(INDENT_SPACES, build_call)
                    build_call = """\
else:
%(build_call)s
""" % locals()
            else:
                sys_platform_check = \
                    SYS_PLATFORM_CHECKS[platform].\
                        replace("sys.platform", "platform")
                build_call = indent(INDENT_SPACES, build_call)
                build_call = """\
if %(sys_platform_check)s:
%(build_call)s
""" % locals()
                if have_sys_platform_check:
                    build_call = "el" + build_call
                else:
                    have_sys_platform_check = True

            build_calls.append(build_call)

        build_call = "\n\n".join(build_calls)

        return """\
import os
from platform import machine
from sys import platform%(project_references)s


try:
    Import( "build_env" )
except:
    build_env = Environment()

    if %(sys_platform_check_win32)s:
        if ARGUMENTS.get( "release", 0 ):
            build_env.AppendUnique( CXXFLAGS=["/MD"] )
        else:
            build_env.AppendUnique( CXXFLAGS=["/MDd", "/ZI", "/W3"] )
    else:
        if %(sys_platform_check_linux)s:
            if machine() == "i686":
                build_env.AppendUnique( CXXFLAGS=["-march=i686"] )
        elif %(sys_platform_check_sunos)s:
            build_env["tools"] = ["gcc", "g++", "gnulink", "ar"]

        if ARGUMENTS.get( "coverage", 0 ):
            build_env.AppendUnique( CXXFLAGS=["-D_DEBUG", "--coverage"] )
            build_env.AppendUnique( LDFLAGS=["--coverage"] )
        elif ARGUMENTS.get( "profile-cpu", 0 ):
            build_env.AppendUnique( CXXFLAGS=["-pg"] )
            build_env.AppendUnique( LDFLAGS=["-pg"] )
        elif ARGUMENTS.get( "profile-heap", 0 ):
            build_env.AppendUnique( CXXFLAGS=["-fno-omit-frame-pointer"] )
            build_env.AppendUnique( LIBS=["tcmalloc"] )
        elif not ARGUMENTS.get( "release", 0 ):
            build_env.AppendUnique( CXXFLAGS=["-g", "-D_DEBUG"] )
        else:
            build_env.AppendUnique( CXXFLAGS=["-O2"] )

    Export( "build_env" )%(populate_build_environment)s%(autoconf)s


%(build_call)s
""" % locals()
