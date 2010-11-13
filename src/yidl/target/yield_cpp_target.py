# yidl/target/yield_cpp_target.py

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


from yutil import indent, lpad, pad, rpad, write_file

from yidl.target.cpp_target import *


__all__ = \
[
    "YieldCPPExceptionType",
    "YieldCPPInterface",
    "YieldCPPOperation",
    "YieldCPPTarget",
]


class YieldCPPConstruct:
    def get_parent_name_macro( self ):
        return "_".join( self.get_name()[:-1] ).upper()


class YieldCPPType(YieldCPPConstruct): pass


class YieldCPPCompoundType(YieldCPPType):
    def get_includes( self ):
        return [
                   "yield/marshal/marshaller.hpp",
                   "yield/marshal/string_literal.hpp",
                   "yield/marshal/unmarshaller.hpp"
               ]


class YieldCPPBooleanType(CPPBooleanType, YieldCPPType):
    def get_boxed_name( self ):
        return "::yield::marshal::Boolean"

    def get_include( self ):
        return ["yield/marshal/boolean.hpp"]

    def get_unmarshal_call( self, key, value ):
        return "%(value)s = unmarshaller.read_bool( %(key)s )" % locals()


class YieldCPPBufferType(CPPBufferType, YieldCPPType):
    def get_cpp_name( self ):
        return "::yield::marshal::Buffer*"

    def get_destructor_statement( self, identifier ):
        return "::yield::marshal::Buffer::dec_ref( %(identifier)s );" % locals()

    def get_default_value( self ):
        return "NULL"

    def get_dummy_value( self ):
        return "NULL"

    def get_getter( self, identifier ):
        return "::yield::marshal::Buffer* get_%(identifier)s() const { return %(identifier)s; }" % locals()

    def get_in_declaration( self, identifier, default_value=None ):
        assert default_value is None
        return "::yield::marshal::Buffer* %(identifier)s" % locals()

    def get_out_declaration( self, identifier ):
        return "::yield::marshal::Buffer* %(identifier)s" % locals()

    def get_initializer( self, identifier ):
        return "::yield::marshal::Object::inc_ref( %(identifier)s )" % locals()

    def get_marshal_call( self, key, value ):
        return """if ( %(value)s != NULL ) marshaller.write( %(key)s, *%(value)s )""" % locals()

    def get_return_name( self ):
        return "::yield::marshal::Buffer*"

    def get_setter( self, identifier ):
        return "void set_%(identifier)s( ::yield::marshal::Buffer* %(identifier)s ) { ::yield::marshal::Buffer::dec_ref( this->%(identifier)s ); this->%(identifier)s = ::yield::marshal::Object::inc_ref( %(identifier)s ); }" % locals()

    def get_static_constant_name( self ):
        raise NotImplementedError

    def get_unmarshal_call( self, key, value ):
        return """if ( %(value)s != NULL ) unmarshaller.read( %(key)s, *%(value)s ); else %(value)s = unmarshaller.read_buffer( %(key)s )""" % locals()


class YieldCPPEnumeratedType(CPPEnumeratedType, YieldCPPType):
    def get_marshal_call( self, key, value ):
        return """marshaller.write( %(key)s, static_cast<int32_t>( %(value)s ) )""" % locals()

    def get_unmarshal_call( self, key, value ):
        name = self.get_cpp_name()
        return """%(value)s = static_cast<%(name)s>( unmarshaller.read_int32( %(key)s ) )""" % locals()


