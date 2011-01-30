# yidl/target/cpp_target.py

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

from yutil import indent, lpad, pad, rpad, static_cast

from yidl.target.c_target import *


__all__ = \
[
   "CPPBooleanType",
   "CPPBufferType",
   "CPPConstant",
   "CPPEnumeratedType",
   "CPPExceptionType",
   "CPPInclude",
   "CPPInterface",
   "CPPMapType",
   "CPPModule",
   "CPPNumericType",
   "CPPOperation", "CPPOperationParameter",
   "CPPPointerType",
   "CPPReferenceType",
   "CPPSequenceType",
   "CPPStringType",
   "CPPStructType",
   "CPPTarget",
   "INDENT_SPACES"
]


# Constants
INDENT_SPACES = ' ' * 2


class CPPConstruct:
    def get_cpp_base_names(self):
        return lpad(
                   " : ",
                   ", ".join(
                       ["public " + base_name
                        for base_name in self.get_base_names()]
                   )
               )


class CPPType(CPPConstruct):
    def get_constant(self, identifier, value):
        name = "::".join(self.get_name())
        return "const static %(name)s %(identifier)s = %(value)s;" % locals()

    def get_cpp_name(self):
        return "::".join(self.get_name())

    def get_default_value(self):
        return None

    def get_destructor_statement(self, identifier):
        return ""

    def get_dummy_value(self):
        raise NotImplementedError

    def get_getter(self, identifier):
        return self.get_cpp_name() + \
            " get_%(identifier)s() const { return %(identifier)s; }" % locals()

    def get_in_declaration(self, identifier, default_value=None):
        return self.get_cpp_name() + \
               lpad(" ", identifier) + \
               (default_value is not None and (" = " + str(default_value)) or "")

    def get_initializer(self, identifier):
        return identifier

    def get_marshal_call(self, key, value):
        return """marshaller.write( %(key)s, %(value)s )""" % locals()

    def get_out_declaration(self, identifier):
        return self.get_cpp_name() + \
               (not self.get_cpp_name().endswith("&") and "& " or "") + identifier

    def get_return_name(self):
        return self.get_cpp_name()

    def get_setter(self, identifier):
        cpp_name = self.get_cpp_name()
        return "void set_%(identifier)s( %(cpp_name)s %(identifier)s ) { this->%(identifier)s = %(identifier)s; }" % locals()

    def get_temp_name(self):
        return self.get_cpp_name()


class CPPCompoundType(CPPType):
    def get_constant(self, *args, **kwds):
        raise NotImplementedError

    def get_dummy_value(self):
        return self.get_cpp_name() + "()"

    def get_getter(self, identifier):
        return "const " + self.get_cpp_name() + \
               "& get_%(identifier)s() const { return %(identifier)s; }" % locals()

    def get_in_declaration(self, identifier, default_value=None):
        assert default_value is None
        return "const " + self.get_cpp_name() + "& " + identifier

    def get_setter(self, identifier):
        const_reference_decl = "const " + self.get_cpp_name() + "& "
        return """void set_%(identifier)s( %(const_reference_decl)s %(identifier)s ) { this->%(identifier)s = %(identifier)s; }""" % locals()

    def get_temp_name(self):
        return self.get_cpp_name()



class CPPConstant(CConstant, CPPConstruct): pass


class CPPBooleanType(CBooleanType, CPPType):
    def get_cpp_name(self):
        return "bool"

    def get_default_value(self):
        return "false"

    def get_dummy_value(self):
        return "false"


class CPPBufferType(CBufferType, CPPType): pass


class CPPEnumeratedType(CEnumeratedType, CPPType):
    def get_default_value(self):
        return self.get_enumerators()[0].get_identifier()

    def get_dummy_value(self):
        return self.get_enumerators()[0].get_identifier()


class CPPExceptionType(CExceptionType, CPPType):
    def get_base_names(self):
        return ["std::exception"]

    def __repr__(self):
        struct_type = \
            CPPStructType(
                self.get_parent_construct(),
                self.get_name(),
                members=self.get_members()
            )

        accessors = lpad("\n\n", indent(INDENT_SPACES, struct_type.get_accessors()))
        base_names = self.get_cpp_base_names()
        constructors = indent(INDENT_SPACES, struct_type.get_constructors())
        destructor_statements = struct_type.get_destructor_statements()
        member_declarations = struct_type.get_member_declarations()
        name = str(self.get_name())

        return """\
class %(name)s%(base_names)s
{
public:%(constructors)s
  virtual ~%(name)s() throw() { %(destructor_statements)s }%(accessors)s%(member_declarations)s
};
""" % locals()


