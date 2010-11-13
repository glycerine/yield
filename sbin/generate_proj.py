# generate_proj.py

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


assert __name__ == "__main__"


from copy import deepcopy
from os import makedirs
from os.path import abspath, exists, join, split
from optparse import OptionParser

from config import *
from yuild import Makefile, SConscript, VCXProj, VCXProjFilters, VCXProjUser, VCXSln
from yutil import deduplist, mirror_subdirectories, write_file


# Constants
YIELD_PUBLIC_DIR_PATH = join( YIELD_DIR_PATH, "..", "yield_public" )
if exists( YIELD_PUBLIC_DIR_PATH ):
    YIELD_PUBLIC_DIR_PATH = abspath( YIELD_PUBLIC_DIR_PATH )
else:
    YIELD_PUBLIC_DIR_PATH = None


# Parse options
option_parser = OptionParser()
option_parser.add_option( "-f", "--force", action="store_true" )
options, ignore = option_parser.parse_args()


# Generate project files
for project_name in PROJECT_NAMES:
    project =\
    {
       "autoconf": AUTOCONF.get( project_name, None ),
       "build_dir_path": join( YIELD_DIR_PATH, "build", "yield", project_name ),
       "cxxflags": CXXFLAGS,
       "cxxpath": CXXPATH,
       "exclude_file_paths": EXCLUDE_FILE_PATHS[project_name],
       "include_file_paths": INCLUDE_FILE_PATHS[project_name],
       "libpath": LIBPATH,
       "libs": LIBS.get( project_name, {} ),
       "ldflags": LDFLAGS,
       "name": project_name,
       "output_file_path": join( YIELD_DIR_PATH, "lib", "yield_" + project_name ),
       "project_dir_path": join( YIELD_DIR_PATH, "proj", "yield", project_name ),
       "project_references":
            [
                join(
                    YIELD_DIR_PATH,
                    "proj",
                    "yield",
                    project_reference,
                    project_reference
                )
                for project_reference in PROJECT_REFERENCES[project_name]
            ],
       "source_file_paths": SOURCE_FILE_PATHS[project_name],
       "type": "lib",
    }

    project_public = None
    if YIELD_PUBLIC_DIR_PATH is not None:
        hpp_file_path =\
            join( YIELD_PUBLIC_DIR_PATH, "include", "yield", project_name + ".hpp" )

        cpp_file_path =\
            join( YIELD_PUBLIC_DIR_PATH, "src", "yield", project_name + ".cpp" )

        if exists( hpp_file_path ) and exists( cpp_file_path ):
            project_public = project.copy()
            project_public["build_dir_path"] =\
                join( YIELD_PUBLIC_DIR_PATH, "build", "yield", project_name )
            project_public["cxxpath"] = [join( YIELD_PUBLIC_DIR_PATH, "include" )]
            project_public["include_file_paths"] = [hpp_file_path]
            project_public["libpath"] = join( YIELD_PUBLIC_DIR_PATH, "lib" )
            project_public["libs"] = LIBS.get( project_name, {} )
            project_public["output_file_path"] =\
                join( YIELD_PUBLIC_DIR_PATH, "lib", "yield_" + project_name )
            del project_public["project_references"]
            project_public["project_dir_path"] =\
                join( YIELD_PUBLIC_DIR_PATH, "proj", "yield", project_name )
            project_public["source_file_paths"] = [cpp_file_path]

    project_test = project.copy()
    project_test["build_dir_path"] =\
        join( YIELD_DIR_PATH, "build", "yield", project_name + "_test" )
    project_test["exclude_file_paths"] = EXCLUDE_FILE_PATHS[project_name + "_test"]
    project_test["include_file_paths"] = INCLUDE_FILE_PATHS[project_name + "_test"]
    project_test["libs"] = deepcopy( project["libs"] )
    project_test["libs"].setdefault( '*', [] ).insert( 0, "yield_" + project_name )

    project_test["name"] = project_name + "_test"
    project_test["output_file_path"] =\
        join( YIELD_DIR_PATH, "bin", "yield_" + project_name + "_test" )
    project_test["project_references"] =\
        [join( YIELD_DIR_PATH, "proj", "yield", project_name, project_name )]
    project_test["source_file_paths"] =\
         SOURCE_FILE_PATHS[project_name + "_test"]
    project_test["type"] = "exe"

    for project_dict in ( project, project_public, project_test ):
        if project_dict is not None:
            for key in ( "build_dir_path", "project_dir_path" ):
                if not exists( project_dict[key] ):
                    makedirs( project_dict[key] )

            if project_name != "common":
                project_source_dir_path =\
                    join( YIELD_SRC_DIR_PATH, "yield", project_name )

                mirror_subdirectories(
                    project_source_dir_path,
                    project["build_dir_path"]
                )

                mirror_subdirectories(
                    project_source_dir_path,
                    project_test["build_dir_path"]
                )

            for project_class, file_ext in \
                (
                    ( Makefile, ".Makefile" ),
                    # ( SConscript, ".SConscript" ),
                    ( VCXProj, ".vcxproj" ),
                    ( VCXProjFilters, ".vcxproj.filters" ),
                    ( VCXProjUser, ".vcxproj.user" ),
                ):

                write_file(
                    join(
                        project_dict["project_dir_path"],
                        project_dict["name"] + file_ext
                    ),
                    repr( project_class( **project_dict ) ),
                    force=options.force
                )