class YieldCPPExceptionType(CPPExceptionType, YieldCPPType):
    def get_createException_type_id_case(self):
        name = str( self.get_name() )
        type_id = int( self.get_name() )
        return "case %(type_id)s: return new %(name)s;" % locals()

    def get_createException_type_name_strncmp( self ):
        name = str( self.get_name() )
        name_len = len( name )
        return "if ( type_name_len == %(name_len)u && strncmp( type_name, \"%(name)s\", %(name_len)u ) == 0 ) return new %(name)s;" % locals()

    def __repr__( self ):
        name = str( self.get_name() )
        parent_name_macro = self.get_parent_name_macro()
        struct_type =\
            CPPStructType(
                self.get_parent_construct(),
                self.get_name(),
                ( "%(parent_name_macro)s_EXCEPTION_PARENT_CLASS" % locals(), ),
                self.get_members()
            )

        if len( self.get_members() ) > 0:
            accessors = []
            default_initializers = []
            member_declarations = []
            unmarshal_calls = []

            have_error_code = have_error_message = False
            for member in self.get_members():
                identifier = str( member.get_identifier() )
                identifier_int = int( member.get_identifier() )
                if identifier == "error_code" or identifier == "error_message":
                    if identifier == "error_code":
                        assert member.get_type().get_name() == "uint32_t"
                        have_error_code = True
                    elif identifier == "error_message":
                        assert member.get_type().get_name() == "string"
                        have_error_message = True

                    unmarshal_call = [member.get_type().get_cpp_name() + " " + identifier + ';']
                    unmarshal_call.append(
                        member.get_type().get_unmarshal_call(
                            '::yield::marshal::StringLiteral( \"%(identifier)s\" )' % locals(),
                            identifier
                        ) + ';'
                    )
                    unmarshal_call.append( "set_%(identifier)s( %(identifier)s )" % member )
                    unmarshal_calls.append( " ".join( unmarshal_call ) )
                else:
                    accessors.append( member.get_type().get_getter( identifier ) )
                    accessors.append( member.get_type().get_setter( identifier ) )

                    member_declarations.append(
                        member.get_type().get_cpp_name() +\
                        " " +\
                        identifier +\
                        ";"
                    )

                    if member.get_default_value() is not None:
                        default_initializers.append(
                            identifier +\
                            "( " +\
                            member.get_default_value() +\
                            " )"
                        )
                    elif member.get_type().get_default_value() is not None:
                        default_initializers.append(
                            identifier +\
                            "( " +\
                            member.get_type().get_default_value() +\
                            " )"
                        )

                    unmarshal_calls.append(
                        member.get_type().get_unmarshal_call(
                            '::yield::marshal::StringLiteral( \"%(identifier)s\" )' % member,
                            identifier )
                    )

            accessors = "\n".join( accessors )
            constructors = []
            default_initializers = ", ".join( default_initializers )

            default_constructor = name + "()"
            if len( default_initializers ) > 0:
                default_constructor += " : " + default_initializers
            default_constructor += " { }"
            constructors.append( default_constructor )

            default_initializers = lpad( ", ", default_initializers )

            if have_error_code:
                constructors.append( "%(name)s( uint32_t error_code ) : %(parent_name_macro)s_EXCEPTION_PARENT_CLASS( error_code )%(default_initializers)s { }" % locals() )

            if have_error_message:
                constructors.append( "%(name)s( const char* error_message ) : %(parent_name_macro)s_EXCEPTION_PARENT_CLASS( error_message )%(default_initializers)s { }" % locals() )
                constructors.append( "%(name)s( const string& error_message ) : %(parent_name_macro)s_EXCEPTION_PARENT_CLASS( error_message )%(default_initializers)s { }" % locals() )

            if have_error_code and have_error_message:
                constructors.append( "%(name)s( uint32_t error_code, const char* error_message ) : %(parent_name_macro)s_EXCEPTION_PARENT_CLASS( error_code, error_message )%(default_initializers)s { }" % locals() )
                constructors.append( "%(name)s( uint32_t error_code, const string& error_message ) : %(parent_name_macro)s_EXCEPTION_PARENT_CLASS( error_code, error_message )%(default_initializers)s { }" % locals() )

            if len( member_declarations ) > 0:
                member_declarations = "\n\nprotected:\n  " + "\n  ".join( member_declarations )

                # Full constructor
                in_declarations =\
                    ", ".join(
                        [member.get_type().get_in_declaration( identifier, member.get_default_value() )
                        for member in self.get_members()]
                    )

                initializers = []
                parent_initializers = []
                for member in self.get_members():
                    identifier = str( member.get_identifier() )
                    if identifier == "error_code" or identifier == "error_message":
                        parent_initializers.append( identifier )
                    else:
                        initializers.append(
                            identifier +\
                            "( " +\
                            member.get_type().get_initializer( identifier ) +\
                            " )"
                        )

                if len( parent_initializers ) > 0:
                    initializers.insert(
                        0,
                        "%(parent_name_macro)s_EXCEPTION_PARENT_CLASS( " % locals() +\
                        ", ".join( parent_initializers ) +\
                        " )"
                    )

                initializers = ", ".join( initializers )
                constructors.append( "%(name)s( %(in_declarations)s ) : %(initializers)s { }" % locals() )
            else:
                member_declarations = ""

            constructors = "\n".join( constructors )

            unmarshal_calls = ";\n  ".join( unmarshal_calls )
            unmarshal_method = """\
void unmarshal( ::yield::marshal::Unmarshaller& unmarshaller )
{
  %(unmarshal_calls)s;
}
""" % locals()
        else:
            accessors = struct_type.get_accessors()
            constructors = struct_type.get_constructors()
            member_declarations = struct_type.get_member_declarations()
            unmarshal_method = struct_type.get_unmarshal_method()

        accessors = lpad( "\n\n", indent( INDENT_SPACES, accessors ) )
        constructors = indent( INDENT_SPACES, constructors )
        destructor_statements = struct_type.get_destructor_statements()
        marshal_method = indent( INDENT_SPACES, struct_type.get_marshal_method() )
        member_getter_calls =\
            pad(
                " ",
                ", ".join( ["get_" + member.get_identifier() + "()"
                            for member in self.get_members()] ),
                " "
            )
        type_id = int( self.get_name() )
        unmarshal_method = indent( INDENT_SPACES, unmarshal_method )

        return """\
class %(name)s : public %(parent_name_macro)s_EXCEPTION_PARENT_CLASS
{
public:
  const static uint32_t TYPE_ID = %(type_id)uUL;

public:
%(constructors)s
  virtual ~%(name)s() throw() { %(destructor_statements)s; }%(accessors)s

  // yield::Object
  uint32_t get_type_id() const { return TYPE_ID; }
  const char* get_type_name() const { return "%(name)s"; }

  // yield::marshal::Object
%(marshal_method)s
%(unmarshal_method)s

  // yield::Exception
  virtual ::yield::Exception& clone() const
  {
    return *new %(name)s(%(member_getter_calls)s);
  }

  virtual void throwStackClone() const
  {
    throw %(name)s(%(member_getter_calls)s);
  }%(member_declarations)s
};
""" % locals()



