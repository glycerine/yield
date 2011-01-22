# yidl/compiler/compiler.py

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
from os import sep as os_sep
from os.path import abspath, dirname, join

from grammar import BUILTIN_TYPE_NAMES, NUMERIC_TYPE_NAMES, grammar
from pyparsing import ParseException
from yidl.target import Interface, Module, Name, Target


__all__ = ["compile"]


# Constants
DEBUG = False # True

IDL_INT_TYPE_TO_STDINT_TYPE_MAP = \
{
  "char" : "int8_t",
  "signed char": "int8_t",
  "unsigned char": "uint8_t",
  "octet": "uint8_t",

  "short" : "int16_t",
  "signed short": "int16_t",
  "unsigned short": "uint16_t",

  "int" : "int32_t",
  "signed int": "int32_t",
  "unsigned int": "uint32_t",

  "long" : "int64_t",
  "signed long": "int64_t",
  "unsigned long": "uint64_t",

  "long long": "int64_t",
  "signed long long": "int64_t",
  "unsigned long long": "uint64_t"
}


# Helper functions
def flatten_tokens(tokens):
    flattened_tokens = []
    for token in tokens:
        if hasattr(token, "asList"):
            flattened_tokens.extend(flatten_tokens(token.asList()))
        elif isinstance(token, list):
            flattened_tokens.extend(flatten_tokens(token))
        else:
            flattened_tokens.append(token)
    return flattened_tokens


# The main entry point
def compile(idl, target, use_stdint_types=True):
    assert isinstance(idl, basestring)
    assert isinstance(target, Target)

    try:
        idl_str = open(idl).read()
        idl_dir_path = dirname(abspath(idl))
    except IOError:
        idl_str = idl
        idl_dir_path = dirname(abspath(sys.modules[__name__].__file__))

    return Compiler(use_stdint_types)(idl_dir_path, idl_str, target)


class Compiler:
    def __init__(self, use_stdint_types=True):
        self.__next_tag = 1
        self.__token_i = 0
        self.__scope_stack = []
        self.__use_stdint_types = use_stdint_types

    def __call__(self, idl_dir_path, idl_str, target):
        if len(idl_str.strip()) > 0:
            self.__tokens = flatten_tokens(grammar.parseString(idl_str))
            if len(self.__tokens) == 0: raise ValueError


            while self.have_next_token() and self.peek_next_token() == "#include":
                self.read_next_token("#include")
                local = self.read_next_token(('<', '"'))
                path = self.read_next_token()
                self.read_next_token(local)
                if DEBUG:
                    print "yidl.compiler: parsed #include %(local)s" + path + local
                include = target.add_include(path, local == '"')
#                if path.endswith( ".idl" ):
#                    include_target = parse( join( idl_dir_path, path.replace( '/', os_sep ) ), target.__class__() )
#                    type_map = merge_dicts( include_target.(), type_map )
#            if len( type_map ) == 0:
#                type_map = { None: {} }
#            self.__type_map = type_map

            self.__scope_stack.append(target)

            while self.have_next_token() and \
                  self.peek_next_token() in ("module", "local"):
                self.parse_module()

        target.finalize()

        return target

    def get_current_scope(self):
        return self.__scope_stack[-1]

    def get_current_module(self):
        module = self.get_current_scope()
        assert isinstance(module, Module), str(module.get_name())
        return module

    def get_current_interface(self):
        interface = self.get_current_scope()
        assert isinstance(interface, Interface), str(interface.get_name())
        return interface

    def get_type(self, name):
        for scope_i in xrange(len(self.__scope_stack) - 1, 0, -1):
            scope = self.__scope_stack[scope_i]
            try:
                return scope.get_type(name)
            except (AttributeError, KeyError):
                pass

        if name in BUILTIN_TYPE_NAMES:
            if name in NUMERIC_TYPE_NAMES and self.__use_stdint_types:
                try:
                    new_name = Name(IDL_INT_TYPE_TO_STDINT_TYPE_MAP[name])
                    if DEBUG:
                        print "yidl.compiler: substituting", new_name, "stdint type for", name
                    name = new_name
                except KeyError:
                    pass

            assert isinstance(self.__scope_stack[0], Target)
            return self.__scope_stack[0].add_builtin_type(name)
        else: # Assume it's a struct
