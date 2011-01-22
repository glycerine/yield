# yidl/target/target.py

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


import sys
from copy import copy
from inspect import isclass

from yutil import lpad, rpad, static_cast, strlist, typelist


__all__ = \
[
    "BooleanType",
    "BufferType",
    "Constant",
    "Declaration",
    "EnumeratedType", "Enumerator",
    "ExceptionType", "ExceptionTypeMember",
    "Include",
    "Interface",
    "MapType",
    "Module",
    "Name",
    "NumericType",
    "Operation", "OperationParameter",
    "PointerType",
    "ReferenceType",
    "SequenceType",
    "StringType",
    "StructType", "StructTypeMember",
    "Target",
    "VariadicType",
]


# Helper functions
def _get_base_module_names(class_):
    base_module_names = []
    for base in class_.__bases__:
        if base.__module__ != "__builtin__":
            base_module_names.append(base.__module__)
            base_module_names.extend(_get_base_module_names(base))
    return base_module_names


class Construct:
    def __init__(self, parent_construct=None):
        self.__child_constructs = []
        self.__parent_construct = static_cast(parent_construct, (Construct, None))

    def _add_child_construct(self, construct_base_class, *args, **kwds):
        search_module_names = ["__main__", self.__class__.__module__]
        search_module_names.extend(_get_base_module_names(self.__class__))
        parent_construct = self.get_parent_construct()
        while parent_construct is not None:
            search_module_names.append(parent_construct.__class__.__module__)
            parent_construct = parent_construct.get_parent_construct()
        search_module_names = list(set(search_module_names))

        construct_class = None
        construct_class_name = ""
        for module_name in search_module_names:
            module = sys.modules[module_name]
            attrs = [
                attr for attr in dir(module)
                if isinstance(attr, str) and
                attr.endswith(construct_base_class.__name__)
            ]

            for attr in attrs:
                class_ = getattr(module, attr)
                if isclass(class_):
                    if issubclass(class_, construct_base_class):
                        if len(attr) > len(construct_class_name):
                            construct_class = class_
                            construct_class_name = attr

        if construct_class is None:
            construct_class = construct_base_class

        construct = construct_class(self, *args, **kwds)

        self.__child_constructs.append(construct)

        return construct

    def get_child_constructs(self, construct_base_class=None):
        if construct_base_class is None:
            return self.__child_constructs
        else:
            return [construct for construct in self.__child_constructs
                    if isinstance(construct, construct_base_class)]

    def get_parent_construct(self):
        return self.__parent_construct

    def get_parent_target(self):
        parent_construct = self.get_parent_construct()
        while not isinstance(parent_construct, Target):
            parent_construct = parent_construct.get_parent_construct()
            assert isinstance(parent_construct, Construct)
        return parent_construct

    def __repr__(self):
        return "".join([repr(construct) for construct in self.__child_constructs])


class Name(list):
    def __init__(self, name, tag=None):
        if isinstance(name, Name):
            list.__init__(self, name)
            self.__tag = name.__tag
        if isinstance(name, list) or isinstance(name, tuple):
            name = typelist(name, str)
            assert len(name) > 0, name
            list.__init__(self, name)
        elif isinstance(name, str):
            assert len(name) > 0, name
            if "::" in name: name = name.split("::")
            elif '.' in name: name = name.split('.')
            else: name = [name]
            list.__init__(self, name)
        elif isinstance(name, int):
            list.__init__(self, [str(name)])
            self.__tag = name
            return
        else:
            raise TypeError, type(name)

        if tag is not None:
            self.__tag = static_cast(tag, int)
        else:
            self.__tag = hash("::".join(name))

    def __add__(self, other):
        if isinstance(other, list):
            return Name(list.__add__(self, other))
        elif isinstance(other, str):
            return self[-1] + other
        else:
            raise TypeError, type(other)

    def endswith(self, other):
        return self[-1].endswith(other)

    def __eq__(self, other):
        if isinstance(other, Name):
            return list.__eq__(self, other)
        elif isinstance(other, str):
            return "::".join(self) == other
        else:
            return False

    def get_tag(self): return self.__tag
    def __hash__(self): return self.__tag
    def __int__(self): return self.__tag

    def __radd__(self, other):
        if isinstance(other, list):
            return Name(list.__radd__(self, other))
        elif isinstance(other, str):
            return other + self[-1]
        else:
            raise TypeError, type(other)

    def set_tag(self, tag): self.__tag = tag
    def __str__(self): return self[-1]