class YieldCPPInterface(CPPInterface, YieldCPPConstruct):
    def _get_messages( self ):
        name = str( self.get_name() )
        if len( self.get_operations() ) > 0 or len( self.get_exception_types() ) > 0:
            messages = []

            for operation in self.get_operations():
                messages.append( repr( operation.get_request_type() ) )
                if not operation.is_oneway():
                    messages.append( repr( operation.get_response_type() ) )

            for exception_type in self.get_exception_types():
                messages.append( repr( exception_type ) )

            messages = indent( INDENT_SPACES, "\n\n".join( messages ) )

            return """\
class %(name)sMessages
{
public:
  // Request/response pair definitions for the operations in %(name)s
%(messages)s
};
""" % locals()
        else:
            return "class %(name)sMessages { };" % locals()

    def _get_message_factory( self ):
        if len( self.get_exception_types() ) > 0:
            createException_type_id_cases = indent( INDENT_SPACES * 2, "\n".join( [exception_type.get_createException_type_id_case() for exception_type in self.get_exception_types()] ) )
            createException_type_name_strncmps = indent( INDENT_SPACES, "\nelse ".join( [exception_type.get_createException_type_name_strncmp() for exception_type in self.get_exception_types()] ) )
            createException = """\
virtual ::yield::Exception* createException( uint32_t type_id )
{
  switch ( type_id )
  {
%(createException_type_id_cases)s
    default: return NULL;
  }
}

virtual ::yield::Exception*
createException
(
  const char* type_name,
  size_t type_name_len
)
{
%(createException_type_name_strncmps)s
  else return NULL;
}
""" % locals()
        else:
            createException = ""

        if len( self.get_operations() ) > 0:
            createRequest_type_id_cases = indent( INDENT_SPACES * 2, "\n".join( [operation.get_createRequest_type_id_case() for operation in self.get_operations()] ) )
            createRequest_type_name_strncmps = indent( INDENT_SPACES, "\nelse ".join( [operation.get_createRequest_type_name_strncmp() for operation in self.get_operations()] ) )
            createRequest = """\
virtual ::yield::concurrency::Request* createRequest( uint32_t type_id )
{
  switch ( type_id )
  {
%(createRequest_type_id_cases)s
    default: return NULL;
  }
}

virtual ::yield::concurrency::Request*
createRequest
(
  const char* type_name,
  size_t type_name_len
)
{
%(createRequest_type_name_strncmps)s
  else return NULL;
}
""" % locals()
            createResponse_type_id_cases = indent( INDENT_SPACES * 2, "\n".join( [operation.get_createResponse_type_id_case() for operation in self.get_operations()] ) )
            createResponse_type_name_strncmps = indent( INDENT_SPACES, "\nelse ".join( [operation.get_createResponse_type_name_strncmp() for operation in self.get_operations()] ) )
            createResponse = """\
virtual ::yield::concurrency::Response* createResponse( uint32_t type_id )
{
  switch ( type_id )
  {
%(createResponse_type_id_cases)s
    default: return NULL;
  }
}

virtual ::yield::concurrency::Response*
createResponse
(
  const char* type_name,
  size_t type_name_len
)
{
%(createResponse_type_name_strncmps)s
  else return NULL;
}
""" % locals()
        else:
            createRequest = createResponse = ""

        name = str( self.get_name() )
        createException = pad( "\n", indent( INDENT_SPACES, createException ), "\n" )
        createRequest = pad( "\n", indent( INDENT_SPACES, createRequest ), "\n" )
        createResponse = pad( "\n", indent( INDENT_SPACES, createResponse ), "\n" )

        marshallable_object_factory = self.get_parent_target().get_marshallable_object_factory()
        marshallable_object_factory = indent( INDENT_SPACES, marshallable_object_factory )

        return """\
class %(name)sMessageFactory
  : public ::yield::concurrency::MessageFactory,
    private %(name)sMessages
{
public:
  // yield::concurrency::MessageFactory%(createException)s%(createRequest)s%(createResponse)s

  // yield::marshal::MarshallableObjectFactory
%(marshallable_object_factory)s
};
""" % locals()

    def _get_request_handler( self ):
        name = str( self.get_name() )

        request_handler_cases = indent( INDENT_SPACES * 3, "\n".join( [operation.get_request_handler_case() for operation in self.get_operations()] ) )
        handle = """\
  virtual void handle( ::yield::concurrency::Request& request )
  {
    // Switch on the request types that this interface handles, unwrap the corresponding requests and delegate to _interface
    switch ( request.get_type_id() )
    {
%(request_handler_cases)s
    }
  }
""" % locals()

        request_handler_defs = indent( INDENT_SPACES, "\n".join( [operation.get_request_handler_def() for operation in self.get_operations()] ) )

        name_upper = '_'.join( self.get_name() ).upper()
        defines = lpad( """\n\n#define %(name_upper)s_REQUEST_HANDLER_PROTOTYPES \\\n""" % locals(), "\\\n".join( [operation.get_request_handler_prototype() + ";" for operation in self.get_operations()] ) )

        return """\
class %(name)sRequestHandler
  : public ::yield::concurrency::RequestHandler,
    protected %(name)sMessages
{
public:
  %(name)sRequestHandler()  // Subclasses must implement
    : _interface( NULL ) // all relevant handle*Request methods
  { }

  // Steals interface_ to allow for *new
  %(name)sRequestHandler( %(name)s& _interface )
    : _interface( &_interface )
  { }

  virtual ~%(name)sRequestHandler()
  {
    delete _interface;
  }

  // yield::concurrency::RequestHandler
%(handle)s
protected:

%(request_handler_defs)s

private:
  %(name)s* _interface;
};%(defines)s
""" % locals()

    def _get_proxy( self ):
        name = str( self.get_name() )
        request_send_defs = indent( INDENT_SPACES, "\n".join( [operation.get_request_proxy_definition() for operation in self.get_operations()] ) )

        return """\
class %(name)sProxy
  : public %(name)s,
    public ::yield::concurrency::RequestHandler,
    private %(name)sMessages
{
public:
  %(name)sProxy( ::yield::concurrency::EventHandler& request_handler )
    : __request_handler( request_handler )
  { }

  ~%(name)sProxy()
  {
    ::yield::concurrency::EventHandler::dec_ref( __request_handler );
  }

  // yield::marshal::RTTIObject
  virtual const char* get_type_name() const
  {
    return "%(name)sProxy";
  }

  // yield::concurrency::RequestHandler
  virtual void handle( ::yield::concurrency::Request& request )
  {
    __request_handler.handle( request );
  }

  // %(name)s
%(request_send_defs)s

private:
  // __request_handler is not a counted reference, since that would create
  // a reference cycle when __request_handler is a subclass of %(name)sProxy
  ::yield::concurrency::EventHandler& __request_handler;
};
""" % locals()

    def get_includes( self ):
        if self.is_local():
            return CPPInterface.get_includes( self )
        else:
            return CPPInterface.get_includes( self ) +\
                   [CPPInclude( self.get_parent_target(), "yield/concurrency.h" )]

    def get_parent_class_macros( self ):
        name_upper = '_'.join( self.get_name() ).upper()
        parent_class_name_uppers = ['_'.join( self.get_name()[:name_part_i] ).upper() for name_part_i in xrange( 1, len( self.get_name() ) )]
        parent_class_name_uppers.reverse()
        all_parent_class_macros = []
        for define_class_name, default_parent_class_name in \
        (
            ( "EXCEPTION", "::yield::Exception" ),
            ( "REQUEST", "::yield::concurrency::Request" ),
            ( "RESPONSE", "::yield::concurrency::Response" ),
        ):
            all_parent_class_macros.append( "#ifndef %(name_upper)s_%(define_class_name)s_PARENT_CLASS\n" % locals() )
            all_parent_class_macros.append(
                "#" +\
                "#el".join(
                    ["if defined( %(parent_class_name_upper)s_%(define_class_name)s_PARENT_CLASS )\n" % locals() + \
                     "#define %(name_upper)s_%(define_class_name)s_PARENT_CLASS %(parent_class_name_upper)s_%(define_class_name)s_PARENT_CLASS\n" % locals()
                     for parent_class_name_upper in parent_class_name_uppers]
                )
            )
            all_parent_class_macros.append( "#else\n" )
            all_parent_class_macros.append( "#define %(name_upper)s_%(define_class_name)s_PARENT_CLASS %(default_parent_class_name)s\n" % locals() )
            all_parent_class_macros.append( "#endif\n#endif\n" )
        return "".join( all_parent_class_macros )

    def __repr__( self ):
        have_operations = len( self.get_operations() ) > 0
        have_exception_types = len( self.get_exception_types() ) > 0

        if self.is_local() or ( not have_operations and not have_exception_types ):
            return CPPInterface.__repr__( self )
        else:
            yield_cpp_interface = [CPPInterface.__repr__( self )]
            yield_cpp_interface.append( self.get_parent_class_macros() )
            yield_cpp_interface.append( self._get_messages() )
            yield_cpp_interface.append( self._get_message_factory() )
            if have_operations:
                yield_cpp_interface.append( self._get_request_handler() )
                yield_cpp_interface.append( self._get_proxy() )

            return "\n\n".join( yield_cpp_interface )


