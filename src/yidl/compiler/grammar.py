# yidl/compiler/grammar.py

# Copyright (c) 2010 Minor Gordon
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


from pyparsing import Literal, CaselessLiteral, Keyword, Word, ZeroOrMore, OneOrMore, \
        Forward, Group, Optional, Combine, \
        alphas, nums, alphanums, printables, QuotedString, restOfLine, cStyleComment


__all__ = ["BUILTIN_TYPE_NAMES", "NUMERIC_TYPE_NAMES", "grammar"]


# Constants
NUMERIC_TYPE_NAMES = \
(
    "char", "unsigned char", "octet", "int8_t", "uint8_t",
    "short", "unsigned short", "int16_t", "uint16_t",
    "int", "unsigned int", "int32_t", "uint32_t",
    "long", "unsigned long",
    "long long", "unsigned long long", "int64_t", "uint64_t",
    "float", "double",
    "off_t", "off64_t", "size_t", "ssize_t", "mode_t"
 )

BUILTIN_TYPE_NAMES = ("any", "boolean", "buffer", "string", "wstring") + \
                     NUMERIC_TYPE_NAMES


type_name = Word(alphas + "_", alphanums + "_:")
for builtin_type_name in BUILTIN_TYPE_NAMES:
    type_name ^= builtin_type_name
type_name ^= Literal("...")
type_name += Optional(OneOrMore(Literal("*")) | Literal("&"))
parent_type_names = Optional(':' + type_name + ZeroOrMore(',' + type_name))

identifier = Word(alphas + "_", alphanums + "_")

real = Combine\
       (
           Word(nums + "+-", nums) + \
           '.' + \
           Optional(Word(nums)) + \
           Optional(CaselessLiteral("E") + \
           Word(nums + "+-", nums))
       )

integer = Combine\
          (
              CaselessLiteral("0x") + \
              Word(nums + "abcdefABCDEF")
          )\
          | \
          Word(nums + "+-", nums)

value = real | integer | QuotedString("\"", "\\") | QuotedString("'", "\\")

tag = Optional("#" + integer)


constant = Keyword("const") + type_name + identifier + '=' + value + ';'

declaration = Optional(Keyword("const")) + \
              Optional(Keyword("struct")) + \
              type_name + identifier

enum = Keyword("enum") + identifier + '{' + identifier + Optional('=' + value) + \
           ZeroOrMore(',' + identifier + Optional('=' + value)) + \
       '}' + Optional(';')

exception = Keyword("exception") + identifier + tag + \
            (
                ';'
                |
                Group('{' + ZeroOrMore(declaration + ';') + '}' + Optional(';'))
            )

map = Keyword("map") + '<' + type_name + ',' + type_name + '>'

sequence = Keyword("sequence") + '<' + type_name + '>'

struct = Keyword("struct") + identifier + tag + \
         (
             ';'
             |
             Group
             (
                 parent_type_names + '{' + \
                     ZeroOrMore(declaration + ';') +
                 '}' + Optional(';')
             )
         )

typedef = Keyword("typedef") + (sequence | map) + identifier + tag + ';'

operation_parameter = \
    Optional(Keyword("in") | Keyword("out") | Keyword("inout")) + \
    declaration
operation = Optional(Keyword("oneway") | Keyword("static")) + \
            (Keyword("void") | type_name) + \
            identifier + tag + \
            '(' + \
                Optional(operation_parameter + ZeroOrMore(',' + operation_parameter)) + \
            ')' + \
            Literal(';')

interface = Optional(Keyword("local")) + Keyword("interface") + \
            identifier + tag + parent_type_names + \
            '{' + \
                ZeroOrMore(constant | exception | operation) + \
            '}' + Optional(';')

module = Forward()
module << (
              Optional(Keyword("local")) + \
              Keyword("module") + identifier + tag + \
              '{' + \
                  ZeroOrMore(constant | enum | interface | module | struct | typedef) + \
              '}' + Optional(';')
          )

include_file_path = Word(alphanums + "_-./")
include = Keyword("#include") + \
              (
                  Group('<' + include_file_path + '>')
                  |
                  Group('"' + include_file_path + '"')
              )

grammar = ZeroOrMore(include) + OneOrMore(module)
grammar.ignore("//" + restOfLine)
grammar.ignore(cStyleComment)
