# yidl/target/c_target.py

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


from copy import copy

from yutil import pad, rpad

from yidl.target import *


__all__ = \
[
    "CBooleanType",
    "CBufferType",
    "CConstant",
    "CEnumeratedType", "CEnumerator",
    "CExceptionType",
    "CInclude",
    "CInterface",
    "CMapType",
    "CModule",
    "CNumericType",
    "COperation", "COperationParameter",
    "CPointerType",
    "CReferenceType",
    "CSequenceType",
    "CStringType",
    "CStructType",
    "CTarget",
    "CVariadicType",
]


class CConstruct:
    def get_includes(self):
        includes = []
        for child_construct in self.get_child_constructs():
            includes.extend(child_construct.get_includes())
        return includes


class CType(CConstruct):
    def get_constant(self, identifier, value):
        name = self.get_name()
        return "const static %(name)s %(identifier)s = %(value)s;\n" % locals()


class CBooleanType(BooleanType, CType): pass


class CBufferType(BufferType, CType): pass


class CConstant(Constant, CConstruct):
    def __repr__(self):
        return self.get_type().get_constant(self.get_identifier(), self.get_value())


class CEnumeratedType(EnumeratedType, CType):
    def __repr__(self):
        return "enum " + self.get_name() + " {" + pad(" ", ", ".join([repr(enumerator) for enumerator in self.get_enumerators()]), " ") + "};"

class CEnumerator(Enumerator):
    def __repr__(self):
        if self.get_value() is not None:
            return self.get_identifier() + " = " + str(self.get_value())
        else:
            return self.get_identifier()


class CExceptionType(ExceptionType, CType): pass


class CInclude(Include, CConstruct):
    def __repr__(self):
        return "#include " + Include.__repr__(self).replace(".idl", ".h")


class CInterface(Interface, CConstruct): pass


class CMapType(MapType, CType): pass


class CModule(Module, CConstruct): pass


class CNumericType(NumericType, CType):
    def get_default_value(self):
        return "0"

    def get_dummy_value(self):
        return "0"

    def get_includes(self):
        return ["<stdint.h>"]


class COperation(Operation, CConstruct): pass
class COperationParameter(OperationParameter, CConstruct): pass


class CPointerType(PointerType, CType):
    def get_default_value(self):
        return "NULL"

    def get_dummy_value(self):
        return "NULL"


class CReferenceType(ReferenceType, CType):
    def get_dummy_value(self):
        return "NULL"


class CSequenceType(SequenceType, CType): pass


class CStringType(StringType, CType):
    def get_constant(self, identifier, value):
        return "const static char* %(identifier)s = \"%(value)s\";\n" % locals()

    def get_static_constant_name(self):
        return "char*"


class CStructType(StructType, CType): pass
class CStructTypeMember(StructTypeMember, CConstruct): pass


class CTarget(Target, CConstruct):
    def __repr__(self):
        includes = \
            rpad(
                "\n".join([repr(include) for include in self.get_includes()]),
                "\n\n\n"
            )

        modules = "\n\n".join([repr(module) for module in self.get_modules()])

        guard = "_" + str(abs(hash(modules))) + "_H_"

        return """\
#ifndef %(guard)s
#define %(guard)s


%(includes)s


%(modules)s

#endif

""" % locals()

    def get_includes(self):
        local_includes_dict = {}
        nonlocal_includes_dict = {}
        for include in Target.get_includes(self) + CConstruct.get_includes(self):
            if isinstance(include, CInclude):
                pass
            elif isinstance(include, str):
                if include[0] == '<' and include[-1] == '>':
                    include = CInclude(self, include[1:-1], local=False)
                else:
                    include = CInclude(self, include)
            else:
                raise TypeError, type(include)

            if include.is_local():
                local_includes_dict[include.get_path()] = include
            else:
                nonlocal_includes_dict[include.get_path()] = include

        includes = []

        for includes_dict in (local_includes_dict, nonlocal_includes_dict):
            include_paths = includes_dict.keys()
            include_paths.sort()
            for include_path in include_paths:
                includes.append(includes_dict[include_path])

        return includes


class CVariadicType(VariadicType, CType): pass