class YieldCPPMapType(CPPMapType, YieldCPPCompoundType):
    def get_base_names( self ):
        key_type_boxed_name = self.get_key_type().get_boxed_name()
        value_type_cpp_name = self.get_value_type().get_cpp_name()
        return [
                   "::yield::marshal::Map",
                   "std::map< %(key_type_boxed_name)s, %(value_type_cpp_name)s >" % locals()
               ]

    def get_includes( self ):
        return CPPMapType.get_includes( self ) +\
               YieldCPPCompoundType.get_includes( self ) +\
               ["yield/marshal/map.hpp"]

    def get_other_methods( self ):
        key_type_boxed_name = self.get_key_type().get_boxed_name()
        name = str( self.get_name() )
        type_id = int( self.get_name() )
        value_type_cpp_name = self.get_value_type().get_cpp_name()
        value_marshal_call = self.get_value_type().get_marshal_call( "i->first", "i->second" )
        value_unmarshal_call = self.get_value_type().get_unmarshal_call( "key", "value" )
        return """\
// yield::marshal::Object
const static uint32_t TYPE_ID = static_cast<uint32_t>( %(type_id)u );
uint32_t get_type_id() const { return TYPE_ID; }
const char* get_type_name() const { return "%(name)s"; }
%(name)s& inc_ref() { return ::yield::Object::inc_ref( *this ); }

void marshal( ::yield::marshal::Marshaller& marshaller ) const
{
  for ( const_iterator i = begin(); i != end(); i++ )
  {
    %(value_marshal_call)s;
  }
}

void unmarshal( ::yield::marshal::Unmarshaller& unmarshaller )
{
  %(key_type_boxed_name)s key;
  unmarshaller.read_key( key );
  %(value_type_cpp_name)s value;
  %(value_unmarshal_call)s;
  ( *this )[key] = value;
}

// yield::marshal::Map
size_t get_size() const { return size(); }
""" % locals()

    def get_unmarshal_call( self, key, value ):
        return """unmarshaller.read_map( %(key)s, %(value)s )""" % locals()


