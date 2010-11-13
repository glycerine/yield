# config.py

# Copyright (c) 2010 Minor Gordon
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


from os.path import abspath, dirname, exists, join
from pprint import pprint
import sys

from yuild.constant import INCLUDE_FILE_FNMATCH_PATTERNS,\
                           SOURCE_FILE_FNMATCH_PATTERNS
from yutil import deduplist, rglob


__all__ =\
[
    "AUTHOR",
    "AUTOCONF",
    "CXXFLAGS",
    "CXXPATH",
    "EXCLUDE_FILE_PATHS",
    "INCLUDE_FILE_PATHS",
    "LIBPATH",
    "LIBS",
    "LDFLAGS",
    "PROJECT_BUILD_ORDER",
    "PROJECT_NAMES",
    "PROJECT_REFERENCES",
    "PROJECT_REFERENCES_EXPANDED",
    "SOURCE_FILE_PATHS",
    "THIRD_PARTY_SOURCE_FNMATCH_PATTERNS",
    "YIELD_DIR_PATH",
    "YIELD_INCLUDE_DIR_PATH",
    "YIELD_SRC_DIR_PATH",
]


AUTHOR = "Minor Gordon"


MY_DIR_PATH = dirname( abspath( sys.modules[__name__].__file__ ) )
YIELD_DIR_PATH = abspath( join( MY_DIR_PATH, ".." ) )
YIELD_INCLUDE_DIR_PATH = join( YIELD_DIR_PATH, "include" )
YIELD_SRC_DIR_PATH = join( YIELD_DIR_PATH, "src" )


AUTOCONF = {}

AUTOCONF["net"] =\
{
#    "linux": { "YIELD_HAVE_LINUX_LIBUUID": ( "uuid", "uuid/uuid.h" ), },
#    "unix": { "YIELD_HAVE_OPENSSL": ( "crypto", "ssl", "ssl.h" ), },
#    "win32": { "YIELD_HAVE_OPENSSL": ( "libeay32", "ssleay32", "ssl.h" ), },
}

AUTOCONF["thread"] = \
{
#    "unix": { "YIELD_HAVE_PAPI": ( "papi", "papi.h" ), },
}

#    "unix": { "YIELD_HAVE_ZLIB": ( "z", "zlib.h" ), },
#    "win32":  { "YIELD_HAVE_ZLIB": ( "zdll", "zlib.h" ), },

CXXFLAGS =\
{
    "unix": ["-fno-rtti", "-Wall", "-Wold-style-cast", "-Wunused-macros"],
    "win32":
    [
      "/EHsc", # Enable exceptions
      "/GR-", # Disable RTTI
      "/nologo",
      "/wd\"4100\"",
      "/wd\"4127\"",
      "/wd\"4290\"",
      "/wd\"4355\"",
      "/wd\"4512\"",
      # "/Za" # Disable language extensions
    ]
}

CXXPATH =\
{
    "*": \
    [
        YIELD_INCLUDE_DIR_PATH,
    ]
}

LDFLAGS =\
{
    "win32":
    [
        "/ignore:4006",
        "/ignore:4221"
    ],
}

LIBPATH =\
{
    "*": [join( YIELD_DIR_PATH, "lib" )],
    # "win32": [join( YIELD_DIR_PATH, "lib", "win32" )]
}

LIBS = {}

LIBS["aio"] =\
{
    "linux": ["aio"],
}

LIBS["common"] =\
{
    "linux": ["rt", "stdc++"],
    "sunos": ["m", "rt", "stdc++"],
}

LIBS["i18n"] =\
{
    "darwin": ["iconv"],
    "freebsd": ["iconv", "intl"],
    "sunos": ["iconv"],
}

LIBS["net"] =\
{
    "sunos": ["nsl", "socket", "uuid"],
}

LIBS["process"] =\
{
    "linux": ["dl"],
}

LIBS["thread"] =\
{
    "freebsd": ["pthread"],
    "linux": ["pthread"],
    "sunos": ["cpc", "kstat"],
}


PROJECT_REFERENCES = {}
PROJECT_REFERENCES["aio"] = ( "fs", "net", "poll", "stage" )
PROJECT_REFERENCES["common"] = tuple()
PROJECT_REFERENCES["fs"] = ( "i18n", )
PROJECT_REFERENCES["http"] = ( "aio", "stage" )
PROJECT_REFERENCES["i18n"] = ( "common", )
PROJECT_REFERENCES["marshal"] = ( "common", )
PROJECT_REFERENCES["net"] = ( "common", )
PROJECT_REFERENCES["poll"] = ( "net", "thread", )
PROJECT_REFERENCES["process"] = ( "fs", )
PROJECT_REFERENCES["stage"] = ( "thread", )
PROJECT_REFERENCES["thread"] = ( "common", )

