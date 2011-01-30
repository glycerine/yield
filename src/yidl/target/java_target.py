# yidl/target/java_target.py

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

import sys, os.path

from yutil import indent, lpad, pad, rpad, write_file

from yidl.target import *


__all__ = [
           "INDENT_SPACES",
           "JavaClass",
           "JavaBooleanType",
           "JavaBufferType",
           "JavaConstant",
           "JavaEnumeratedType", "JavaEnumerator",
           "JavaExceptionType",
           "JavaInterface",
           "JavaMapType",
           "JavaModule",
           "JavaNumericType",
           "JavaOperation",
           "JavaPointerType",
           "JavaSequenceType",
           "JavaStringType",
           "JavaStructType",
           "JavaTarget",
          ]


# Constants
INDENT_SPACES = ' ' * 4


class JavaClass:
    def get_class_header(self):
        package_name = self.get_package_name()
        imports = list(set(self.get_imports()))
        imports.sort()
        imports = pad("\n\n", "\n".join(imports), "\n\n\n")
        name = self.get_name()
        try: parent_names = self.get_parent_names()
        except AttributeError: parent_names = self.get_parent_interface_names()
        assert isinstance(parent_names, tuple) or isinstance(parent_names, list)
        if len(parent_names) > 0:
            parents = []
            if parent_names[0] is not None:
                parents.append("extends " + parent_names[0])
            if len(parent_names) > 1:
                parents.extend(["implements " + parent_name for parent_name in parent_names[1:]])
            parents = lpad(" ", " ".join(parents))
        else:
            parents = ""

        return """\
package %(package_name)s;%(imports)s

public class %(name)s%(parents)s
{""" % locals()

    def get_class_footer(self):
        return "}"

    def get_file_path(self):
        return os.path.join(self.get_package_dir_path(), self.get_name() + ".java")

    def get_imports(self):
        imports = []
        package_name_parts = self.get_package_name().split(".")
        if len(package_name_parts) > 2:
            for package_name_part_i in xrange(2, len(package_name_parts)):
                imports.append("import " + ".".join(package_name_parts[:package_name_part_i]) + ".*;")
        return imports

    def get_package_dir_path(self):
        return os.sep.join(self.get_qname()[:-1])

    def get_package_name(self):
        return ".".join(self.get_qname()[:-1])


class JavaPackage:
    def __init__(self):
        try: os.makedirs(self.get_package_dir_path())
        except: pass

    def get_package_dir_path(self): return os.sep.join(self.get_qname())
    def get_package_name(self): return ".".join(self.get_qname())


class JavaType:
    def get_boxed_name(self): return self.get_java_name()
    def get_default_initializer(self, identifier): return ""
    def get_constant_value(self, value): return value
    def get_java_name(self): return self.get_name()

    def get_getter(self, identifier):
        getter = [self.get_java_name()]
        getter.append(" get")
        getter.append(identifier[0].capitalize())
        getter.append(identifier[1:])
        getter.append("() { return %(identifier)s; }" % locals())
        return "".join(getter)

    def get_imports(self): return []

    def get_marshal_call(self, decl_identifier, value_identifier):
        marshaller_name = self.get_marshaller_name()
        return """marshaller.write%(marshaller_name)s( \"%(decl_identifier)s\", %(value_identifier)s );""" % locals()

    def get_setter(self, identifier):
        setter = ["void set"]
        setter.append(identifier[0].capitalize())
        setter.append(identifier[1:])
        setter.append("( ")
        setter.append(self.get_java_name())
        setter.append(" %(identifier)s ) { this.%(identifier)s = %(identifier)s; }" % locals())
        return "".join(setter)

    def get_unmarshal_call(self, decl_identifier, value_identifier):
        marshaller_name = self.get_marshaller_name()
        return value_identifier + """ = unmarshaller.read%(marshaller_name)s( %(decl_identifier)s );""" % locals()


class JavaBooleanType(BooleanType, JavaType):
    def get_boxed_name(self): return "Boolean"
    def get_java_name(self): return "boolean"
    def get_xdr_size(self, identifier): return "Integer.SIZE / 8"


class JavaBufferType(BufferType, JavaType):
    def get_constant_value(self, value):
        return "\"%(value)s\"" % locals()

    def get_imports(self):
        return JavaType.get_imports(self) + ["java.nio.ByteBuffer"]

    def get_xdr_size(self, identifier):
        return "Integer.SIZE / 8 + ( %(identifier)s != null ? ( ( %(identifier)s.remaining() %% 4 == 0 ) ? %(identifier)s.remaining() : ( %(identifier)s.remaining() + 4 - %(identifier)s.remaining() %% 4 ) ) : 0 )" % locals()


