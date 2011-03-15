# yutil.py

# Copyright (c) 2011 Minor Gordon
# All rights reserved

# This source file is part of the yutil project.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
# * Neither the name of the yutil project nor the
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

from copy import copy, deepcopy
from cStringIO import StringIO
from fnmatch import fnmatch as _fnmatch
from glob import iglob
from hashlib import md5
from inspect import isclass
from os import curdir, getcwd, listdir, makedirs, pardir, walk as _walk, sep as os_sep
from os.path import abspath, exists, isdir, isfile, join, normcase, relpath, split
import shutil


__all__ = \
[
  "abspath", "abspaths",
  "bstrip",
  "copy_file",
  "deduplist",
  "fnmatch",
  "indent",
  "ishidden",
  "istype",
 "list_files",
  "list_subdirectories",
  "lpad",
  "merge_dicts",
  "mirror_subdirectories",
  "ntpath", "ntpaths",
  "pad",
  "posixpath", "posixpaths",
  "quote",
  "quotestrlist",
  "reinterpret_cast",
  "relpath", "relpaths",
  "rglob",
  "rpad",
  "spacify",
  "static_cast",
  "strlist",
  "strlistprefix",
  "strlistsort",
  "touch",
  "treepaths",
  "tstrip",
  "typelist",
  "walk",
  "write_file",
]


# Constants
DEBUG = False


def abspaths(paths):
    return __applypaths(paths, abspath)


def __applypaths(paths, function):
    if paths is None:
        return []
    elif isinstance(paths, list) or isinstance(paths, tuple):
        return [function(path) for path in paths]
    elif isinstance(paths, dict):
        paths_copy = {}
        for key in paths.iterkeys():
            paths_copy[key] = __applypaths(paths[key], function)
        return paths_copy
    elif isinstance(paths, basestring):
        return function(paths)
    else:
        raise TypeError, "invalid paths type: " + str(type(paths))


def bstrip(lines, pattern=''):
    if len(lines) > 0:
        lines = copy(lines)
        line_i = len(lines) - 1
        while line_i > 0:
            if __startswith(lines[line_i].strip(), pattern):
                del lines[line_i]
                line_i -= 1
            else:
                break
    return lines


def copy_file(from_path, to_path):
    file_hashes = []
    for path in (from_path, to_path):
        try:
            file_hashes.append(md5(open(path).read()).digest())
        except IOError:
            file_hashes.append(0)

    if file_hashes[0] != file_hashes[1]:
        shutil.copyfile(from_path, to_path)
        print "copied", from_path, "to", to_path
        return True
    else:
        return False


def deduplist(x):
    z = []
    for y in x:
        if y not in z:
            z.append(y)
    return z


def fnmatch(path, pattern):
    name = split(path)[1]
    if isinstance(pattern, basestring):
        if _fnmatch(name, pattern) or\
           _fnmatch(path, pattern):
            return True
    elif hasattr(pattern, "match"): # A regex-like object
        if pattern.match(name) or pattern.match(file_path):
            found_file_paths.append(file_path)
    elif isinstance(pattern, tuple) or isinstance(pattern, list):
        for subpattern in strlist(pattern):
            if fnmatch(path, subpattern):
                return True

    return False


def indent(spaces, text):
    if len(spaces) > 0 and len(text) > 0:
        if isinstance(text, tuple) or isinstance(text, list):
            return [indent(spaces, line) for line in text]
        else:
            indented_lines = []
            for line in StringIO(str(text)).readlines():
                line = line.rstrip()
                if len(line) > 0:
                    indented_lines.append(spaces + line)
                else:
                    indented_lines.append(line)
            return '\n'.join(indented_lines)
    else:
        return text


def ishidden(path):
    for path_segment in path.split(os_sep):
        if len(path_segment) > 0 and\
           path_segment[0] == '.' and\
           path_segment != curdir and path_segment != pardir:
            return True
    return False


def istype(type_):
    return isinstance(type_, type) or isclass(type_)


def list_files(dir_path):
    for file_name in listdir(dir_path):
        file_path = join(dir_path, file_name)
        if isfile(file_path):
            yield file_path


def list_subdirectories(root_dir_path, include_hidden=False):
    for dir_name in listdir(root_dir_path):
        dir_path = join(root_dir_path, dir_name)
        if isdir(dir_path):
            if include_hidden or not ishidden(dir_path):
                yield dir_path


def lpad(padding, s):
    if s is not None:
        if len(s) > 0:
            return padding + s
        else:
            return s
    else:
        return ""


def merge_dicts(a, b):
    a_copy = deepcopy(a)
    if len(a) > 0 or len(b) > 0:
        b_copy = deepcopy(b)
        for a_key, a_value in a_copy.iteritems():
            try:
                b_value = b_copy[a_key]
                del b_copy[a_key]
            except KeyError:
                continue

            assert a_value.__class__ == b_value.__class__
            if isinstance(b_value, list):
                a_value.extend(b_value)
                a_value = deduplist(a_value)
            elif isinstance(b_value, dict):
                merge_dicts(a_value, b_value)

        a_copy.update(b_copy)
    return a_copy