class YieldCPPNumericType(CPPNumericType, YieldCPPType):
    def get_boxed_name( self ):
        if self.get_name() in ( "double", "float" ):
            return "::yield::marshal::Double"
        else:
            return "::yield::marshal::Integer"

    def get_includes( self ):
        if self.get_name() in ( "double", "float" ):
            return ["yield/marshal/double.hpp"]
        else:
            return ["yield/marshal/integer.hpp"]

    def get_unmarshal_call( self, key, value ):
        name = str( self.get_name() )
        if name.endswith( "_t" ): name = name[:-2]
        return "%(value)s = unmarshaller.read_%(name)s( %(key)s )" % locals()


class YieldCPPOperation(CPPOperation, YieldCPPConstruct):
    def get_createRequest_type_id_case( self ):
        name = str( self.get_request_type().get_name() )
        type_id = int( self.get_request_type().get_name() )
        return "case %(type_id)u: return new %(name)s;" % locals()

    def get_createRequest_type_name_strncmp( self ):
        name = str( self.get_request_type().get_name() )
        name_len = len( name )
        return "if ( type_name_len == %(name_len)u && strncmp( type_name, \"%(name)s\", %(name_len)u ) == 0 ) return new %(name)s;" % locals()

    def get_createResponse_type_id_case( self ):
        name = str( self.get_response_type().get_name() )
        type_id = int( self.get_response_type().get_name() )
        return "case %(type_id)u: return new %(name)s;" % locals()

    def get_createResponse_type_name_strncmp( self ):
        name = str( self.get_response_type().get_name() )
        name_len = len( name )
        return "if ( type_name_len == %(name_len)u && strncmp( type_name, \"%(name)s\", %(name_len)u ) == 0 ) return new %(name)s;" % locals()

    def get_request_handler_case( self ):
        return self.get_request_type().get_request_handler_case()

    def get_request_handler_def( self ):
        name = str( self.get_name() )

        temp_out_variables = []
        call_parameters = []
        respond_parameters = []

        for parameter in self.get_parameters():
            parameter_identifier = parameter.get_identifier()

            if parameter.is_out():
                if parameter.is_in():
                    temp_out_variables.append(
                        parameter.get_type().get_temp_name() +\
                        " " +\
                        parameter_identifier +\
                        "( __request.get_%(parameter_identifier)s()" % locals() +\
                         " );"
                    )
                else:
                    temp_out_variables.append(
                        parameter.get_type().get_temp_name() +\
                        " " +\
                        parameter_identifier +\
                        ";"
                    )

                call_parameters.append( parameter_identifier )
                respond_parameters.append( parameter_identifier )
            elif parameter.is_in():
                call_parameters.append( "__request.get_%(parameter_identifier)s()" % locals() )

        if self.get_return_type() is not None:
            temp_out_variables.append( self.get_return_type().get_return_name() + " _return_value =" )
            respond_parameters.append( "_return_value" )

        temp_out_variables =\
            rpad( indent( INDENT_SPACES * 3, "\n".join( temp_out_variables ) ), "\n\n" )

        test_call_params = pad( " ", ", ".join( call_parameters ), " " )
        test_call = INDENT_SPACES * 3 + "_interface->%(name)s(%(test_call_params)s);" % locals()
        if len( test_call ) <= 80:
            call = test_call
        else:
            call= ["(" ]
            call.append( INDENT_SPACES + ( ",\n" + INDENT_SPACES ).join( call_parameters ) )
            call.append( ");" )
            call = INDENT_SPACES * 3 + "_interface->%(name)s\n" % locals() + \
                   indent( INDENT_SPACES * 3, "\n".join( call ) )

        if len( respond_parameters ) > 0:
            respond = "\n\n" + INDENT_SPACES * 3 +\
                      "__request.respond( " +\
                      ", ".join( respond_parameters ) +\
                      " );"
        else:
            respond = ""

        return """\
virtual void handle( %(name)sRequest& __request )
{
  if ( _interface != NULL )
  {
    try
    {
%(temp_out_variables)s%(call)s%(respond)s
    }
    catch( ::yield::Exception* exception )
    {
      __request.respond( *exception );
    }
    catch ( ::yield::Exception& exception )
    {
      __request.respond( exception.clone() );
    }
  }
}
""" % locals()

    def get_request_handler_prototype( self ):
        return self.get_request_type().get_request_handler_prototype()

    def get_request_proxy_definition( self ):
        name = str( self.get_name() )

        parameter_identifiers =\
            rpad(
                ", ".join(
                    [str( parameter.get_identifier() )
                     for parameter in self.get_parameters()]
                ),
            ", "
        )

        in_parameter_identifiers =\
            pad(
                "( ",
                ", ".join(
                    [str( parameter.get_identifier() )
                     for parameter in self.get_in_parameters()] ),
                " )"
            )

        prototype = CPPOperation.__repr__( self )

        if self.is_oneway():
            return """\
%(prototype)s
{
  handle( *new %(name)sRequest%(in_parameter_identifiers)s );
}
""" % locals()
        else:
            out_setters = []
            for parameter in self.get_parameters():
                if parameter.is_out():
                    lvalue = str( parameter.get_identifier() )
                    rvalue = "__response->get_" + parameter.get_identifier() + "()"
                    out_setters.append( lvalue + " = " + rvalue + ";"  )

            if self.get_return_type() is not None:
                lvalue = self.get_return_type().get_return_name() + " _return_value"
                rvalue = "__response->get__return_value()"
                out_setters.append( lvalue + " = " + rvalue + ";"  )

            out_setters = lpad( "\n", indent( INDENT_SPACES, "\n".join( out_setters ) ) )

            if self.get_return_type() is not None:
                return_statement = "return _return_value; "
            else:
                return_statement = ""

            return """\
%(prototype)s
{
  %(name)sRequest* __request = new %(name)sRequest%(in_parameter_identifiers)s;

  ::yield::platform::auto_Object< ::yield::concurrency::ResponseQueue<%(name)sResponse> >
    __response_queue( new ::yield::concurrency::ResponseQueue<%(name)sResponse> );
  __request->set_response_handler( *__response_queue );

  handle( *__request );

  ::yield::platform::auto_Object<%(name)sResponse> __response = __response_queue->dequeue();%(out_setters)s%(return_statement)s
}
""" % locals()

    def get_request_type( self ):
        return YieldCPPRequestType(
                   self,
                   self.get_name() + "Request",
                   members=self.get_in_parameters()
                )

    def get_response_type( self ):
        return YieldCPPRequestType(
                   self,
                   self.get_name() + "Response",
                   members=self.get_out_parameters()
                )


