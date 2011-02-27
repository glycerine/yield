# yuild/constant.py

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

COMMENT_PREFIXES = {
    'c': "/*",
    "cpp": "//",
    "java": "//",
    "py": '#',
    "rl": "//",
}

COMMENT_SUFFIXES = {
    'c': "*/",
    "cpp": '',
    "java": '',
    "py": '',
    "rl": '',
}

INDENT_SPACES = {
    'c': ' ' * 2,
    "cpp": ' ' * 2,
    "java": ' ' * 4,
    "py": ' ' * 4,
    "rl": ' ' * 2,
}

C_INCLUDE_FILE_EXTENSIONS = (".h",)
C_INCLUDE_FILE_FNMATCH_PATTERNS = ['*' + ext for ext in C_INCLUDE_FILE_EXTENSIONS]
C_SOURCE_FILE_EXTENSIONS = (".c",)
C_SOURCE_FILE_FNMATCH_PATTERNS = ['*' + ext for ext in C_SOURCE_FILE_EXTENSIONS]
C_FILE_EXTENSIONS = C_INCLUDE_FILE_EXTENSIONS + C_SOURCE_FILE_EXTENSIONS
C_FILE_FNMATCH_PATTERNS = ['*' + ext for ext in C_FILE_EXTENSIONS]
for c_file_extension in C_FILE_EXTENSIONS:
    COMMENT_PREFIXES[c_file_extension] = COMMENT_PREFIXES['c']
    COMMENT_SUFFIXES[c_file_extension] = COMMENT_SUFFIXES['c']
    INDENT_SPACES[c_file_extension] = INDENT_SPACES['c']


CXX_INCLUDE_FILE_EXTENSIONS = (".hpp",)
CXX_INCLUDE_FILE_FNMATCH_PATTERNS = ['*' + ext for ext in CXX_INCLUDE_FILE_EXTENSIONS]
CXX_SOURCE_FILE_EXTENSIONS = (".cc", ".cpp")
CXX_SOURCE_FILE_FNMATCH_PATTERNS = ['*' + ext for ext in CXX_SOURCE_FILE_EXTENSIONS]
CXX_FILE_EXTENSIONS = CXX_INCLUDE_FILE_EXTENSIONS + CXX_SOURCE_FILE_EXTENSIONS
CXX_FILE_FNMATCH_PATTERNS = ['*' + ext for ext in CXX_FILE_EXTENSIONS]
for cpp_file_extension in CXX_FILE_EXTENSIONS:
    COMMENT_PREFIXES[cpp_file_extension] = COMMENT_PREFIXES["cpp"]
    COMMENT_SUFFIXES[cpp_file_extension] = COMMENT_SUFFIXES["cpp"]
    INDENT_SPACES[cpp_file_extension] = INDENT_SPACES["cpp"]


C_CXX_INCLUDE_FILE_EXTENSIONS = C_INCLUDE_FILE_EXTENSIONS + CXX_INCLUDE_FILE_EXTENSIONS
C_CXX_INCLUDE_FILE_FNMATCH_PATTERNS = ['*' + ext for ext in C_CXX_INCLUDE_FILE_EXTENSIONS]
C_CXX_SOURCE_FILE_EXTENSIONS = C_SOURCE_FILE_EXTENSIONS + CXX_SOURCE_FILE_EXTENSIONS
C_CXX_SOURCE_FILE_FNMATCH_PATTERNS = ['*' + ext for ext in C_CXX_SOURCE_FILE_EXTENSIONS]
C_CXX_FILE_EXTENSIONS = C_CXX_INCLUDE_FILE_EXTENSIONS + C_CXX_SOURCE_FILE_EXTENSIONS
C_CXX_FILE_FNMATCH_PATTERNS = C_CXX_INCLUDE_FILE_FNMATCH_PATTERNS + C_CXX_SOURCE_FILE_FNMATCH_PATTERNS

IDL_SOURCE_FILE_EXTENSIONS = (".idl",)
for idl_source_file_extension in IDL_SOURCE_FILE_EXTENSIONS:
    COMMENT_PREFIXES[idl_source_file_extension] = "//"
    COMMENT_SUFFIXES[idl_source_file_extension] = ''
    INDENT_SPACES[idl_source_file_extension] = INDENT_SPACES['c']

INCLUDE_FILE_EXTENSIONS = {}
for c_include_file_extension in C_INCLUDE_FILE_EXTENSIONS:
    INCLUDE_FILE_EXTENSIONS[c_include_file_extension] = 'c'
for cpp_include_file_extension in CXX_INCLUDE_FILE_EXTENSIONS:
    INCLUDE_FILE_EXTENSIONS[cpp_include_file_extension] = "cpp"
for idl_source_file_extension in IDL_SOURCE_FILE_EXTENSIONS:
    INCLUDE_FILE_EXTENSIONS[idl_source_file_extension] = "idl"