class CPPInclude(CInclude, CPPConstruct): pass


class CPPInterface(CInterface, CPPConstruct):
    def __repr__(self):
        base_names = self.get_cpp_base_names()
        name = self.get_name()
        constants = [repr(constant) for constant in self.get_constants()]
        constants = pad("\npublic:\n", indent(INDENT_SPACES, "\n".join(constants)), "\n")
        exception_types = [repr(exception_type) for exception_type in self.get_exception_types()]
        exception_types = pad("\npublic:\n", indent(INDENT_SPACES, "\n".join(exception_types)), "\n")

        if len(self.get_operations()) > 0:
            defines = [""]
            defines.append("// Use this macro in an implementation class to get all of the prototypes")
            defines.append("// for the operations in %(name)s" % locals())
            defines.append("#define %s_PROTOTYPES\\" % '_'.join(self.get_name()).upper())

            methods = ["", ""]

            for operation in self.get_operations():
                define = repr(operation) + ";"
                define = "\\\n".join(define.split('\n')) + "\\"
                defines.append(indent(INDENT_SPACES, define))
                method = operation.get_dummy_definition()
                method = indent(INDENT_SPACES, method)
                methods.append(method)
            defines = rpad("\n".join(defines), "\n")
            methods = "\n".join(methods)
        else:
            defines = methods = ""

        return """\
class %(name)s%(base_names)s
{%(constants)s%(exception_types)s
public:
  virtual ~%(name)s() { }%(methods)s
};

%(defines)s
""" % locals()


class CPPMapType(CMapType, CPPCompoundType):
    def get_base_names(self):
        key_type_cpp_name = self.get_key_type().get_cpp_name()
        value_type_cpp_name = self.get_value_type().get_cpp_name()
        return ["std::map<%(key_type_cpp_name)s, %(value_type_cpp_name)s>" % locals()]

    def get_includes(self):
        return ["<map>"]

    def get_other_methods(self):
        return ""

    def __repr__(self):
        base_names = self.get_cpp_base_names()
        name = str(self.get_name())
        other_methods = \
            lpad("\n\n", indent(INDENT_SPACES, self.get_other_methods()))
        return """\
class %(name)s%(base_names)s
{
public:
  virtual ~%(name)s() { }

  virtual bool operator==( const %(name)s& other ) const
  {
    if ( size() == other.size() )
    {
      return true;
    }
    else
      return false;
  }%(other_methods)s
};
""" % locals()


class CPPModule(CModule, CPPConstruct):
    def __repr__(self):
        return "namespace " + self.get_name() + \
               "\n{\n" + \
               rpad(indent(INDENT_SPACES, CModule.__repr__(self)), '\n') + \
               "}"


class CPPNumericType(CNumericType, CPPType): pass


class CPPOperation(COperation, CPPConstruct):
    def get_dummy_definition(self):
        prototype = repr(self)
        if self.get_return_type() is None:
            if len(prototype) <= 80:
                return prototype + " { }"
            else:
                return prototype + "\n{ }\n"
        else:
            return prototype + "{\n  return " + self.get_return_type().get_dummy_value() + ";\n}\n"

    def __repr__(self):
        prototype = "virtual "

        if self.is_static():
            prototype += "static "

        if self.get_return_type() is not None:
            prototype += self.get_return_type().get_return_name()
        else:
            prototype += "void"

        parameters = []
        for parameter in self.get_parameters():
            # Out parameter declarations usually don't have const,
            # so out has precedence over in for inout parameters
            if parameter.is_out():
                parameters.append(
                    parameter.get_type().get_out_declaration(
                        str(parameter.get_identifier())
                    )
                )
            else:
                parameters.append(
                    parameter.get_type().get_in_declaration(
                         str(parameter.get_identifier()),
                         parameter.get_value()
                    )
                )

        const = self.is_const() and " const" or ""

        test_prototype = prototype + " " + str(self.get_name()) + \
                         "(" + \
                          pad(" ", ", ".join(parameters), " ") + \
                         ")" + const
        if len(test_prototype) <= 80:
            return test_prototype
        else:
            return prototype + "\n" + self.get_name() + \
                    "\n(\n  " + \
                    ",\n  ".join(parameters) + \
                    "\n)"