def mirror_subdirectories(source_dir_path, target_dir_path):
    if exists(source_dir_path):
        if not exists(target_dir_path):
            makedirs(target_dir_path)

        for root_dir_path, dir_names, _ in _walk(source_dir_path):
            if not ishidden(root_dir_path):
                for dir_name in dir_names:
                    source_subdir_path = join(root_dir_path, dir_name)
                    if not ishidden(source_subdir_path):
                        source_subdir_path = relpath(source_subdir_path, source_dir_path)
                        target_subdir_path = join(target_dir_path, source_subdir_path)
                        if not exists(target_subdir_path):
                            makedirs(target_subdir_path)


def ntpath(path):
    return path.replace(os_sep, '\\')

def ntpaths(paths):
    return __applypaths(paths, ntpath)


def pad(left_padding, s, right_padding):
    if s is not None:
        if len(s) > 0:
            return left_padding + s + right_padding
        else:
            return s
    else:
        return ""


def posixpath(path):
    return path.replace(os_sep, '/')

def posixpaths(paths):
    return __applypaths(paths, posixpath)


def quote(x):
    assert isinstance(x, basestring)
    if not '"' in x: return '"' + x + '"'
    elif not "'" in x: return "'" + x + "'"
    else: raise ValueError, x


def quotestrlist(x):
    return [quote(y) for y in strlist(x)]


def reinterpret_cast(x, expected_type):
    if isinstance(x, expected_type):
        return x
    else:
        return expected_type(x)


def relpaths(paths, start=None):
    if start is None:
        start = getcwd()
    return __applypaths(abspaths(paths), lambda path: relpath(path, start))


def rglob(path):
    if DEBUG:
        print "rglob(", path, ")"

    if isinstance(path, tuple) or isinstance(path, list):
        for subpath in path:
            for path in rglob(subpath):
                yield path
        return
    else:
        path_parts = []
        head, tail = split(str(path))
        while len(tail) > 0:
            path_parts.append(tail)
            head, tail = split(head)
        if len(head) > 0:
            path_parts.append(head)
        path_parts.reverse()

        for path in __rglob(path_parts, 0):
            if DEBUG:
                print "rglob: yielding", path

            yield path

        if DEBUG:
            print

def __rglob(path_parts, path_part_i):
    if DEBUG:
        print " __rglob(", path_parts, ",", path_part_i, ")"

    assert len(path_parts) > 0
    assert path_part_i < len(path_parts)

    if path_parts[path_part_i] == "**":
        if path_part_i == 0:
            path = curdir
        else:
            path = join(*path_parts[:path_part_i])

        if path_part_i + 1 < len(path_parts):
            sub_path_parts = path_parts[:path_part_i] + \
                             path_parts[path_part_i + 1:]
            sub_path = join(*sub_path_parts)
            if DEBUG:
                print " __rglob: iglob1(", sub_path, ")"
            for sub_path in iglob(sub_path):
                yield sub_path

            for _1, dir_paths, _2 in\
                walk(path, abspaths=True, include_hidden=False):

                for dir_path in dir_paths:
                    sub_path_parts = path_parts[:path_part_i] + \
                                     [dir_path[len(path) + 1:]] + \
                                     path_parts[path_part_i + 1:]
                    sub_path = join(*sub_path_parts)
                    if DEBUG:
                        print " __rglob: recursing into", dir_path, sub_path
                    for sub_path in __rglob(sub_path_parts, path_part_i + 1):
                        yield sub_path
        else:
            for _, dir_paths, file_paths in walk(path, abspaths=True, include_hidden=False):
                for path in file_paths + dir_paths:
                    yield path
    elif path_part_i + 1 < len(path_parts):
        path = join(*path_parts[:path_part_i + 1])
        if DEBUG:
            print " __rglob: iglob2(", path, ")"
        for path in iglob(path):
            if isdir(path):
                for path in __rglob(path_parts, path_part_i + 1):
                    yield path
    else:
        path = join(*path_parts[:path_part_i + 1])
        if DEBUG:
            print " __rglob: iglob3(", path, ")"
        for path in iglob(path):
            yield path


def rpad(s, padding):
    if s is not None:
        if len(s) > 0:
            return s + padding
        else:
            return s
    else:
        return ""


def spacify(x, spaces=' ' * 4):
    if isinstance(x, basestring):
        return x.replace('\t', spaces)
    elif isinstance(x, tuple) or isinstance(x, list):
        return [y.replace('\t', spaces) for y in x]
    else:
        raise ValueError, str(type(x))


def __startswith(line, pattern):
    if pattern is None:
        return len(line) == 0
    if isinstance(pattern, basestring):
        if len(pattern) == 0:
            return len(line) == 0
        else:
            return line.startswith(pattern)
    elif hasattr(pattern, "match"):
        return pattern.match(line) is not None
    elif isinstance(pattern, tuple) or isinstance(pattern, list):
        for subpattern in pattern:
            if __startswith(line, subpattern):
                return True
    else:
        raise TypeError, str(type(pattern))