INCLUDE_FILE_FNMATCH_PATTERNS = ['*' + ext for ext in INCLUDE_FILE_EXTENSIONS.iterkeys()]

PLATFORMS = (
    '*',
    "bsd",
    "darwin",
    "freebsd",
    "linux",
    "posix",
    "sunos",
    "unix",
    "win32",
)

ABSTRACT_PLATFORMS = ("bsd", "unix", "posix", "*")
CONCRETE_PLATFORMS = ("darwin", "freebsd", "linux", "sunos", "win32")
assert len(ABSTRACT_PLATFORMS) + len(CONCRETE_PLATFORMS) == len(PLATFORMS)

PLATFORM_TREE = {
    '*': None,
    "bsd": "unix",
    "darwin": "bsd",
    "freebsd": "bsd",
    "linux": "unix",
    "posix": '*',
    "sunos": "unix",
    "unix": "posix",
    "win32": '*'
}
assert len(PLATFORM_TREE) == len(PLATFORMS)

PLATFORM_GUARDS = {
    '*': "#if 1",
    "bsd": "#if defined(__MACH__) || defined(__FreeBSD__)",
    "darwin": "#ifdef __MACH__",
    "freebsd": "#ifdef __FreeBSD__",
    "linux": "#ifdef __linux__",
    "posix": "#ifdef __unix__",
    "sunos": "#ifdef __sun",
    "unix": "#ifdef __unix__",
    "win32": "#ifdef _WIN32"
}
assert len(PLATFORM_GUARDS) == len(PLATFORMS)

PY_SOURCE_FILE_EXTENSIONS = (".py",)
for py_source_file_extension in PY_SOURCE_FILE_EXTENSIONS:
    COMMENT_PREFIXES[py_source_file_extension] = COMMENT_PREFIXES["py"]
    COMMENT_SUFFIXES[py_source_file_extension] = COMMENT_SUFFIXES["py"]
    INDENT_SPACES[py_source_file_extension] = INDENT_SPACES["py"]

RAGEL_SOURCE_FILE_EXTENSIONS = (".rl",)
for ragel_source_file_extension in RAGEL_SOURCE_FILE_EXTENSIONS:
    COMMENT_PREFIXES[ragel_source_file_extension] = COMMENT_PREFIXES["rl"]
    COMMENT_SUFFIXES[ragel_source_file_extension] = COMMENT_SUFFIXES["rl"]
    INDENT_SPACES[ragel_source_file_extension] = INDENT_SPACES["rl"]

SOURCE_FILE_EXTENSIONS = {}
for source_file_extension in C_SOURCE_FILE_EXTENSIONS:
    SOURCE_FILE_EXTENSIONS[source_file_extension] = 'c'
for source_file_extension in CXX_SOURCE_FILE_EXTENSIONS:
    SOURCE_FILE_EXTENSIONS[source_file_extension] = "cpp"
for source_file_extension in IDL_SOURCE_FILE_EXTENSIONS:
    SOURCE_FILE_EXTENSIONS[source_file_extension] = "idl"
for source_file_extension in PY_SOURCE_FILE_EXTENSIONS:
    SOURCE_FILE_EXTENSIONS[source_file_extension] = "py"
for source_file_extension in RAGEL_SOURCE_FILE_EXTENSIONS:
    SOURCE_FILE_EXTENSIONS[source_file_extension] = "rl"

SOURCE_FILE_FNMATCH_PATTERNS = ['*' + ext for ext in SOURCE_FILE_EXTENSIONS.iterkeys()]

SYS_PLATFORM_CHECKS = {
    '*': 'True:',
    "bsd": 'sys.platform == "darwin" or "bsd" in sys.platform',
    "darwin": 'sys.platform == "darwin"',
    "freebsd": 'sys.platform.startswith( "freebsd" )',
    "linux": 'sys.platform.startswith( "linux" )',
    "sunos": 'sys.platform.startswith( "sunos" )',
    "win32": 'sys.platform == "win32"',
}
SYS_PLATFORM_CHECKS["posix"] = \
    " or ".join(
        (
            SYS_PLATFORM_CHECKS["bsd"],
            SYS_PLATFORM_CHECKS["linux"],
            SYS_PLATFORM_CHECKS["sunos"]
        )
    )
SYS_PLATFORM_CHECKS["unix"] = SYS_PLATFORM_CHECKS["posix"]
for __platform in PLATFORMS: SYS_PLATFORM_CHECKS[__platform]

assert len(COMMENT_PREFIXES) == len(COMMENT_SUFFIXES)
for source_file_extension in COMMENT_PREFIXES.keys():
    assert COMMENT_SUFFIXES.has_key(source_file_extension)
    COMMENT_PREFIXES[source_file_extension[1:]] = COMMENT_PREFIXES[source_file_extension]
    COMMENT_SUFFIXES[source_file_extension[1:]] = COMMENT_SUFFIXES[source_file_extension]
