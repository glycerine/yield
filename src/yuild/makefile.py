# yuild/sconscript.py

# Copyright (c) 2010 Minor Gordon
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

from os.path import join, split, splitext

from yutil import deduplist, indent, posixpath, posixpaths, pad, relpath, rpad

from yuild.constant import C_CXX_SOURCE_FILE_FNMATCH_PATTERNS,\
                           INDENT_SPACES
from yuild.project import Project


__all__ = ["Makefile"]


# Constants
INDENT_SPACES = INDENT_SPACES["py"]

UNAME_CHECKS = \
{
    "darwin": "ifeq ($(UNAME), Darwin)",
    "freebsd": "ifeq ($(UNAME), FreeBSD)",
    "linux": "ifeq ($(UNAME), Linux)",
    "sunos": "ifeq ($(UNAME), Solaris)",
    "win32": "ifeq ($(UNAME), MINGW32)",
}

SOURCE_FILE_RULE = """\
%(object_file_path)s: %(source_file_path)s
    $(CXX) -c -o %(object_file_path)s -MD $(CXXFLAGS) %(source_file_path)s
"""


# Helper functions
def expand_platform( platform ):
    if platform == "bsd":
        platforms = ( "darwin", "freebsd", )    
    elif platform == "posix" or platform == "unix":
        platforms = ( "darwin", "freebsd", "linux", "sunos" )
    else:
        platforms = ( platform, )
    return platforms

def uname_check( platform, statements ):
    if platform == '*':
        return statements
    else:
        statements = indent( INDENT_SPACES, statements )
        conditional_statements = []
        for platform in expand_platform( platform ):
            uname_check = UNAME_CHECKS[platform]
            conditional_statements.append( """\
%(uname_check)s
%(statements)s
endif""" % locals() )
        return '\n'.join( list( set( conditional_statements ) ) )