class CPPOperationParameter(COperationParameter, CPPConstruct): pass


class CPPPointerType(CPointerType, CPPType):
    def get_cpp_name(self):
        if self.get_name() == "ptr" or self.get_name() == "any":
            return "void*"
        else:
            return "::".join(self.get_name())


class CPPReferenceType(CReferenceType, CPPType): pass


class CPPSequenceType(CSequenceType, CPPCompoundType):
    def get_base_names(self):
        value_type_cpp_name = self.get_value_type().get_cpp_name()
        return ["std::vector< %(value_type_cpp_name)s >" % locals()]

    def get_includes(self):
        return ["<vector>"]

    def get_other_methods(self):
        return ""

    def __repr__(self):
        base_names = self.get_cpp_base_names()
        name = str(self.get_name())
        other_methods = \
            lpad("\n\n", indent(INDENT_SPACES, self.get_other_methods()))
        value_type_cpp_name = self.get_value_type().get_cpp_name()
        value_in_constructor_declaration = self.get_value_type().get_in_declaration("first_value")
        return """\
class %(name)s%(base_names)s
{
public:
  %(name)s() { }

  %(name)s( %(value_in_constructor_declaration)s )
  {
    vector<%(value_type_cpp_name)s>::push_back( first_value );
  }

  %(name)s( size_type size )
    : vector<%(value_type_cpp_name)s>( size )
  { }

  virtual ~%(name)s() { }

  virtual bool operator==( const %(name)s& other ) const
  {
    if ( size() == other.size() )
    {
      size_t i_max = size();
      for ( size_t i = 0; i < i_max; i++ )
      {
        if ( !( ( *this )[i] == other[i] ) )
          return false;
      }

      return true;
    }
    else
      return false;
  }%(other_methods)s
};
""" % locals()


class CPPStringType(CStringType, CPPType):
    def get_cpp_name(self):
        return "std::string"

    def get_dummy_value(self):
        return "std::string()"

    def get_getter(self, identifier):
        return "const std::string& get_%(identifier)s() const { return %(identifier)s; }" % locals()

    def get_includes(self):
        return ["<string>"]

    def get_in_declaration(self, identifier, default_value=None):
        in_constructor_declaration = "const std::string& " + identifier
        if default_value is not None:
            in_constructor_declaration += " = " + str(default_value)
        return in_constructor_declaration

    def get_initializer(self, identifier):
        return identifier

    def get_out_declaration(self, identifier):
        return "std::string& " + identifier

    def get_return_name(self):
        return "std::string"

    def get_setter(self, identifier):
        return """void set_%(identifier)s( const std::string& %(identifier)s ) { this->%(identifier)s = %(identifier)s; }""" % locals()

    def get_temp_name(self):
        return "std::string"