class JavaCompoundType(JavaClass, JavaType):
    def get_default_initializer(self, identifier):
        name = self.get_name()
        return "%(identifier)s = new %(name)s(); " % locals()

    def get_imports(self):
        return JavaClass.get_imports(self) + JavaType.get_imports(self) + \
               ["import yidl.runtime.Marshaller;", "import yidl.runtime.Unmarshaller;"]

    def get_other_methods(self): return ""

    def get_unmarshal_call(self, decl_identifier, value_identifier):
         name = self.get_name()
         marshaller_name = self.get_marshaller_name()
         return "%(value_identifier)s = new %(name)s(); unmarshaller.read%(marshaller_name)s( %(decl_identifier)s, %(value_identifier)s );""" % locals()

    def get_xdr_size(self, identifier):
        return "%(identifier)s.getXDRSize()" % locals()


class JavaConstant(Constant):
    def __repr__(self):
        return "public static final " + self.get_type().get_java_name() + " " + \
            self.get_identifier() + " = " + self.get_type().get_constant_value(self.get_value()) + ';'


class JavaEnumeratedType(EnumeratedType, JavaCompoundType):
    def generate(self):
        package_name = self.get_package_name()
        name = self.get_name()
        enumerators = indent(INDENT_SPACES, pad("\n", ",\n".join([repr(enumerator) for enumerator in self.get_enumerators()]), ";"))
        write_file(self.get_file_path(), """\
package %(package_name)s;


public enum %(name)s
{%(enumerators)s

    private int __value;

    %(name)s() { this.__value = 0; }
    %(name)s( int value ) { this.__value = value; }
    public int intValue() { return __value; }

    public static %(name)s parseInt( int value )
    {
        %(name)s check_values[] = %(name)s.values();
        for ( int check_value_i = 0; check_value_i < check_values.length; check_value_i++ )
        {
            if ( check_values[check_value_i].intValue() == value )
                return check_values[check_value_i];
        }
        return null;
    }
}
""" % locals())

    def get_default_initializer(self, identifier):
        if len(self.get_enumerators()) > 0:
            return identifier + " = " + self.get_name() + "." + self.get_enumerators()[0].get_identifier() + "; "
        else:
            return ""

    def get_marshal_call(self, decl_identifier, value_identifier):
        return "marshaller.writeInt32( %(decl_identifier)s, %(value_identifier)s.intValue() );" % locals()

    def get_unmarshal_call(self, decl_identifier, value_identifier):
        name = self.get_name()
        return "%(value_identifier)s = %(name)s.parseInt( unmarshaller.readInt32( %(decl_identifier)s ) );" % locals()

    def get_xdr_size(self, identifier):
        return "Integer.SIZE / 8"


class JavaEnumerator(Enumerator):
    def __repr__(self):
        if self.get_value() is not None:
            return self.get_identifier() + "( " + str(self.get_value()) + " )"
        else:
            return self.get_identifier()


class JavaExceptionType(ExceptionType):
    def generate(self):
        JavaStructType(self.getScope(), self.get_qname(), self.get_tag(), ("Exception",), self.get_members()).generate()


class JavaInclude(Include): pass


class JavaInterface(Interface, JavaClass):
    def generate(self):
        package_name = self.get_package_name()
        imports = pad("\n\n", "\n".join(self.get_imports()), "\n")
        name = self.get_name()
        operations = "".join([pad(INDENT_SPACES, repr(operation), "\n") for operation in self.get_operations()])

        write_file(os.path.join(self.get_package_dir_path(), name + ".java"), """\
package %(package_name)s;%(imports)s

public interface %(name)s
{
%(operations)s
}
""" % locals())