# Expand project references so that the list has dependencies going left -> right, e.g.
# if project A depends on project B depends on project C, the order will be [A, B, C]
def __expand_project_references( project_name ):
    project_references = []
    for project_reference in PROJECT_REFERENCES[project_name]:
        project_references.append( project_reference )
        for project_reference in __expand_project_references( project_reference ):
            try:
                del project_references[project_references.index( project_reference )]
            except ValueError:
                pass
            project_references.append( project_reference )
    return project_references
PROJECT_REFERENCES_EXPANDED = {}
for project_name in PROJECT_REFERENCES.iterkeys():
    PROJECT_REFERENCES_EXPANDED[project_name] =\
        deduplist( __expand_project_references( project_name ) )

# pprint( PROJECT_REFERENCES_EXPANDED )

LIBS_EXPANDED = {}
for project_name, project_references in PROJECT_REFERENCES_EXPANDED.iteritems():
    # Add all of the project references' libs to the project's libs
    LIBS_EXPANDED[project_name] = {}
    for project_reference in project_references + [project_name]:
        try:
            for platform, platform_libs in LIBS[project_reference].iteritems():
                LIBS_EXPANDED[project_name].setdefault( platform, [] ).extend( platform_libs )
        except KeyError:
            pass

    # Add the project references' outputs to the project's libs
    for project_reference in project_references:
        project_reference_lib = "yield_" + project_reference
        LIBS_EXPANDED[project_name].setdefault( '*', [] )
        assert project_reference_lib not in LIBS_EXPANDED[project_name]['*'], LIBS_EXPANDED[project_name]['*']
        LIBS_EXPANDED[project_name]['*'].append( project_reference_lib )

    # dedup
    for platform in LIBS_EXPANDED[project_name].iterkeys():
        LIBS_EXPANDED[project_name][platform] = deduplist( LIBS_EXPANDED[project_name][platform] )
LIBS = LIBS_EXPANDED

# pprint( LIBS )

PROJECT_NAMES = PROJECT_REFERENCES.keys()

PROJECT_BUILD_ORDER = []
def __fill_project_build_order( project_name ):
    for project_reference in PROJECT_REFERENCES[project_name]:
        __fill_project_build_order( project_reference )

    if project_name not in PROJECT_BUILD_ORDER:
        PROJECT_BUILD_ORDER.append( project_name )
__fill_project_build_order( "http" )
__fill_project_build_order( "marshal" )
__fill_project_build_order( "process" )
assert len( PROJECT_BUILD_ORDER ) == len( PROJECT_NAMES )


EXCLUDE_FILE_PATHS = {}
INCLUDE_FILE_PATHS = {}
SOURCE_FILE_PATHS = {}


for project_name in PROJECT_NAMES:
    if project_name != "common":
        project_include_dir_path = join( YIELD_INCLUDE_DIR_PATH, "yield", project_name )
        assert exists( project_include_dir_path ), project_include_dir_path
        project_source_dir_path = join( YIELD_SRC_DIR_PATH, "yield", project_name )
        assert exists( project_source_dir_path ), project_source_dir_path

        if not EXCLUDE_FILE_PATHS.has_key( project_name ):
            EXCLUDE_FILE_PATHS[project_name] =\
            [
                 join( project_source_dir_path, "**", "*_test" + fnmatch_pattern )
                 for fnmatch_pattern in SOURCE_FILE_FNMATCH_PATTERNS
            ]

        EXCLUDE_FILE_PATHS.setdefault( project_name + "_test", [] )

        if not INCLUDE_FILE_PATHS.has_key( project_name ):
            INCLUDE_FILE_PATHS[project_name] =\
            [
                join( project_include_dir_path, "**", fnmatch_pattern )
                for fnmatch_pattern in INCLUDE_FILE_FNMATCH_PATTERNS
            ]

        INCLUDE_FILE_PATHS.setdefault( project_name + "_test", [] )

        if not SOURCE_FILE_PATHS.has_key( project_name ):
            SOURCE_FILE_PATHS[project_name] =\
            [
                 join( project_source_dir_path, "**", fnmatch_pattern )
                 for fnmatch_pattern in INCLUDE_FILE_FNMATCH_PATTERNS +\
                                        SOURCE_FILE_FNMATCH_PATTERNS
            ]

        if not SOURCE_FILE_PATHS.has_key( project_name + "_test" ):
            SOURCE_FILE_PATHS[project_name + "_test"] =\
            [
                join( project_source_dir_path, "**", "*_test" + fnmatch_pattern )
                for fnmatch_pattern in INCLUDE_FILE_FNMATCH_PATTERNS +\
                                       SOURCE_FILE_FNMATCH_PATTERNS
            ]\
            +\
            [
                join( project_source_dir_path, "**", "test_" + fnmatch_pattern )
                for fnmatch_pattern in INCLUDE_FILE_FNMATCH_PATTERNS +\
                                       SOURCE_FILE_FNMATCH_PATTERNS
            ]\
            +\
            [join( project_source_dir_path, "yield_" + project_name + "_test_main.cpp" )]

