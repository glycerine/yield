# yidl/__init__.py

# Copyright (c) 2011 Minor Gordon
# All rights reserved

# This source file is part of the yidl project.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
# * Neither the name of the yidl project nor the
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

from os import getcwd
from os.path import abspath, join


__all__ = \
[
    "format_src",
    "generate_SConscript",
    "generate_vcxproj",
    "generate_vcxproj",
]


def format_src(*args, **kwds):
    from format_src import format_src
    format_src(*args, **kwds)


def generate_SConscript(name, force=False, project_dir_path=None, *args, **kwds):
    from project.sconscript import SConscript
    from utility.file import write_file

    if project_dir_path is None:
        project_dir_path = getcwd()
    project_dir_path = abspath(project_dir_path)

    sconscript = SConscript(project_dir_path=project_dir_path, *args, **kwds)

    write_file(
        join(project_dir_path, name + ".SConscript"),
        repr(sconscript),
        force=force
    )


def generate_vcproj(name, force=False, project_dir_path=None, *args, **kwds):
    from project.vcproj import VCProj
    from utility.file import write_file

    if project_dir_path is None:
        project_dir_path = getcwd()
    project_dir_path = abspath(project_dir_path)

    vcproj = VCProj(name=name, project_dir_path=project_dir_path, *args, **kwds)

    write_file(
        join(project_dir_path, name + ".vcproj"),
        repr(vcproj),
        force=force
    )


def generate_vcxproj(name, force=False, project_dir_path=None, *args, **kwds):
    from project.vcxproj import VCXProj, VCXProjFilters, VCXProjUser
    from utility.file import write_file

    if project_dir_path is None:
        project_dir_path = getcwd()
    project_dir_path = abspath(project_dir_path)

    vcxproj = VCXProj(name=name, project_dir_path=project_dir_path, *args, **kwds)

    write_file(
        join(project_dir_path, name + ".vcxproj"),
        repr(vcxproj),
        force=force
    )

    write_file(
        join(project_dir_path, name + ".vcxproj.filters"),
        repr(VCXProjFilters(vcxproj)),
        force=force
    )

    write_file(
        join(project_dir_path, name + ".vcxproj.user"),
        repr(VCXProjUser()),
        force=force
    )