class CPPStructType(CStructType, CPPCompoundType):
    def get_accessors(self):
        if len(self.get_members()) > 0:
            accessors = []
            for member in self.get_members():
                accessors.append(member.get_type().get_getter(member.get_identifier()))
            for member in self.get_members():
                accessors.append(member.get_type().get_setter(member.get_identifier()))
            return "\n".join(accessors)
        else:
            return ""

    def get_constructors(self):
        constructors = []

        if len(self.get_members()) > 0:
            have_reference_member = False
            for member in self.get_members():
                if isinstance(member.get_type(), CPPReferenceType):
                    have_reference_member = True
                    break

            if not have_reference_member:
                for member in self.get_members():
                    if member.get_value() is None:
                        constructors.append(self._get_default_constructor())
                        break

            constructors.append(self._get_full_constructor())
            constructors.append(self._get_copy_constructor())

        return "\n\n".join(constructors)

    def _get_copy_constructor(self):
        name = self.get_name()
        copy_constructor = "%(name)s( const %(name)s& other )" % locals()

        if len(self.get_members()) > 0:
            copy_constructor += "\n  : "
            initializers = \
            [
             member.get_identifier() + \
             "( " + \
              member.get_type().get_initializer
              (
                 "other.get_" + member.get_identifier() + "()"
              ) + \
              " )"
              for member in self.get_members()
            ]
            test_copy_constructor = copy_constructor + ", ".join(initializers)
            if len(test_copy_constructor) <= 80:
                copy_constructor = test_copy_constructor
            else:
                copy_constructor += (",\n" + INDENT_SPACES * 2).join(initializers)
            copy_constructor += "\n{ }"
        else:
            copy_constructor += " { }"

        return copy_constructor

    def _get_default_constructor(self):
        default_constructor = self.get_name() + "()"
        initializers = []
        for member in self.get_members():
            if member.get_value() is not None:
                initializers.append(
                    member.get_identifier() + \
                    "( " + member.get_value() + " )"
                )
            elif member.get_type().get_default_value() is not None:
                initializers.append(
                    member.get_identifier() + \
                    "( " + member.get_type().get_default_value() + " )"
                )

        if len(initializers) > 0:
            default_constructor += "\n  : "
            test_default_constructor = default_constructor + ", ".join(initializers)
            if len(test_default_constructor) <= 80:
                default_constructor = test_default_constructor
            else:
                default_constructor += (",\n" + INDENT_SPACES * 2).join(initializers)
            default_constructor += "\n{ }"
        else:
            default_constructor += " { }"

        return default_constructor

    def get_destructor_statements(self):
        return "".join(
                   [member.get_type().get_destructor_statement(member.get_identifier())
                    for member in self.get_members()]
                )

    def _get_full_constructor(self):
        constructor = self.get_name()

        if len(self.get_members()) > 0:
            decls = \
                [member.get_type().get_in_declaration(
                     member.get_identifier(),
                     member.get_value()
                 )
                 for member in self.get_members()]

            test_constructor = constructor + "( " + ", ".join(decls) + " )"
            if len(test_constructor) <= 80:
                constructor = test_constructor
            else:
                constructor += "\n(\n  " + ",\n  ".join(decls) + "\n)"

            constructor += "\n  : "
            initializers = \
                [member.get_identifier() + \
                 "( " + \
                 member.get_type().get_initializer(member.get_identifier()) + \
                 " )"
                 for member in self.get_members()]

            if len(", ".join(initializers)) <= 80:
                constructor += ", ".join(initializers)
            else:
                constructor += ",\n    ".join(initializers)
            constructor += "\n{ }"
        else:
            constructor += "() { }"

        return constructor

    def get_member_declarations(self):
        if len(self.get_members()) > 0:
            return "\n\nprotected:\n  " + \
                   "\n  ".join(
                       [member.get_type().get_cpp_name() + \
                        " " + \
                        member.get_identifier() + \
                        ";"
                        for member in self.get_members()]
                    )
        else:
            return ""

    def get_operator_equals(self):
        name = self.get_name()
        if len(self.get_members()) > 0:
            operator_equals = \
            [
              "get_" + member.get_identifier() + "()" + \
              " == " + \
              "other.get_" + member.get_identifier() + "()"
              for member in self.get_members()
            ]
            operator_equals = "\n         &&\n         ".join(operator_equals)
            return """\
bool operator==( const %(name)s& other ) const
{
  return %(operator_equals)s;
}
""" % locals()
        else:
            return "bool operator==( const %(name)s& ) const { return true; }" % locals()

    def get_other_methods(self):
        return ""

    def __repr__(self):
        if self.get_members() is not None:
            accessors = lpad("\n\n", indent(INDENT_SPACES, self.get_accessors()))
            base_names = self.get_cpp_base_names()
            constructors = indent(INDENT_SPACES, pad("\n", self.get_constructors(), "\n\n"))
            destructor_statements = self.get_destructor_statements()
            member_declarations = self.get_member_declarations()
            name = self.get_name()
            operator_equals = indent(INDENT_SPACES, self.get_operator_equals())
            other_methods = lpad("\n\n", indent(INDENT_SPACES, self.get_other_methods()))
            return """\
class %(name)s%(base_names)s
{
public:%(constructors)s
  virtual ~%(name)s() { %(destructor_statements)s }%(accessors)s

%(operator_equals)s%(other_methods)s%(member_declarations)s
};
""" % locals()
        else:
            return ""


class CPPTarget(CTarget): pass