class JavaMapType(MapType, JavaCompoundType):
    def generate(self):
        class_header = self.get_class_header()
        name = self.get_name()
        qname = self.get_qname("::")
        tag = self.get_tag()
        key_type_boxed_name = self.get_key_type().get_boxed_name()
        key_type_marshaller_name = self.get_key_type().get_marshaller_name()
        key_xdr_size = self.get_key_type().get_xdr_size("key")
        value_type_boxed_name = self.get_value_type().get_boxed_name()
        value_type_marshaller_name = self.get_value_type().get_marshaller_name()
        value_unmarshal_call = self.get_value_type().get_unmarshal_call("key", "value")
        value_xdr_size = self.get_value_type().get_xdr_size("value")
        other_methods = lpad("\n\n", indent(INDENT_SPACES, self.get_other_methods()))
        class_footer = self.get_class_footer()

        write_file(self.get_file_path(), """\
%(class_header)s
    public %(name)s() { }%(other_methods)s

    // yidl.runtime.Object
    public int getTag() { return %(tag)s; }
    public String getTypeName() { return "%(qname)s"; }

    public int getXDRSize()
    {
        int my_size = 4; // The number of keys

        for ( Iterator<%(key_type_boxed_name)s> key_i = keySet().iterator(); key_i.hasNext(); )
        {
            %(key_type_boxed_name)s key = key_i.next();
            my_size += %(key_xdr_size)s; // Size of the key

            %(value_type_boxed_name)s value = get( key );
            my_size += %(value_xdr_size)s; // Size of the value
        }

        return my_size;
    }

    public void marshal( Marshaller marshaller )
    {
        for ( Iterator<%(key_type_boxed_name)s> key_i = keySet().iterator(); key_i.hasNext(); )
        {
             %(key_type_boxed_name)s key = key_i.next();
             %(value_type_boxed_name)s value = get( key );
             marshaller.write%(value_type_marshaller_name)s( key, value );
        }
    }

    public void unmarshal( Unmarshaller unmarshaller )
    {
        %(key_type_boxed_name)s key = unmarshaller.read%(key_type_marshaller_name)s( null );
        if ( key != null )
        {
            %(value_type_boxed_name)s value;
            %(value_unmarshal_call)s
            put( key, value );
        }
    }
%(class_footer)s
""" % locals())

    def get_imports(self):
        return JavaCompoundType.get_imports(self) + \
               ["import yidl.runtime.Map;", "import java.util.Iterator;" ] + \
               self.get_key_type().get_imports() + \
               self.get_value_type().get_imports()

    def get_parent_names(self):
        key_type_boxed_name = self.get_key_type().get_boxed_name()
        value_type_boxed_name = self.get_value_type().get_boxed_name()
        return ("Map<%(key_type_boxed_name)s, %(value_type_boxed_name)s>" % locals(),)


class JavaModule(Module, JavaPackage):
    def __init__(self, *args, **kwds):
        Module.__init__(self, *args, **kwds)
        JavaPackage.__init__(self)

    def generate(self):
        Module.generate(self)

        if len(self.get_constants()) > 0:
            constants = indent(INDENT_SPACES, "\n".join([repr(constant) for constant in self.get_constants()]))
            package_name = self.get_package_name()
            write_file(os.path.join(self.get_package_dir_path(), "Constants.java"), """\
package %(package_name)s;


public interface Constants
{
    %(constants)s
};
""" % locals())

        for enumerated_type in self.get_enumerated_types():
            enumerated_type.generate()


class JavaNumericType(NumericType, JavaType):
    def get_boxed_name(self):
        decl_type = self.get_java_name()
        if decl_type == "int": return "Integer"
        else: return decl_type[0].upper() + decl_type[1:]

    def get_java_name(self):
        name = self.get_name()
        if name == "float" or name == "double": return name
        elif name.endswith("int8_t"): return "int"
        elif name.endswith("int16_t"): return "int"
        elif name.endswith("int32_t"): return "int"
        elif name.endswith("int64_t"): return "long"
        else: return "long" # mode_t, etc.

    def get_xdr_size(self, identifier):
        return self.get_boxed_name() + ".SIZE / 8"


class JavaOperation(Operation):
    def __repr__(self):
        if not self.isStatic():
            if self.get_return_type() is not None:
                return_type = self.get_return_type().get_java_name()
            else:
                return_type = "void"
            name = self.get_name()
            param_decls = []
            for param in self.getParameters():
                assert not param.isOutbound()
                param_decls.append(param.get_type().get_java_name() + " " + param.get_identifier())
            param_decls = pad(" ", ", ".join(param_decls), " ")
            return "public %(return_type)s %(name)s(%(param_decls)s);" % locals()
        else:
            return ""


class JavaPointerType(PointerType, JavaType):
    def get_java_name(self):
        name = self.get_name()
        if name.endswith(" *"):
            return name[:-2]
        else:
            return name


class JavaSequenceType(SequenceType, JavaCompoundType):
    def generate(self):
        class_header = self.get_class_header()
        name = self.get_name()
        qname = self.get_qname("::")
        tag = self.get_tag()
        value_type_boxed_name = self.get_value_type().get_boxed_name()
        value_type_java_name = self.get_value_type().get_java_name()
        value_marshal_call = self.get_value_type().get_marshal_call("value", "i.next()")
        value_unmarshal_call = self.get_value_type().get_unmarshal_call("\"value\"", "value")
        value_xdr_size = self.get_value_type().get_xdr_size("value")
        other_methods = lpad("\n\n", indent(INDENT_SPACES, self.get_other_methods()))
        class_footer = self.get_class_footer()
        write_file(self.get_file_path(), """\
%(class_header)s
    public %(name)s() { }%(other_methods)s

    // yidl.runtime.Object
    public int getTag() { return %(tag)s; }
    public String getTypeName() { return "%(qname)s"; }

    public int getXDRSize()
    {
        int my_size = 4; // Length of the sequence

        for ( Iterator<%(value_type_boxed_name)s> i = iterator(); i.hasNext(); )
        {
            %(value_type_boxed_name)s value = i.next();
            my_size += %(value_xdr_size)s; // Size of value
        }

        return my_size;
    }

    public void marshal( Marshaller marshaller )
    {
        for ( Iterator<%(value_type_boxed_name)s> i = iterator(); i.hasNext(); )
            %(value_marshal_call)s;
    }

    public void unmarshal( Unmarshaller unmarshaller )
    {
        %(value_type_java_name)s value;
        %(value_unmarshal_call)s
        this.add( value );
    }

%(class_footer)s
""" % locals())

    def get_imports(self):
        return JavaCompoundType.get_imports(self) + \
               ["import yidl.runtime.Sequence;", "import java.util.Iterator;"] + \
               self.get_value_type().get_imports()

    def get_parent_names(self):
        return ("Sequence<%s>" % self.get_value_type().get_boxed_name(),)