EXCLUDE_FILE_PATHS["common"] =\
[
    join( YIELD_SRC_DIR_PATH, "yield", "*_test" + fnmatch_pattern )
    for fnmatch_pattern in SOURCE_FILE_FNMATCH_PATTERNS
]

EXCLUDE_FILE_PATHS["common_test"] = []

INCLUDE_FILE_PATHS["aio"] =\
[
    join( YIELD_INCLUDE_DIR_PATH, "yield", "aio", "posix", "aiocb.hpp" ),
    join( YIELD_INCLUDE_DIR_PATH, "yield", "aio", "win32", "aiocb.hpp" ),
    join( YIELD_INCLUDE_DIR_PATH, "yield", "aio", "aiocb.hpp" ),
    join( YIELD_INCLUDE_DIR_PATH, "yield", "aio", "net", "sockets", "aiocb.hpp" ), # before accept_aiocb
]+\
INCLUDE_FILE_PATHS["aio"]

INCLUDE_FILE_PATHS["common"] =\
[
    join( YIELD_INCLUDE_DIR_PATH, "yield", "types.hpp" ), # before atomic
    join( YIELD_INCLUDE_DIR_PATH, "yield", "atomic.hpp" ), # before object
    join( YIELD_INCLUDE_DIR_PATH, "yield", "object.hpp" ), # before event
    join( YIELD_INCLUDE_DIR_PATH, "yield", "event.hpp" ), # before message
    join( YIELD_INCLUDE_DIR_PATH, "yield", "message.hpp" ), # before response
    join( YIELD_INCLUDE_DIR_PATH, "yield", "response.hpp" ), # before exception
    join( YIELD_INCLUDE_DIR_PATH, "yield", "time.hpp" ), # before event_queue
]+\
[
    join( YIELD_INCLUDE_DIR_PATH, "yield", fnmatch_pattern )
    for fnmatch_pattern in INCLUDE_FILE_FNMATCH_PATTERNS
]

INCLUDE_FILE_PATHS["common_test"] = \
[join( YIELD_INCLUDE_DIR_PATH, "yunit.hpp" )]

SOURCE_FILE_PATHS["common"] =\
[
    join( YIELD_SRC_DIR_PATH, "yield", fnmatch_pattern )
    for fnmatch_pattern in INCLUDE_FILE_FNMATCH_PATTERNS +\
                           SOURCE_FILE_FNMATCH_PATTERNS
]

SOURCE_FILE_PATHS["common_test"] =\
[
    join( YIELD_SRC_DIR_PATH, "yield", "*_test" + fnmatch_pattern )
    for fnmatch_pattern in INCLUDE_FILE_FNMATCH_PATTERNS +\
                           SOURCE_FILE_FNMATCH_PATTERNS
]\
+\
[join( YIELD_SRC_DIR_PATH, "yield", "yield_common_test_main.cpp" )]

INCLUDE_FILE_PATHS["fs"] =\
[
    join( YIELD_INCLUDE_DIR_PATH, "yield", "fs", "file.hpp" ), # before aiocb
    join( YIELD_INCLUDE_DIR_PATH, "yield", "fs", "path.hpp" ), # before file_log
    join( YIELD_INCLUDE_DIR_PATH, "yield", "fs", "stat.hpp" ), # before directory
]+\
INCLUDE_FILE_PATHS["fs"]

INCLUDE_FILE_PATHS["i18n"] =\
[
    join( YIELD_INCLUDE_DIR_PATH, "yield", "i18n", "posix", "code.hpp" ),
    join( YIELD_INCLUDE_DIR_PATH, "yield", "i18n", "posix", "iconv.hpp" ),
    join( YIELD_INCLUDE_DIR_PATH, "yield", "i18n", "posix", "tstring.hpp" ),
    join( YIELD_INCLUDE_DIR_PATH, "yield", "i18n", "win32", "code.hpp" ),
    join( YIELD_INCLUDE_DIR_PATH, "yield", "i18n", "win32", "iconv.hpp" ),
    join( YIELD_INCLUDE_DIR_PATH, "yield", "i18n", "win32", "tstring.hpp" ),
]+\
INCLUDE_FILE_PATHS["i18n"]

