# yuild/platform_dict.py

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

from copy import copy

from yutil import static_cast

from yuild.constant import ABSTRACT_PLATFORMS, \
                           CONCRETE_PLATFORMS, \
                           PLATFORMS, \
                           PLATFORM_TREE


__all__ = ["PlatformDict"]


class PlatformDict(dict):
    def __init__(self, *args):
        dict.__init__(self)

        if len(args) == 0:
            return
        elif len(args) == 1:
            if args[0] is None:
                return
            elif isinstance(args[0], dict):
                for platform, value in args[0].iteritems():
                    platform = static_cast(platform, str)
                    assert platform in PLATFORMS, platform

                    if isinstance(value, tuple):
                        value = list(value)

                    dict.__setitem__(self, platform, value)
            else:
                value = args[0]
                if isinstance(value, tuple):
                    value = list(value)
                dict.__setitem__(self, '*', value)
        else:
            dict.__setitem__(self, '*', list(args))

    def __getitem__(self, platform):
        value = self.get(platform)
        if value is not None:
            return value
        else:
            value = self.get(platform, combine_platforms=True)
            if value is not None:
                return value
            else:
                raise KeyError, platform

    def get(self, platform, default=None, combine_platforms=False):
        if static_cast(platform, str) in PLATFORMS:
            value = dict.get(self, platform)

            if value is not None:
                if combine_platforms and \
                   isinstance(value, list) and \
                   platform != '*':
                    value = copy(value)
                else:
                    return value
            elif len(self) > 0 and combine_platforms:
                if isinstance(self.values()[0], list):
                    value = []
                else:
                    abstract_platform = platform
                    while True:
                        abstract_platform = PLATFORM_TREE.get(abstract_platform)
                        if abstract_platform is not None:
                            value = dict.get(self, abstract_platform)
                            if value is not None:
                                return value
                        else:
                            break
                    return default
            else:
                return default

            assert platform != '*'
            assert combine_platforms
            assert isinstance(value, list)

            abstract_platform = platform
            while True:
                abstract_platform = PLATFORM_TREE.get(abstract_platform)
                if abstract_platform is not None:
                    value.extend(dict.get(self, abstract_platform, []))
                else:
                    break

            return value
        else:
            return default

    def iteritems(self, combine_platforms=False):
        platforms = self.keys()
        for known_platform in CONCRETE_PLATFORMS + ABSTRACT_PLATFORMS:
            if known_platform in platforms:
                value = self.get(known_platform, combine_platforms=combine_platforms)
                if value is not None:
                    yield known_platform, value

    def keys(self):
        platforms = copy(dict.keys(self))
        platforms.sort()
        return platforms

    def merge(self, other):
        if not isinstance(other, PlatformDict):
            other = PlatformDict(other)
        self.update(merge_dicts(self, other))
        return self

    def __setitem__(self, platform, value):
        platform = static_cast(platform, str)
        assert platform in PLATFORMS, platform

        if isinstance(value, tuple):
            value = list(value)

        for other_value in self.itervalues():
            if type(value) != type(other_value):
                raise TypeError, str(value) + " vs. " + str(other_value)

        dict.__setitem__(self, platform, value)