class NamedConstruct(Construct):
    def __init__(self, parent_construct, name):
        Construct.__init__(self, parent_construct)
        self.__name = static_cast(name, Name)

    def get_name(self): return self.__name



class Type(NamedConstruct): pass


class BooleanType(Type): pass


class BufferType(Type): pass


class Declaration(NamedConstruct):
    def __init__(self, parent_construct, name, type, type_qualifiers=None, value=None):
        NamedConstruct.__init__(self, parent_construct, name)
        self.__type = static_cast(type, Type)
        self.__type_qualifiers = typelist(type_qualifiers, str)
        self.__value = value

    def __getitem__(self, key):
        if key == "identifier": return self.get_identifier()
        else: return NamedConstruct.__getitem__(self, key)

    def get_value(self): return self.__value
    def get_identifier(self): return self.get_name()
    def get_type(self): return self.__type
    def get_type_qualifiers(self): return self.__type_qualifiers


class Constant(Declaration):
    def __init__(self, *args, **kwds):
        Declaration.__init__(self, *args, **kwds)
        assert self.get_value() is not None
        assert "const" in self.get_type_qualifiers()


class EnumeratedType(Type):
    def add_enumerator(self, *args, **kwds):
        return self._add_child_construct(Enumerator, *args, **kwds)

    def get_enumerators(self):
        return self.get_child_constructs(Enumerator)

class Enumerator(Declaration): pass


class ExceptionType(Type):
    def add_member(self, *args, **kwds):
        return self._add_child_construct(ExceptionTypeMember, *args, **kwds)

    def get_members(self):
        return self.get_child_constructs(ExceptionTypeMember)

class ExceptionTypeMember(Declaration): pass


class Include(Construct):
    def __init__(self, parent_target, path, local=True):
        Construct.__init__(self, parent_target)
        self.__local = static_cast(local, bool)
        self.__path = static_cast(path, str)

    def __eq__(self, other):
        return self.__abspath == other.__abspath

    def __hash__(self):
        return hash(self.__repr__())

    def __repr__(self):
        if self.is_local():
            return '"' + self.__path + '"'
        else:
            return '<' + self.__path + '>'

    def get_path(self): return self.__path
    def is_local(self): return self.__local


class Interface(NamedConstruct):
    def __init__(self, parent_module, name, base_names=None, local=False):
        NamedConstruct.__init__(self, parent_module, name)
        self.__base_names = typelist(base_names, Name)
        self.__local = local

    def add_constant(self, *args, **kwds):
        return self._add_child_construct(Constant, *args, **kwds)

    def add_exception_type(self, *args, **kwds):
        return self._add_child_construct(ExceptionType, *args, **kwds)

    def add_operation(self, *args, **kwds):
        return self._add_child_construct(Operation, *args, **kwds)

    def get_base_names(self): return self.__base_names
    def get_constants(self): return self.get_child_constructs(Constant)
    def get_exception_types(self): return self.get_child_constructs(ExceptionType)
    def get_operations(self): return self.get_child_constructs(Operation)
    def is_local(self): return self.__local


class MapType(Type):
    def __init__(self, parent_module, name, key_type, value_type):
        Type.__init__(self, parent_module, name)
        self.__key_type = static_cast(key_type, Type)
        self.__value_type = static_cast(value_type, Type)

    def get_key_type(self): return self.__key_type
    def get_value_type(self): return self.__value_type


class Module(NamedConstruct):
    def __init__(self, parent_construct, name, local=False):
        NamedConstruct.__init__(self, parent_construct, name)
        self.__local = local

    def add_constant(self, *args, **kwds):
        return self._add_child_construct(Constant, *args, **kwds)

    def add_enumerated_type(self, *args, **kwds):
        return self._add_child_construct(EnumeratedType, *args, **kwds)

    def add_interface(self, *args, **kwds):
        return self._add_child_construct(Interface, *args, **kwds)

    def add_map_type(self, *args, **kwds):
        return self._add_child_construct(MapType, *args, **kwds)

    def add_module(self, *args, **kwds):
        return self._add_child_construct(Module, *args, **kwds)

    def add_sequence_type(self, *args, **kwds):
        return self._add_child_construct(SequenceType, *args, **kwds)

    def add_struct_type(self, *args, **kwds):
        return self._add_child_construct(StructType, *args, **kwds)

    def get_constants(self): return self.get_child_constructs(Constant)
    def get_interfaces(self): return self.get_child_constructs(Interface)
    def get_modules(self): return self.get_child_constructs(Module)
    def get_types(self): return self.get_child_constructs(Type)

    def get_type(self, name):
        for type in self.get_types():
            if str(type.get_name()) == str(name):
                return type
        raise KeyError, name

    def is_local(self): return self.__local

    def __repr__(self):
        module_repr = []

        constants = '\n'.join([repr(constant) for constant in self.get_constants()])
        if len(constants) > 0: module_repr.append(constants)

        interfaces = "\n\n".join([repr(interface) for interface in self.get_interfaces()])
        if len(interfaces) > 0: module_repr.append(interfaces)

        modules = "\n\n".join([repr(module) for module in self.get_modules()])
        if len(modules) > 0: module_repr.append(modules)

        types = "\n\n".join([repr(type) for type in self.get_types()])
        if len(types) > 0: module_repr.append(types)

        return "\n\n".join(module_repr)


