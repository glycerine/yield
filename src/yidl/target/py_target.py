# yidl/target/py_target.py

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


from yutil import indent, lpad, pad

from yidl.target import *


__all__ = [
           "INDENT_SPACES",
           "PyBooleanType",
           "PyConstant",
           "PyInterface",
           "PyMapType",
           "PyNumericType",
           "PyOperation",
           "PySequenceType",
           "PyStringType",
           "PyStructType",
           "PyTarget",
          ]


# Constants
INDENT_SPACES = ' ' * 4


class PyType:
    def get_marshal_call( self, var_name, var_qname ):
        return "marshaller.write( %(var_name)s, %(var_qname)s )" % locals()


class PyPrimitiveType(PyType):
    def get_constant_value( self, value ):
        return value


class PyBooleanType(BooleanType, PyPrimitiveType):
    def get_boxed_name( self ):
        return "bool"

    def get_constant_value( self, value ):
        if "true" in value.lower:
            return "True"
        else:
            return "False"

    def get_default_value( self ):
        return "False"

    def get_unmarshal_call( self, var_name, var_qname ):
        return "%(var_qname)s = unmarshaller.read_bool( %(var_name)s )" % locals()


class PyCompoundType(PyType):
    def get_boxed_name( self ):
        return self.get_name()

    def get_default_value( self ):
        return "None"

    def get_unmarshal_call( self, var_name, var_qname ):
        name = str( self.get_name() )
        return "%(var_qname)s = %(name)s(); unmarshaller.read( %(var_name)s, %(var_qname)s )" % locals()


class PyConstant(Constant):
    def __repr__( self ):
        return str( self.get_identifier() ) +\
               " = " +\
               self.get_type().get_constant_value( self.get_value() ) +\
               '\n'


class PyExceptionType(ExceptionType, PyType):
    def __repr__( self ):
        name = str( self.get_name() )

        if len( self.get_members() ) > 0:
            raise NotImplementedError
        else:
            return "class %(name)s(Exception): pass\n" % locals()


class PyInclude(Include):
    def __repr__( self ):
        return ""


class PyInterface(Interface):
    def __repr__( self ):
        name = str( self.get_name() )

        child_constructs = [repr( child_construct )
                            for child_construct in self.get_child_constructs()]
        if len( child_constructs ) > 0:
            child_constructs = '\n'.join( child_constructs )
            child_constructs = indent( INDENT_SPACES, child_constructs )
            child_constructs = '\n' + child_constructs
        else:
            child_constructs = " pass"

        return """\
class %(name)s(object):%(child_constructs)s
""" % locals()


class PyMapType(MapType, PyCompoundType):
    def __repr__( self ):
        name = self.get_name()
        key_unmarshal_call = self.get_key_type().get_unmarshal_call( "None", "key" )
        value_name = self.get_value_type().get_name()
        value_marshal_call = self.get_value_type().get_marshal_call( "key", "value" )
        value_unmarshal_call = self.get_value_type().get_unmarshal_call( "key", "value" )
        return """
class %(name)s(dict):
    def marshal( self, marshaller ):
        for key, value in self.iteritems():
            %(value_marshal_call)s

    def unmarshal( self, unmarshaller ):
        %(key_unmarshal_call)s
        %(value_unmarshal_call)s
        self[key] = value

""" % locals()


class PyNumericType(NumericType, PyPrimitiveType):
    def get_boxed_name( self ):
        if self.get_name() == "float" or self.get_name() == "double": return "float"
        elif self.get_name().endswith( "int64_t" ): return "long"
        else: return "int"

    def get_default_value( self ):
        return "0"

    def get_unmarshal_call( self, var_name, var_qname ):
        name = str( self.get_name() )
        if name.endswith( "_t" ): name = name[:-2]
        return "%(var_qname)s = unmarshaller.read_%(name)s( %(var_name)s )" % locals()


class PyOperation(Operation):
    def __repr__( self ):
        name = self.get_name()
        parameters = ["self"]
        for parameter in self.get_parameters():
            assert not parameter.is_out()
            parameters.append( parameter.get_identifier() )
        parameters = pad( " ", ", ".join( parameters ), " " )
        return "def %(name)s(%(parameters)s): raise NotImplementedError" % locals()


class PySequenceType(SequenceType, PyCompoundType):
    def __repr__( self ):
        name = self.get_name()
        value_marshal_call =\
            self.get_value_type().get_marshal_call( "str( i )", "self[i]" )
        value_unmarshal_call =\
            self.get_value_type().get_unmarshal_call( "str( i )", "self[i]" )
        return """
class %(name)s(list):
    def marshal( self, marshaller ):
        for i in xrange( len( self ) ):
            %(value_marshal_call)s

    def unmarshal( self, marshaller ):
        for i in xrange( len( self ) ):
            %(value_unmarshal_call)s
""" % locals()


class PyStringType(StringType, PyPrimitiveType):
    def get_boxed_name( self ):
        return "str"

    def get_constant_value( self, value ):
        return "\"" + value + "\""

    def get_default_value( self ):
        return "\"\""

    def get_unmarshal_call( self, var_name, var_qname ):
        return "%(var_qname)s = unmarshaller.read_str( %(var_name)s )" % locals()


class PyStructType(StructType, PyCompoundType):
    def __repr__( self ):
        name = self.get_name()

        base_names = self.get_base_names()
        if len( base_names ) == 0: base_names = ( "object", )
        base_names = ", ".join( base_names )

        other_methods = indent( INDENT_SPACES, self.get_other_methods() )

        if len( self.get_members() ) > 0:
            init_parameters = []; init_statements = []
            marshal_statements = []; unmarshal_statements = []

            for member in self.get_members():
                init_parameters.append(
                    member.get_identifier() +\
                    "=" +\
                    member.get_type().get_default_value()
                )

                init_statements.append(
                    "self." + member.get_identifier() +\
                    " = " +\
                    member.get_identifier()
                )

                marshal_statements.append(
                    member.get_type().get_marshal_call(
                        "\"" + member.get_identifier() + "\"",
                        "self." + member.get_identifier()
                    )
                )

                unmarshal_statements.append(
                    member.get_type().get_unmarshal_call(
                        "\"" + member.get_identifier() + "\"",
                        "self." + member.get_identifier()
                    )
                )

            init_parameters = ", ".join( init_parameters )

            init_statements = '\n'.join( init_statements )
            init_statements = indent( INDENT_SPACES * 2, init_statements )

            marshal_statements = '\n'.join( marshal_statements )
            marshal_statements = indent( INDENT_SPACES * 2, marshal_statements )

            unmarshal_statements = '\n'.join( unmarshal_statements )
            unmarshal_statements = indent( INDENT_SPACES * 2, unmarshal_statements )

            other_methods = lpad( "\n\n", other_methods )

            return """\
class %(name)s(%(base_names)s):
    def __init__( self, %(init_parameters)s ):
%(init_statements)s

    def marshal( self, marshaller ):
%(marshal_statements)s

    def unmarshal( self, unmarshaller ):
%(unmarshal_statements)s%(other_methods)s

""" % locals()
        else:
            if len( other_methods ) == 0:
                other_methods = " pass"

            return """\
class %(name)s(%(base_names)s):%(other_methods)s
""" % locals()

    def get_other_methods( self ):
        return ""


class PyTarget(Target): pass