class YieldCPPPointerType(CPPPointerType, YieldCPPType):
    def get_unmarshal_call( self, key, value ):
        cpp_name = self.get_cpp_name()
        return """%(value)s = static_cast<%(cpp_name)s>( unmarshaller.read_pointer( %(key)s ) )""" % locals()


class YieldCPPRequestType(CPPStructType, YieldCPPType):
    def get_other_methods( self ):
        if self.get_parent_operation().is_oneway():
            return ""

        response_type_name = self.get_parent_operation().get_response_type().get_name()

        respond_parameters = []
        response_parameters = []
        for out_parameter in self.get_parent_operation().get_response_type().get_members():
            respond_parameters.append( out_parameter.get_type().get_in_declaration( out_parameter.get_identifier() ) )
            response_parameters.append( out_parameter.get_identifier() )

        test_respond_parameters = pad( " ", ", ".join( respond_parameters ), " " )
        test_respond_prototype = "virtual void respond(%(respond_parameters)s)" % locals()
        if len( test_respond_prototype ) <= 80:
            respond_parameters = test_respond_parameters
            response_parameters = pad( " ", ", ".join( response_parameters ), " " )
            respond = """
virtual void respond(%(respond_parameters)s)
{
  respond( *new %(response_type_name)s(%(response_parameters)s) );
}
""" % locals()
        else:
            respond_parameters = indent( INDENT_SPACES, "\n,".join( respond_parameters ) )
            response_parameters = indent( INDENT_SPACES * 5 + ' ', "\n,".join( response_parameters ) )
            respond = """
virtual void
respond
(
%(respond_parameters)s
)
{
  respond
  (
    *new %(response_type_name)s
         (
%(response_parameters)s
         )
  );
}
""" % locals()

        createDefaultResponse = """\
virtual ::yield::concurrency::Response* createDefaultResponse()
{
"""
        response_parameters = []
        for inout_parameter in self.get_parent_operation().get_inout_parameters():
            response_parameters.append( "get_" + inout_parameter.get_identifier() + "()" )
        test_response_parameters = pad( "( ", ", ".join( response_parameters ), " )" )
        if len( test_response_parameters ) <= 0:
            response_parameters = test_response_parameters
            createDefaultResponse += """\
  return new %(response_type_name)s%(response_parameters)s;""" % locals()
        else:
            response_parameters = indent( INDENT_SPACES * 7 + ' ', "\n,".join( response_parameters ) )
            createDefaultResponse += """\
  return new %(response_type_name)s
             (
%(response_parameters)s
             );""" % locals()
        createDefaultResponse += """
}"""

        return """\

// yield::concurrency::Request
%(createDefaultResponse)s
%(respond)s
virtual void respond( ::yield::concurrency::Response& response )
{
  Request::respond( response );
}
""" % locals()

    def get_base_names( self ):
        return ( self.get_parent_name_macro() + "_REQUEST_PARENT_CLASS", )

    def get_parent_operation( self ):
        return self.get_parent_construct()

    def get_request_handler_case( self ):
        name = str( self.get_name() )
        type_id = int( self.get_name() )
        return "case %(type_id)uUL: handle( static_cast<%(name)s&>( request ) ); return;" % locals()

    def get_request_handler_prototype( self ):
        name = str( self.get_name() )
        return "virtual void handle( %(name)s& __request )" % locals()