class JavaStringType(StringType, JavaType):
    def get_constant_value(self, value): return "\"%(value)s\"" % locals()
    def get_java_name(self): return "String"

    def get_xdr_size(self, identifier):
        return "Integer.SIZE / 8 + ( %(identifier)s != null ? ( ( %(identifier)s.getBytes().length %% 4 == 0 ) ? %(identifier)s.getBytes().length : ( %(identifier)s.getBytes().length + 4 - %(identifier)s.getBytes().length %% 4 ) ) : 0 )" % locals()


class JavaStructType(StructType, JavaCompoundType):
    def generate(self):
        class_header = self.get_class_header()
        name = self.get_name()
        qname = self.get_qname("::")
        tag = self.get_tag()
        constructors = []
        if self.get_members() is not None:
            default_initializers = "".join([member.get_type().get_default_initializer(member.get_identifier()) for member in self.get_members()])

            if len(self.get_members()) > 0:
                constructor = ["public " + self.get_name() + "( "]
                constructor.append(", ".join([member.get_type().get_java_name() + " " + member.get_identifier() for member in self.get_members()]))
                constructor.append(" ) { ")
                constructor.append(" ".join(["this." + member.get_identifier() + " = " + member.get_identifier() + ";" for member in self.get_members()]))
                constructor.append(" }")
                constructors.append("".join(constructor))

            getters = ["public " + member.get_type().get_getter(member.get_identifier()) for member in self.get_members()]
            setters = ["public " + member.get_type().get_setter(member.get_identifier()) for member in self.get_members()]
            accessors = indent(INDENT_SPACES, lpad("\n\n", "\n".join(getters + setters)))
            my_size = indent(INDENT_SPACES * 2, "\n".join(["my_size += " + member.get_type().get_xdr_size(member.get_identifier()) + "; // " + member.get_identifier() for member in self.get_members()]))
            marshal_calls = indent(INDENT_SPACES * 2, "\n".join([member.get_type().get_marshal_call(member.get_identifier(), member.get_identifier()) for member in self.get_members()]))
            unmarshal_calls = indent(INDENT_SPACES * 2, "\n".join([member.get_type().get_unmarshal_call("\"" + member.get_identifier() + "\"", member.get_identifier()) for member in self.get_members()]))
            declarations = indent(INDENT_SPACES, lpad("\n\n", "\n".join(["private " + member.get_type().get_java_name() + " " + member.get_identifier() + ";" for member in self.get_members()])))
        else:
            default_initializers = accessors = my_size = marshal_calls = unmarshal_calls = declarations = ""

        constructors.insert(0, "public %(name)s() { %(default_initializers)s }" % locals())
        constructors = indent(INDENT_SPACES, "\n".join(constructors))

        other_methods = lpad("\n\n", indent(INDENT_SPACES, self.get_other_methods()))

        class_footer = self.get_class_footer()

        write_file(self.get_file_path(), """\
%(class_header)s
    public static final int TAG = %(tag)s;

%(constructors)s%(accessors)s%(other_methods)s

    // java.io.Serializable
    public static final long serialVersionUID = %(tag)s;

    // yidl.runtime.Object
    public int getTag() { return %(tag)s; }
    public String getTypeName() { return "%(qname)s"; }

    public int getXDRSize()
    {
        int my_size = 0;
%(my_size)s
        return my_size;
    }

    public void marshal( Marshaller marshaller )
    {
%(marshal_calls)s
    }

    public void unmarshal( Unmarshaller unmarshaller )
    {
%(unmarshal_calls)s
    }%(declarations)s
%(class_footer)s
""" % locals())

    def get_imports(self):
        return JavaCompoundType.get_imports(self) + ["import yidl.runtime.Struct;"]

    def get_parent_names(self):
        parent_names = StructType.get_parent_names(self)
        if len(parent_names) == 0:
            parent_names = (None, "Struct")
        return parent_names


class JavaTarget(Target): pass