INCLUDE_FILE_PATHS["http"] =\
[
    join( YIELD_INCLUDE_DIR_PATH, "yield", "http", "socket_peer.hpp" ), # before socket_client
    join( YIELD_INCLUDE_DIR_PATH, "yield", "http", "socket_client.hpp" ), # before stream_socket_client
    join( YIELD_INCLUDE_DIR_PATH, "yield", "http", "socket_server.hpp" ), # before stream_socket_server
    join( YIELD_INCLUDE_DIR_PATH, "yield", "http", "stream_socket_peer.hpp" ), # before stream_socket_client
    join( YIELD_INCLUDE_DIR_PATH, "yield", "http", "stream_socket_client.hpp" ), # before http_client
    join( YIELD_INCLUDE_DIR_PATH, "yield", "http", "stream_socket_server.hpp" ), # before http_server
]+\
INCLUDE_FILE_PATHS["http"]

INCLUDE_FILE_PATHS["net"] =\
[
    join( YIELD_INCLUDE_DIR_PATH, "yield", "net", "uri.hpp" ), # before socket_address
    join( YIELD_INCLUDE_DIR_PATH, "yield", "net", "sockets", "socket.hpp" ), # before datagram_socket
]+\
INCLUDE_FILE_PATHS["net"]

INCLUDE_FILE_PATHS["stage"] =\
[
    join( YIELD_INCLUDE_DIR_PATH, "yield", "stage", "event.hpp" ),
    join( YIELD_INCLUDE_DIR_PATH, "yield", "stage", "message.hpp" ),
    join( YIELD_INCLUDE_DIR_PATH, "yield", "stage", "response.hpp" ),
    join( YIELD_INCLUDE_DIR_PATH, "yield", "stage", "stage_scheduler.hpp" ),
]+\
INCLUDE_FILE_PATHS["stage"]

INCLUDE_FILE_PATHS["thread"] =\
[
    join( YIELD_INCLUDE_DIR_PATH, "yield", "thread", "mutex.hpp" ),
    join( YIELD_INCLUDE_DIR_PATH, "yield", "thread", "condition_variable.hpp" ), # before synchronized_queue
    join( YIELD_INCLUDE_DIR_PATH, "yield", "thread", "synchronized_queue.hpp" ), # Before synchronized_event_queue
    join( YIELD_INCLUDE_DIR_PATH, "yield", "thread", "thread.hpp" ),
]+\
INCLUDE_FILE_PATHS["thread"]

SOURCE_FILE_PATHS["fs"] =\
[
    join( YIELD_SRC_DIR_PATH, "yield", "fs", "win32", "stat.hpp" ), # before directory
]+\
SOURCE_FILE_PATHS["fs"]

SOURCE_FILE_PATHS["net"] =\
[
    join( YIELD_SRC_DIR_PATH, "yield", "net", "sockets", "win32", "winsock.hpp" ),
]+\
SOURCE_FILE_PATHS["net"]

SOURCE_FILE_PATHS["thread"] =\
[
    join( YIELD_SRC_DIR_PATH, "yield", "thread", "win32", "mutex.hpp" ), # before condition_variable
    join( YIELD_SRC_DIR_PATH, "yield", "thread", "win32", "lightweight_mutex.hpp" ), # before condition_variable
    join( YIELD_SRC_DIR_PATH, "yield", "thread", "win32", "semaphore.hpp" ), # before condition_variable
]+\
SOURCE_FILE_PATHS["thread"]

assert len( EXCLUDE_FILE_PATHS ) == len( INCLUDE_FILE_PATHS ) == len( SOURCE_FILE_PATHS )

for __project_name in EXCLUDE_FILE_PATHS.iterkeys():
    for file_paths in ( EXCLUDE_FILE_PATHS, INCLUDE_FILE_PATHS, SOURCE_FILE_PATHS ):
        file_paths[__project_name] =\
            deduplist( list( rglob( file_paths[__project_name] ) ) )

THIRD_PARTY_SOURCE_FNMATCH_PATTERNS =\
(
    "charProps.c",
    "genx.*",
    "pyparsing.py",
    "rapidxml.hpp",
    "yajl.*",
)