class YieldCPPResponseType(CPPStructType, YieldCPPType):
    def get_base_names( self ):
        return ( self.get_parent_name_macro() + "_RESPONSE_PARENT_CLASS", )


class YieldCPPSequenceType(CPPSequenceType, YieldCPPCompoundType):
    def get_includes( self ):
        return YieldCPPCompoundType.get_includes( self ) +\
               ["yield/marshal/null.hpp", "yield/marshal/sequence.hpp"]

    def get_other_methods( self ):
        name = str( self.get_name() )
        type_id = int( self.get_name() )
        value_type_cpp_name = self.get_value_type().get_cpp_name()
        value_marshal_call = self.get_value_type().get_marshal_call( "::yield::marshal::Null()", "( *this )[value_i]" )
        value_unmarshal_call = self.get_value_type().get_unmarshal_call( "::yield::marshal::Null()", "value" )
        return """\
// yield::Object
const static uint32_t TYPE_ID = static_cast<uint32_t>( %(type_id)u );
uint32_t get_type_id() const { return TYPE_ID; }
const char* get_type_name() const { return "%(name)s"; }
%(name)s& inc_ref() { return ::yield::Object::inc_ref( *this ); }

// yield::marshal::Object
void marshal( ::yield::marshal::Marshaller& marshaller ) const
{
  size_type value_i_max = size();
  for ( size_type value_i = 0; value_i < value_i_max; value_i++ )
  {
    %(value_marshal_call)s;
  }
}

void unmarshal( ::yield::marshal::Unmarshaller& unmarshaller )
{
  %(value_type_cpp_name)s value;
  %(value_unmarshal_call)s;
  push_back( value );
}

// yield::marshal::Sequence
size_t get_size() const { return size(); }
""" % locals()

    def get_base_names( self ):
        return ["::yield::marshal::Sequence"] + CPPSequenceType.get_base_names( self )

    def get_unmarshal_call( self, key, value ):
        return """unmarshaller.read_sequence( %(key)s, %(value)s )""" % locals()


class YieldCPPStringType(CPPStringType, YieldCPPType):
    def get_includes( self ):
        return CPPStringType.get_includes( self ) +\
               ["yield/marshal/string.hpp"]

    def get_boxed_name( self ):
        return "::yield::marshal::String"

    def get_unmarshal_call( self, key, value ):
        return """unmarshaller.read_string( %(key)s, %(value)s )""" % locals()


