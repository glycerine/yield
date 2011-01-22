# yuild/source_files.py

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


from datetime import datetime
import getpass
from hashlib import md5
from os import sep as os_sep
from os.path import isfile, split, splitext

from yutil import deduplist, fnmatch, strlist

from yuild.constant import C_CXX_INCLUDE_FILE_EXTENSIONS, \
                           C_CXX_SOURCE_FILE_EXTENSIONS, \
                           COMMENT_PREFIXES, \
                           COMMENT_SUFFIXES, \
                           INCLUDE_FILE_EXTENSIONS, \
                           INDENT_SPACES, \
                           PLATFORMS, \
                           PLATFORM_GUARDS, \
                           SOURCE_FILE_EXTENSIONS
from yuild.platform_dict import PlatformDict
from yuild.source_file import SourceFile


__all__ = ["SourceFiles"]




class SourceFiles(list):
    def __init__(self, *args):
        list.__init__(self)

        if len(args) > 0:
            if len(args) == 1:
                if isinstance(args[0], PlatformDict):
                    raise NotImplementedError
                elif isinstance(args[0], list) or isinstance(args[0], tuple):
                    paths = args[0]
                else:
                    paths = strlist(args)
            else:
                paths = strlist(args)

            for path in deduplist(paths):
                self.append(SourceFile(path))

    def append(self, source_file):
        if isinstance(source_file, SourceFile):
            list.append(self, source_file)
        elif isinstance(source_file, basestring):
            list.append(self, SourceFile(source_file))
        else:
            raise TypeError, type(source_file)

    def as_platform_dict(self):
        platform_dict = {}
        for source_file in self:
            platform_dict.setdefault(
                source_file.get_platform(),
                []
            ).append(source_file.get_path())
        return PlatformDict(platform_dict)

    def exclude(self, pattern):
        filtered = SourceFiles()
        for source_file in self:
            if not fnmatch(source_file.get_path(), pattern):
                filtered.append(source_file)
        return filtered

    def filter(self, pattern):
        filtered = SourceFiles()
        for source_file in self:
            if fnmatch(source_file.get_path(), pattern):
                filtered.append(source_file)
        return filtered

    def format(self, *args, **kwds):
        for source_file in self:
            source_file.format(*args, **kwds)

    def get_combined_repr(self):
        language = None
        for source_file in self:
            if language is None:
                language = source_file.get_language()
            elif language != source_file.get_language():
                if language == "c" and source_file.get_language() == "cpp":
                    language = "cpp"
                elif language == "cpp" and source_file.get_language() == "c":
                    pass
                else:
                    raise RuntimeError, \
                          "cannot combine mixed file types: " + \
                          language + " vs. " + source_file.get_language() + \
                          " (" + source_file.get_path() + ")"

        comment_prefix = COMMENT_PREFIXES[language]
        comment_suffix = COMMENT_SUFFIXES[language]

        current_date_time = datetime.now().isoformat()
        current_user = getpass.getuser()

        source_files = []
        for file_ext in \
            deduplist(
                list(C_CXX_INCLUDE_FILE_EXTENSIONS) + \
                INCLUDE_FILE_EXTENSIONS.keys() + \
                SOURCE_FILE_EXTENSIONS.keys()
            ):
            for source_file in self:
                if source_file.get_path().endswith(file_ext):
                    source_files.append(source_file.get_combined_repr())
        source_files = "\n\n".join(source_files)

        return """\
%(comment_prefix)s Generated at %(current_date_time)s by %(current_user)s %(comment_suffix)s


%(source_files)s


""" % locals()