class NumericType(Type): pass


class Operation(NamedConstruct):
    def __init__(self, parent_interface, name, modifiers=None, return_type=None):
        NamedConstruct.__init__(self, parent_interface, name)
        self.__modifiers = typelist(modifiers, str)
        self.__return_type = static_cast(return_type, (Type, None))

    def add_parameter(self, *args, **kwds):
        return self._add_child_construct(OperationParameter, *args, **kwds)

    def get_in_parameters(self):
        return [parameter for parameter in self.get_parameters()
                if parameter.is_in()]

    def get_inout_parameters(self):
        return [parameter for parameter in self.get_parameters()
                if parameter.is_in() and parameter.is_out()]

    def get_modifiers(self):
        return self.__modifiers

    def get_out_parameters(self, include_return_value=None):
        return [parameter for parameter in self.get_parameters()
                if parameter.is_out()]

    def get_parameters(self): return self.get_child_constructs(OperationParameter)
    def get_return_type(self): return self.__return_type
    def is_const(self): return "const" in self.get_modifiers()
    def is_oneway(self): return "oneway" in self.get_modifiers()
    def is_static(self): return "static" in self.get_modifiers()

class OperationParameter(Declaration):
    def __init__(self, *args, **kwds):
        Declaration.__init__(self, *args, **kwds)
        if self.get_parent_operation().is_oneway():
            assert not self.is_out()

    def get_parent_operation(self): return self.get_parent_construct()
    def is_in(self): return "in" in self.get_type_qualifiers()
    def is_out(self): return "out" in self.get_type_qualifiers()


class PointerType(Type): pass


class ReferenceType(Type): pass


class SequenceType(Type):
    def __init__(self, parent_module, name, value_type):
        Type.__init__(self, parent_module, name)
        self.__value_type = static_cast(value_type, Type)

    def get_value_type(self): return self.__value_type


class StringType(Type): pass


class StructType(Type):
    def __init__(self, parent_construct, name, base_names=None, members=[]):
        Type.__init__(self, parent_construct, name)
        self.__base_names = typelist(base_names, Name)
        if members is None: # A forward declaration or an undefined type
            self.__members = None
        else:
            self.__members = typelist(members, StructTypeMember)

    def add_member(self, *args, **kwds):
        return self._add_child_construct(StructTypeMember, *args, **kwds)

    def get_base_names(self): return self.__base_names
    def get_members(self): return self.get_child_constructs(StructTypeMember)
    def is_defined(self): return self.get_members() is not None

class StructTypeMember(Declaration): pass


class Target(Construct):
    def add_builtin_type(self, name):
        name = static_cast(name, Name)
        if name == "any" or name.endswith('*'):
            return self._add_child_construct(PointerType, name)
        elif name.endswith('&'):
            return self._add_child_construct(ReferenceType, name)
        elif name == "boolean":
            return self._add_child_construct(BooleanType, name)
        elif name == "buffer":
            return self._add_child_construct(BufferType, name)
        elif name == "string":
            return self._add_child_construct(StringType, name)
        else:
            return self._add_child_construct(NumericType, name)

    def add_include(self, *args, **kwds):
        return self._add_child_construct(Include, *args, **kwds)

    def add_module(self, *args, **kwds):
        return self._add_child_construct(Module, *args, **kwds)

    def finalize(self): pass

    def get_builtin_types(self): return self.get_child_constructs(Type)
    def get_includes(self): return self.get_child_constructs(Include)
    def get_modules(self): return self.get_child_constructs(Module)


class VariadicType(Type): pass