class Makefile(Project):    
    def __repr__( self ):
        my_dir_path = "" # "$(dir $(lastword $(MAKEFILE_LIST)))"

        assert len( self.get_build_dir_path() ) == 1 and self.get_build_dir_path().keys()[0] == '*'
        build_dir_path = posixpath( self.get_build_dir_path()['*'] )

        # CXXFLAGS
        platform_cxxflags = {}
        for platform, cxxflags in self.get_cxxflags().iteritems():
            platform_cxxflags.setdefault( platform, [] ).extend( cxxflags )
        for platform, cxxdefines in self.get_cxxdefines().iteritems():
            platform_cxxflags.setdefault( platform, [] ).extend( ["-D" + cxxdefine for cxxdefine in cxxdefines] )
        for platform, cxxpath in self.get_cxxpath().iteritems():
            platform_cxxflags.setdefault( platform, [] ).extend( ["-I" + posixpath( cxxpath ) for cxxpath in cxxpath] )
        CXXFLAGS = []
        platforms = list( platform_cxxflags.keys() ); platforms.sort()
        for platform in platforms:
            CXXFLAGS.append( uname_check( platform, "CXXFLAGS += " + ' '.join( platform_cxxflags[platform] ) ) )
        CXXFLAGS.sort()
        CXXFLAGS = pad( "\n\n", '\n'.join( CXXFLAGS ), "\n" )

        # LDFLAGS
        platform_ldflags = {}
        for platform, ldflags in self.get_ldflags().iteritems():
            platform_ldflags.setdefault( platform, [] ).extend( ldflags )
        for platform, libpath in self.get_libpath().iteritems():
            platform_ldflags.setdefault( platform, [] ).extend( ["-L" + posixpath( libpath ) for libpath in libpath] )
        LDFLAGS = []
        platforms = list( platform_ldflags.keys() ); platforms.sort()
        for platform in platforms:
            LDFLAGS.append( uname_check( platform, "LDFLAGS += " + ' '.join( platform_ldflags[platform] ) ) )
        LDFLAGS.sort()
        LDFLAGS = pad( "\n\n", '\n'.join( LDFLAGS ), "\n" )

        # LIBS
        LIBS = []
        for platform, libs in self.get_libs().iteritems():
            LIBS.append( uname_check( platform, "LIBS += " + ' '.join( ["-l" + lib for lib in libs] ) ) )
        LIBS = pad( "\n\n", '\n'.join( LIBS ), "\n" )

        # source_files
        platform_object_file_paths = {}
        source_file_rules = []
        source_files = self.get_source_files()
        source_files = source_files.exclude( self.get_exclude_files() )
        source_files = source_files.filter( C_CXX_SOURCE_FILE_FNMATCH_PATTERNS )
        source_files_platform_dict = source_files.as_platform_dict()

        for platform, source_files in source_files_platform_dict.iteritems():
            assert len( source_files ) > 0

            for platform in expand_platform( platform ):
                object_file_paths = []

                for source_file in source_files:
                    source_file_path = my_dir_path + source_file

                    object_file_path =\
                        my_dir_path +\
                        join(
                            self.get_build_dir_path()[platform],
                            splitext(
                                relpath( source_file_path, self.get_root_source_dir_path() )
                            )[0] + ".o"
                        )
                    object_file_path = posixpath( object_file_path )
                    object_file_paths.append( object_file_path )

                    source_file_path = posixpath( source_file_path )
                    source_file_rules.append( SOURCE_FILE_RULE % locals() )
          
                platform_object_file_paths.setdefault( platform, [] ).append(
                    "OBJECT_FILE_PATHS += " +\
                    ' '.join( object_file_paths )
                )

        object_file_paths = []
        platforms = list( platform_object_file_paths.keys() ); platforms.sort()
        for platform in platforms:
            platform_object_file_paths[platform].sort()
            object_file_paths.append(
                uname_check(
                    platform,
                    '\n'.join( platform_object_file_paths[platform] )
                )
            )
        object_file_paths.sort()
        object_file_paths = '\n'.join( object_file_paths )

        source_file_rules = deduplist( source_file_rules )
        source_file_rules.sort()
        source_file_rules = '\n'.join( source_file_rules )

        # project_references
        project_references = []
        # for project_reference in self.get_project_references().get( '*', default=[] ):
        #     Makefile = posixpath( project_reference + ".Makefile" )
        #    project_references.append( 'include %(Makefile)s' % locals() )
        project_references = rpad( '\n'.join( project_references ), "\n\n\n" )

        # output_file_path
        assert len( self.get_output_file_path() ) == 1 and\
               self.get_output_file_path().keys()[0] == '*'
        output_file_path = my_dir_path + posixpath( self.get_output_file_path()['*'] )
        if self.get_type() == "exe":
            output_file_path_action = "$(LINK.cpp) $(OBJECT_FILE_PATHS) -o $@ $(LIBS)"
        elif self.get_type() == "lib":
            output_dir_path, output_file_name = split( output_file_path )
            if not output_file_name.startswith( "lib" ):
                output_file_name = "lib" + output_file_name
            if not output_file_name.endswith( ".a" ):
                output_file_name += ".a"
            output_file_path = posixpath( join( output_dir_path, output_file_name ) )
            output_file_path_action = "$(AR) -r $@ $(OBJECT_FILE_PATHS)"
        else:
            raise NotImplementedError, self.get_type()

        return ( """\
# SHELL = /bin/bash
UNAME := $(shell uname)%(CXXFLAGS)s%(LDFLAGS)s%(LIBS)s

DEP_FILE_PATHS := $(shell find %(my_dir_path)s%(build_dir_path)s -name "*.d")


%(object_file_paths)s


%(project_references)s%(output_file_path)s: $(OBJECT_FILE_PATHS)
    %(output_file_path_action)s

clean:
    $(RM) %(output_file_path)s $(OBJECT_FILE_PATHS)

depclean:
    $(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


%(source_file_rules)s

""" % locals() ).replace( "    ", '\t' )
        