def static_cast(x, expected_type):
    if istype(expected_type):
        if isinstance(x, expected_type):
            return x
        else:
            raise TypeError, type(x)
    elif isinstance(expected_type, tuple) or isinstance(expected_type, list):
        expected_types = expected_type
        for expected_type in expected_types:
            if istype(expected_type):
                if isinstance(x, expected_type):
                    return x
            elif x == expected_type:
                return x

        if hasattr(x, "__class__"):
            repr_type_x = x.__class__.__name__
        else:
            repr_type_x = repr(type(x))

        raise TypeError, repr_type_x + " vs. " + repr(expected_types)
    else:
        raise TypeError, type(x)


def strlist(x):
    return typelist(x, str)


def strlistprefix(x):
    prefix = []
    while True:
        len_prefix = len(prefix)
        test_char = None
        for test_string in strlist(x):
            if len(test_string) > len_prefix:
                if test_char is None:
                    test_char = test_string[len_prefix]
                elif test_string[len_prefix] == test_char:
                    pass
                else:
                    return "".join(prefix)
            else:
                return "".join(prefix)
        prefix.append(test_char)
    return "".join(prefix)


def strlistsort(x):
    y_lower = dict((y.lower(), y) for y in x)
    y_lower_keys = copy(y_lower.keys())
    y_lower_keys.sort()
    return [y_lower[key] for key in y_lower_keys]


def touch(file_path):
    if not exists(file_path):
        open(file_path, "w+").close()


# Create a tree of { dir_path: { path: path } }
def treepaths(paths):
    assert isinstance(paths, list) or isinstance(paths, tuple)
    tree = {}
    for path in paths:
        str_path = str(path)
        subtree = tree
        path_segments = str_path.split(os_sep)
        for path_segment_i in xrange(len(path_segments)):
            path_segment = path_segments[path_segment_i]
            if path_segment_i < len(path_segments) - 1:
                dir_path = os_sep.join(path_segments[:path_segment_i + 1])
                try:
                    subtree = subtree[dir_path]
                    if not isinstance(subtree, dict): # dir_path is set to a file
                        raise ValueError, "subtree[" + dir_path + "] already set to a file"
                except KeyError:
                    subtree[dir_path] = {}
                    subtree = subtree[dir_path]
            elif not subtree.has_key(str_path):
                subtree[str_path] = path
            else:
                raise ValueError, "subtree[" + str_path + "] already set to", subtree[str_path]

    while len(tree) == 1 and \
          isinstance(tree.values()[0], dict) and \
          len(tree.values()[0]) == 1:
            tree = tree.values()[0]

    return tree


def tstrip(lines, pattern=''):
    lines = copy(lines)
    line_i = 0
    while line_i < len(lines):
        if __startswith(lines[line_i].strip(), pattern):
            del lines[line_i]
        else:
            break

    return lines


def typelist(x, item_type=None):
    if x is None:
        return []
    else:
        if isinstance(x, tuple):
            x = list(x)
        elif isinstance(x, list):
            pass
        elif item_type is not None:
            if isinstance(x, item_type):
                return [x]
            else:
                return [item_type(x)]
        else:
            raise TypeError, type(x)

        if item_type is None:
            return x
        else:
            for item in x:
                if not isinstance(item, item_type):
                    new_x = []
                    for item in x:
                        if isinstance(item, item_type):
                            new_x.append(x)
                        else:
                            new_x.append(item_type(item))
                    return new_x
            return x


def walk(root_dir_path, abspaths=False, include_hidden=True):
    for root_dir_path, dir_names, file_names in _walk(root_dir_path):
        if include_hidden or not ishidden(root_dir_path):
            if abspaths:
                dir_paths = []
                for dir_name in dir_names:
                    dir_path = join(root_dir_path, dir_name)
                    if include_hidden or not ishidden(dir_path):
                        dir_paths.append(dir_path)

                file_paths = []
                for file_name in file_names:
                    file_path = join(root_dir_path, file_name)
                    if include_hidden or not ishidden(file_path):
                        file_paths.append(file_path)

                yield root_dir_path, dir_paths, file_paths
            elif not include_hidden:
                dir_names = [dir_name for dir_name in dir_names
                             if not ishidden(join(root_dir_path, dir_name))]
                file_names = [file_name for file_name in file_names
                              if not ishidden(join(root_dir_path, file_name))]

                yield root_dir_path, dir_names, file_names
            else:
                yield root_dir_path, dir_names, file_names


def write_file(path, contents, force=False, newline='\n'):
    if isinstance(contents, basestring):
        pass
    elif isinstance(contents, tuple) or isinstance(contents, list):
        contents = newline.join(contents)
    else:
        contents = str(contents)

    if not force:
        try:
            old_file = open(path)

            file_hashes = []
            for file in (old_file, StringIO(contents)):
                file_hash = md5()

                for line in file.readlines():
                    line = line.strip()
                    if len(line) == 0:
                        pass # Ignore empty lines
                    else:
                        file_hash.update(line)

                file_hashes.append(file_hash.digest())

            if file_hashes[0] == file_hashes[1]:
                return False

        except IOError: # old file does not exist
            pass

    open(path, "wb").write(contents)
    print "wrote", path
    return True