class YieldCPPStructType(CPPStructType, YieldCPPCompoundType):
    def get_includes( self ):
        return YieldCPPCompoundType.get_includes( self ) +\
               ["yield/marshal/object.hpp"]

    def get_marshal_method( self ):
        if len( self.get_members() ) > 0:
            marshal_calls = []
            for member in self.get_members():
                identifier = member.get_identifier()
                identifier_int = int( member.get_identifier() )
                key = '::yield::marshal::StringLiteral( \"%(identifier)s\" )' % locals()
                marshal_calls.append( member.get_type().get_marshal_call( key, "get_" + member.get_identifier() + "()" ) )
            marshal_calls = ";\n  ".join( marshal_calls  )

            return """\
void marshal( ::yield::marshal::Marshaller& marshaller ) const
{
  %(marshal_calls)s;
}

""" % locals()
        else:
            return "void marshal( ::yield::marshal::Marshaller& ) const { }"

    def get_base_names( self ):
        return ["::yield::marshal::Object"]

    def get_unmarshal_call( self, key, value ):
        return """unmarshaller.read_object( %(key)s, %(value)s )""" % locals()

    def get_unmarshal_method( self ):
        if len( self.get_members() ) > 0:
            unmarshal_calls = []
            for member in self.get_members():
                identifier = member.get_identifier()
                identifier_int = int( member.get_identifier() )
                key = '::yield::marshal::StringLiteral( \"%(identifier)s\" )' % locals()
                unmarshal_calls.append( member.get_type().get_unmarshal_call( key, member.get_identifier() ) )
            unmarshal_calls = ";\n  ".join( unmarshal_calls  )

            return """\
void unmarshal( ::yield::marshal::Unmarshaller& unmarshaller )
{
  %(unmarshal_calls)s;
}
""" % locals()
        else:
            return "void unmarshal( ::yield::marshal::Unmarshaller& ) { }"

    def __repr__( self ):
        accessors = lpad( "\n\n", indent( INDENT_SPACES, self.get_accessors() ) )
        constructors = indent( INDENT_SPACES, pad( "\n", self.get_constructors(), "\n\n" ) )
        destructor_statements = self.get_destructor_statements()
        marshal_method = indent( INDENT_SPACES, self.get_marshal_method() )
        member_declarations = self.get_member_declarations()
        name = str( self.get_name() )
        operator_equals = indent( INDENT_SPACES, self.get_operator_equals() )
        other_methods = lpad( "\n", indent( INDENT_SPACES, self.get_other_methods() ) )
        base_names = ", public ".join( self.get_base_names() )
        type_id = int( self.get_name() )
        unmarshal_method = indent( INDENT_SPACES, self.get_unmarshal_method() )
        return """\
class %(name)s : public %(base_names)s
{
public:
  const static uint32_t TYPE_ID = %(type_id)uUL;

public:%(constructors)s
  virtual ~%(name)s() { %(destructor_statements)s }%(accessors)s

%(operator_equals)s

  // yield::Object
  uint32_t get_type_id() const { return TYPE_ID; }
  const char* get_type_name() const { return "%(name)s"; }
  %(name)s& inc_ref() { return ::yield::Object::inc_ref( *this ); }

%(marshal_method)s
%(unmarshal_method)s%(other_methods)s%(member_declarations)s
};
""" % locals()



class YieldCPPTarget(CPPTarget):
    def __init__( self, force=False, output_file_path=None ):
        CPPTarget.__init__( self )
        self.__force = force
        self.__output_file_path = output_file_path

    def finalize( self ):
        if self.__output_file_path is not None:
            write_file( self.__output_file_path, repr( self ), force=self.__force )

    def get_marshallable_object_factory( self, class_name=None ):
        user_defined_types = []
        for module in self.get_modules():
            user_defined_types.extend( module.get_types() )

        seen_type_ids = []
        type_id_cases = []
        type_name_strncmps = []
        for type in user_defined_types:
            type_name = str( type.get_name() )
            type_name_len = len( type_name )
            type_id = int( type.get_name )
            if type_id != 0 and type_id not in seen_type_ids:
                type_id_cases.append( "case %(type_id)s: return new %(type_name)s;" % locals() )
                seen_type_ids.append( type_id )
            type_name_strncmps.append( "if ( type_name_len == %(type_name_len)u && strncmp( type_name, \"%(type_name)s\", %(type_name_len)u ) == 0 ) return new %(type_name)s;" % locals() )

        if len( type_id_cases ) > 0:
            type_id_cases = indent( INDENT_SPACES * 2, "\n".join( type_id_cases ) )
            createMarshallableObject_type_id = """\
virtual ::yield::marshal::MarshallableObject*
createMarshallableObject
(
  uint32_t type_id
)
{
  switch ( type_id )
  {
%(type_id_cases)s
    default: return NULL;
  }
}

""" % locals()
        else:
            createMarshallableObject_type_id = ""

        if len( type_name_strncmps ) > 0:
            type_name_strncmps.append( "return NULL;" )
            type_name_strncmps = indent( INDENT_SPACES, "\nelse ".join( type_name_strncmps ) )
            createMarshallableObject_type_name = """\
virtual ::yield::marshal::MarshallableObject*
createMarshallableObject
(
  const char* type_name,
  size_t type_name_len
)
{
%(type_name_strncmps)s
}
""" % locals()
        else:
            createMarshallableObject_type_name = ""

        if class_name is not None:
            "class " + class_name + "\n{" + \
            indent( INDENT_SPACES, createMarshallableObject_type_id ) + \
            indent( INDENT_SPACES, createMarshallableObject_type_name ) + \
            "\n};"
        else:
            return createMarshallableObject_type_id + createMarshallableObject_type_name
