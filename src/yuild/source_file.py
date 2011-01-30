# yuild/source_file.py

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
from hashlib import md5
from os import sep as os_sep
from os.path import split, splitext
from time import gmtime

from yutil import bstrip, \
                  lpad, \
                  pad, \
                  rpad, \
                  spacify, \
                  tstrip

from yuild.constant import C_CXX_INCLUDE_FILE_EXTENSIONS, \
                           COMMENT_PREFIXES, \
                           COMMENT_SUFFIXES, \
                           INCLUDE_FILE_EXTENSIONS, \
                           INDENT_SPACES, \
                           PLATFORMS, \
                           PLATFORM_GUARDS, \
                           SOURCE_FILE_EXTENSIONS


__all__ = ["SourceFile"]


# Constants
BOILERPLATE = """\
%(path)s

Copyright (c) %(current_year)u %(author)s%(acknowledgments)s
All rights reserved

This source file is part of the %(project)s project.%(license)s
"""

NEW_BSD_LICENSE = """\
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the %(project)s project nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL %(author)s BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
"""


class SourceFile:
    def __init__(
        self,
        path,
        language=None,
        platform=None
    ):
        assert isinstance(path, basestring), type(path)

        if language is None:
            ext = splitext(path)[1]
            language = INCLUDE_FILE_EXTENSIONS.get(ext)
            if language is None:
                language = SOURCE_FILE_EXTENSIONS.get(ext, "other")
        self.__language = language

        self.__path = path

        if platform is None:
            for path_part in path.split(os_sep):
                if path_part in PLATFORMS:
                    platform = path_part
                    break
            if platform is None:
                platform = '*'
        self.__platform = platform

    def __eq__(self, other):
        if isinstance(other, SourceFile):
            return self.get_path() == other.get_path()
        elif isinstance(other, basestring):
            return self.get_path() == other
        else:
            raise NotImplementedError

    def format(
        self,
        author,
        project,
        acknowledgments=None,
        force=False,
        license=NEW_BSD_LICENSE,
    ):
        path = self.__get_display_path()
        acknowledgments = lpad("\n", acknowledgments)
        current_year = gmtime().tm_year
        license = license is not None and lpad("\n\n", license % locals()) or ""
        boilerplate = BOILERPLATE % locals()
        boilerplate = StringIO(boilerplate).readlines()
        boilerplate = [line.strip() for line in boilerplate]

        file = open(self.get_path(), "rb")
        lines = [line.rstrip() for line in file.readlines()]
        file.close()

        if force:
            old_hash = False
        else:
            old_hash = md5("".join(lines)).digest()

        comment_prefix = rpad(self.__get_comment_prefix(), ' ')
        comment_suffix = lpad(' ', self.__get_comment_suffix())

        interpreter_line = None
        if len(lines) > 0 and lines[0].startswith("#!"):
            interpreter_line = lines[0]
            lines = lines[1:]

        # Cut out empty lines at the top and bottom
        lines = tstrip(lines)
        lines = bstrip(lines)

        # Cut out comment lines at the top
        lines = tstrip(lines, [comment_prefix, ''])

        # Cut out empty lines after the boilerplate
        lines = tstrip(lines)

        if self.get_language() == "cpp":
            # Cut out the header guard
            line_i = 0
            while line_i < len(lines):
                line = lines[line_i].strip()
                if line.endswith("_H_") or line.endswith("_HPP_"):
                    if line.startswith("#ifndef _"): pass
                    elif line.startswith("#define _"): pass
                    else: break
                    del lines[line_i]
                elif len(line) == 0:
                    line_i += 1
                else:
                    break

        lines = tstrip(lines)
        lines = bstrip(lines, "#endif")
        lines = bstrip(lines)

        if splitext(self.get_path())[1] in C_CXX_INCLUDE_FILE_EXTENSIONS:
           # Generate a new header guard and add it back
           guard = self.__get_c_cpp_header_guard()
           lines.insert(0, "#ifndef _%(guard)s_" % locals())
           lines.insert(1, "#define _%(guard)s_" % locals())
           lines.insert(2, "")
           if len(lines[-1]) != 0: lines.append("")

           lines.append("#endif")

        # Add the interpreter line and boilerplate back
        old_lines = lines
        lines = []
        if interpreter_line is not None:
            lines.extend((interpreter_line, ''))
        lines.extend([
            pad(
                comment_prefix,
                line.strip(),
                comment_suffix
            )
            for line in boilerplate
        ])
        lines.extend(old_lines)

        # Convert tabs to spaces
        lines = spacify(lines, INDENT_SPACES.get(self.get_language(), '  '))

        if len(lines[-1]) > 0:
            # Add an empty line at the end of the file
            lines.append("")

        if force:
            new_hash = True
        else:
            new_hash = md5("".join(lines)).digest()

        if new_hash != old_hash:
            new_file = open(self.get_path(), "wb")
            new_file.write("\n".join(lines))
            new_file.close()
            print "wrote", self.get_path()

    def get_combined_repr(self):
        comment_prefix = self.__get_comment_prefix()

        lines = []
        for line in open(self.get_path()).readlines():
            line_stripped = line.strip()
            if line_stripped.startswith("#include \""):
                pass
            elif line_stripped.startswith(comment_prefix):
                if len(lines) > 0:
                    lines.append(line.rstrip())
            elif len(line_stripped) == 0:
                if len(lines) > 0:
                    lines.append(line_stripped)
            else:
                lines.append(line.rstrip())

        display_path = self.__get_display_path()
        comment_prefix = self.__get_comment_prefix()
        comment_suffix = self.__get_comment_suffix()
        lines.insert(
            0,
            "%(comment_prefix)s %(display_path)s %(comment_suffix)s" % locals()
        )

        if self.get_platform() != '*':
            lines.insert(1, PLATFORM_GUARDS[self.get_platform()])
            lines.append("#endif")

        lines.extend(('', ''))

        return '\n'.join(lines)

    def __get_comment_prefix(self):
        return COMMENT_PREFIXES.get(self.get_language(), '#')

    def __get_comment_suffix(self):
        return COMMENT_SUFFIXES.get(self.get_language(), '')

    def __get_c_cpp_header_guard(self):
        for base_dir_name in ("include", "src"):
            base_dir_name_pos = self.get_path().find(base_dir_name + os_sep)
            if base_dir_name_pos != -1:
               return self.get_path()[base_dir_name_pos + len(base_dir_name) + 1:].\
                      replace(os_sep, "_").\
                      replace(".", "_").\
                      upper()
        return self.get_path().\
               replace(os_sep, "_").\
               replace(".", "_").\
               upper()

    def __get_display_path(self):
        for base_dir_name in ("include", "src",):
           base_dir_name_pos = self.get_path().find(base_dir_name + os_sep)
           if base_dir_name_pos != -1:
               return self.get_path()[base_dir_name_pos + len(base_dir_name) + 1:].\
                      replace(os_sep, '/')
        return split(self.get_path())[1]

    def get_language(self):
        return self.__language

    def get_path(self):
        return self.__path

    def get_platform(self):
        return self.__platform

    def __repr__(self):
        return self.get_path()

    def __str__(self):
        return self.get_path()