#            if DEBUG:
            print "yidl.compiler: assuming", name, "is a struct"
            return self.get_current_module().add_struct_type(name, members=None)

    def have_next_token(self):
        return self.__token_i < len(self.__tokens)

    def parse_constant(self):
        self.read_next_token("const")
        identifier, type, type_qualifiers = self.parse_declaration()
        assert "const" not in type_qualifiers
        identifier = self.get_current_scope().get_name() + Name([identifier])
        self.read_next_token('=')
        value = self.read_next_token()
        self.read_next_token(';')
        if DEBUG:
            print "yidl.compiler: parsed constant", identifier, "=", value
        return self.get_current_scope().add_constant(
                   identifier,
                   type,
                   ["const"],
                   value
                )

    def parse_declaration(self):
        type_qualifiers = []
        while self.peek_next_token() in ("const", "in", "inout", "out"):
            type_qualifiers.append(self.read_next_token())

        type_name = self.parse_type_name()
        type = self.get_type(type_name)

        identifier = Name(self.read_next_token())

        return identifier, type, type_qualifiers

    def parse_enumerated_type(self):
        self.read_next_token("enum")
        name = self.parse_name()
        self.read_next_token('{')
        enumerated_type = self.get_current_scope(Module).add_enumerated_type(name)
        while self.peek_next_token() != '}':
            identifier = self.read_next_token()
            if self.peek_next_token() == '=':
                self.__skip_next_token()
                value = self.read_next_token()
            else:
                value = None
            enumerated_type.add_enumerator(identifier, value)
            if self.peek_next_token() != '}':
                self.read_next_token(',')
        self.read_next_token('}')
        self.__skip_next_token(';')
        return enumerated_type

    def parse_exception_type(self):
        self.read_next_token("exception")
        name = self.parse_name()
        exception_type = self.get_current_interface().add_exception_type(name)
        if self.peek_next_token() == ';':
            self.__skip_next_token()
        else:
            self.read_next_token('{')
            while self.peek_next_token() != "}":
                exception_type.add_member(*self.parse_declaration())
                self.read_next_token(';')
            self.read_next_token('}')
            self.__skip_next_token(';')

        return exception_type

    def parse_interface(self):
        if self.peek_next_token() == "local":
            self.__skip_next_token()
            local = True
        else:
            local = False

        self.read_next_token("interface")

        name = self.parse_name()
        base_names = self.parse_base_names()
        if DEBUG:
            print "yidl.compiler: parsing interface", name

        self.read_next_token('{')

        interface = self.get_current_module().add_interface(name, base_names, local)

        self.__scope_stack.append(interface)

        next_token = self.peek_next_token()
        while next_token != '}':
            if next_token == "const":
                self.parse_constant()
            elif next_token == "exception":
                self.parse_exception_type()
            else:
                self.parse_operation()

            next_token = self.peek_next_token()

        self.read_next_token('}')
        self.__skip_next_token(';')

        self.__scope_stack.pop()

        return interface

    def parse_module(self):
        if self.peek_next_token() == "local":
            self.__skip_next_token()
            local = True
        else:
            local = False

        self.read_next_token("module")
        name = self.parse_name()
        if DEBUG:
            print "yidl.compiler: parsing module", name

        self.read_next_token('{')

        module = self.get_current_scope().add_module(name, local)

        self.__scope_stack.append(module)

        token = self.peek_next_token()
        while token != '}': # Within a module
            if token == "const":
                self.parse_constant()
            elif token == "enum":
                self.parse_enumerated_type()
            elif token == "local" or token == "interface":
                self.parse_interface()
            elif token == "module":
                self.parse_module()
            elif token == "struct":
                self.parse_struct_type()
            elif token == "typedef":
                 self.parse_typedef()
            else:
                raise ValueError, token

            token = self.peek_next_token()

        self.read_next_token('}')
        self.__skip_next_token(';')

        self.__scope_stack.pop()

        return module

    def parse_name(self):
        name = Name(self.read_next_token())
        tag = self.parse_tag()
        if not isinstance(self.get_current_scope(), Target):
            name = self.get_current_scope().get_name() + name
            tag = self.get_current_scope().get_name().get_tag() + tag
        name.set_tag(tag)
        return name

    def parse_operation(self):
        modifiers = []
        while self.peek_next_token() in ("oneway", "static"):
            modifiers.append(self.read_next_token())

        return_type_name = self.parse_type_name()
        if return_type_name == "void":
            return_type = None
        else:
            return_type = self.get_type(return_type_name)

        name = self.parse_name()

        operation = self.get_current_interface().add_operation(name, modifiers, return_type)

        self.read_next_token('(')
        while self.peek_next_token() != ')':
            operation.add_parameter(*self.parse_declaration())
            self.__skip_next_token(',')
        self.read_next_token(')')
        self.read_next_token(';')

        return operation

    def parse_base_names(self):
        base_names = None
        if self.peek_next_token() == ':':
            self.__skip_next_token()
            while self.peek_next_token() != '{':
                if self.peek_next_token() != ',':
                    base_name = self.parse_name()
                    try: base_names.append(base_name)
                    except AttributeError: base_names = [base_name]
        return base_names

    def parse_struct_type(self):
        self.read_next_token("struct")
        name = self.parse_name()
        if self.peek_next_token() == ';': # A forward declaration
            self.__skip_next_token()
            struct_type = self.get_current_module().add_struct_type(name, members=None)
        else:
            base_names = self.parse_base_names()
            struct_type = \
                self.get_current_module().add_struct_type(name, base_names=base_names)
            self.read_next_token('{')
            while self.peek_next_token() != "}":
                struct_type.add_member(*self.parse_declaration())
                self.read_next_token(';')
            self.read_next_token('}')
            self.__skip_next_token(';')
        return struct_type

    def parse_tag(self):
        if self.peek_next_token() == '#':
            self.__skip_next_token()
            return int(self.read_next_token())
        else:
            tag = self.__next_tag
            self.__next_tag += 1
            return tag

    def parse_typedef(self):
        self.read_next_token("typedef")
        original_type_name = self.read_next_token(("sequence", "map"))
        self.read_next_token('<')

        if original_type_name == "sequence":
            value_type = self.get_type(self.parse_type_name())
            self.read_next_token('>')
            type = self.get_current_module().add_sequence_type(self.parse_name(), value_type)
        elif original_type_name == "map":
            key_type = self.get_type(self.parse_type_name())
            self.read_next_token(',')
            value_type = self.get_type(self.parse_type_name())
            self.read_next_token('>')
            type = self.get_current_module().add_map_type(self.parse_name(), key_type, value_type)
        else:
            raise AssertionError, original_type_name

        self.read_next_token(';')

        return type

    def parse_type_name(self):
        type_name = []
        if self.peek_next_token() in ("signed", "unsigned"):
            type_name.append(self.read_next_token())

        type_name.append(self.read_next_token())

        if self.peek_next_token() == '*':
            while self.peek_next_token() == '*':
                type_name.append(self.read_next_token())
        elif self.peek_next_token() == '&':
            type_name.append(self.read_next_token())

        if DEBUG:
            print "yidl.compiler: parsed type name", type_name

        return Name(type_name)

    def peek_next_token(self):
        if self.have_next_token():
            if DEBUG:
                print "yidl.compiler: peeked next token '" + \
                      self.__tokens[self.__token_i] + "'"
            return self.__tokens[self.__token_i]
        else:
            raise ParseException, "ran out of tokens"

    def read_next_token(self, expected_token=None):
        if self.have_next_token():
            next_token = self.__tokens[self.__token_i]
            self.__token_i += 1

            if expected_token is None:
                if DEBUG:
                    print "yidl.compiler: read next token '" + next_token + "'"
                return next_token
            else:
                if isinstance(expected_token, basestring):
                    if next_token == expected_token:
                        if DEBUG:
                            print "yidl.compiler: read next token '" + next_token + "'"
                        return next_token
                elif isinstance(expected_token, list) or isinstance(expected_token, tuple):
                    if next_token in expected_token:
                        if DEBUG:
                            print "yidl.compiler: read next token '" + next_token + "'"
                        return next_token
                else:
                    raise NotImplementedError, str(type(expected_token))

                context_tokens = []

                if self.__token_i >= 5:
                    context_tokens.extend(self.__tokens[self.__token_i - 5:self.__token_i])
                else:
                    context_tokens.extend(self.__tokens[0:self.__token_i])

                if self.__token_i + 5 < len(self.__tokens):
                    context_tokens.extend(self.__tokens[self.__token_i:self.__token_i + 5])
                else:
                    context_tokens.extend(self.__tokens[self.__token_i:])

                raise ParseException, "expected token '" + str(expected_token) + "'"\
                                      " but got " + str(next_token) + \
                                       ", surrounding tokens = '" + \
                                       " ".join(context_tokens) + "'"
        else:
            raise ParseException, "ran out of tokens"

    def __skip_next_token(self, expected_token=None):
        if self.have_next_token():
            if expected_token is None or self.__tokens[self.__token_i] == expected_token:
                if DEBUG:
                    print "yidl.compiler: skipping next token '" + \
                          self.__tokens[self.__token_i] + "'"
                self.__token_i += 1