# Generate Visual Studio solution files
project_references = []
for project_name in PROJECT_NAMES:
    project_references.append( join( "proj", "yield", project_name, project_name ) )
    project_references.append( join( "proj", "yield", project_name, project_name + "_test" ) )
write_file(
    "yield.sln",
    repr( VCXSln( project_references, "yield.sln" ) ),
    newline="\r\n"
)
if YIELD_PUBLIC_DIR_PATH is not None:
    project_references = []
    for project_name in PROJECT_NAMES:
        project_reference = join( YIELD_PUBLIC_DIR_PATH, "proj", "yield", project_name, project_name )
        if exists( project_reference + ".vcxproj" ):
            project_references.append( project_reference )

    write_file(
        join( YIELD_PUBLIC_DIR_PATH, "yield.sln" ),
        repr(
            VCXSln(
                project_references,
                #join( YIELD_PUBLIC_DIR_PATH, "yield.sln" )
            )
        )
    )


# Generate Makefile
clean_actions = []
project_targets = []
project_target_names = []

for project_name in PROJECT_NAMES:
    clean_actions.append( "$(MAKE) -C proj/yield/%(project_name)s -f %(project_name)s.Makefile clean" % locals() )
    clean_actions.append( "$(MAKE) -C proj/yield/%(project_name)s -f %(project_name)s_test.Makefile clean" % locals() )

    project_references = ' '.join(
                             [split( project_reference )[1]
                              for project_reference in PROJECT_REFERENCES[project_name]]
                         )
    project_targets.append( """\
%(project_name)s: %(project_references)s
    $(MAKE) -C proj/yield/%(project_name)s -f %(project_name)s.Makefile

%(project_name)s_test: %(project_name)s
    $(MAKE) -C proj/yield/%(project_name)s -f %(project_name)s_test.Makefile
""" % locals() )
    project_target_names.extend( ( project_name, project_name + "_test" ) )

clean_actions.sort()
clean_actions = '\n\t'.join( clean_actions )

project_targets.sort()
project_targets = '\n'.join( project_targets )

project_target_names.sort()
project_target_names = ' '.join( project_target_names )

Makefile = """\
all: %(project_target_names)s
	
clean:
    %(clean_actions)s

%(project_targets)s
""" % locals()
Makefile = Makefile.replace( "    ", '\t' )

write_file( "Makefile", Makefile, force=options.force )
