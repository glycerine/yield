// yield/marshal/xml/xml_unmarshaller.cpp

// Copyright (c) 2010 Minor Gordon
// All rights reserved

// This source file is part of the Yield project.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
// * Neither the name of the Yield project nor the
// names of its contributors may be used to endorse or promote products
// derived from this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL Minor Gordon BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#include "rapidxml.hpp"
#include "yield/assert.hpp"
#include "yield/buffer.hpp"
#include "yield/marshal/double.hpp"
#include "yield/marshal/integer.hpp"
#include "yield/marshal/null.hpp"
#include "yield/marshal/object.hpp"
#include "yield/marshal/sequence.hpp"
#include "yield/marshal/string.hpp"
#include "yield/marshal/string_literal.hpp"
#include "yield/marshal/xml/xml_unmarshaller.hpp"

#include <sstream> // For std::ostringstream


namespace yield
{
  namespace marshal
  {
    namespace xml
    {
      using rapidxml::xml_attribute;
      using rapidxml::xml_node;


      static void debug_assert_valid_node( const xml_node<>* node )
      {
        // node should have attributes or child nodes or neither, but not both

        debug_assert
        (
          node == NULL
          ||
          (
            node->first_attribute() == NULL
            &&
            node->first_node() == NULL
          )
          ||
          (
            node->first_attribute() != NULL
            &&
            node->first_node() == NULL
          )
          ||
          (
            node->first_attribute() == NULL
            &&
            node->first_node() != NULL
          )
        );
      }


      XMLUnmarshaller::XMLUnmarshaller( const Buffer& xml )
        : xml( xml, xml.size() )
      {
        init();
      }

      XMLUnmarshaller::XMLUnmarshaller( const char* xml, size_t xml_len )
        : xml( xml, xml_len )
      {
        init();
      }

      XMLUnmarshaller::XMLUnmarshaller( const xml_node<>& root_node )
        : root_node( &root_node )
      {
        doc = NULL;
        next_attribute = root_node.first_attribute();
        next_node = root_node.first_node();
      }

      XMLUnmarshaller::~XMLUnmarshaller()
      {
        delete doc;
      }

      void XMLUnmarshaller::init()
      {
        doc = new rapidxml::xml_document<>;
        doc->parse<0>( const_cast<char*>( xml.c_str() ) );
        root_node = doc;
        debug_assert_valid_node( root_node );
        next_attribute = root_node->first_attribute();
        next_node = root_node->first_node();
      }

      const xml_attribute<>*
      XMLUnmarshaller::read_attribute
      (
        const Object& key
      )
      {
        debug_assert_ne( root_node, NULL );

        const xml_attribute<>* attribute;

        switch ( key.get_type_id() )
        {
          case Double::TYPE_ID:
          {
            std::ostringstream attribute_name;
            attribute_name << "_" << static_cast<const Double&>( key );
            attribute
              = root_node->first_attribute
                (
                  attribute_name.str().data(),
                  attribute_name.str().size()
                );
          }
          break;

          case Null::TYPE_ID:
          {
            if ( this->next_attribute != NULL )
              attribute = this->next_attribute;
          }
          break;

          case String::TYPE_ID:
          {
            attribute
              = root_node->first_attribute
                (
                  static_cast<const String&>( key ).c_str(),
                  static_cast<const String&>( key ).size()
                );
          }
          break;

          case StringLiteral::TYPE_ID:
          {
            attribute
              = root_node->first_attribute
                (
                  static_cast<const StringLiteral&>( key ),
                  strlen( static_cast<const StringLiteral&>( key ) )
                );
          }
          break;

          default: DebugBreak(); return NULL;
        }

        if ( next_attribute != NULL )
          next_attribute = next_attribute->next_attribute();

        return attribute;
      }

      bool XMLUnmarshaller::read_bool( const Object& key )
      {
        const xml_attribute<>* attr = read_attribute( key );
        if ( attr != NULL )
          return strcmp( attr->value(), "true" ) == 0 ||
                 strcmp( attr->value(), "1" ) == 0;
        else
          return false;
      }

      double XMLUnmarshaller::read_double( const Object& key )
      {
        const xml_attribute<>* attr = read_attribute( key );
        if ( attr != NULL )
          return atof( attr->value() );
        else
          return 0.0;
      }

      int64_t XMLUnmarshaller::read_int64( const Object& key )
      {
        const xml_attribute<>* attr = read_attribute( key );
        if ( attr != NULL )
        {
          int64_t value;
          sscanf( attr->value(), "%lld", &value );
          return value;
        }
        else
          return 0;
      }

      void XMLUnmarshaller::read_key( Object& key )
      {
        const char* next_name;
        size_t next_name_size;

        if ( next_attribute != NULL )
        {
          next_name = next_attribute->name();
          next_name_size = next_attribute->name_size();
        }
        else
        {
          debug_assert_ne( next_node, NULL );
          next_name = next_node->name();
          next_name_size = next_node->name_size();
        }

        switch ( key.get_type_id() )
        {
          case Double::TYPE_ID:
          {
            debug_assert_eq( next_name[0], '_' );
            static_cast<Double&>( key ) = atof( next_name+1 );
          }
          break;

          case Integer::TYPE_ID:
          {
            int64_t int64_key;
            sscanf( next_name, "%lld", &int64_key );
            static_cast<Integer&>( key ) = int64_key;
          }
          break;

          case String::TYPE_ID:
          {
            static_cast<String&>( key ).assign( next_name, next_name_size );
          }
          break;

          default: DebugBreak(); break;
        }
      }

      void XMLUnmarshaller::read_map( const Object& key, Map& value )
      {
        const xml_node<>* node = read_node( key );
        if ( node != NULL )
        {
          XMLUnmarshaller child_xml_unmarshaller( *node );

          while
          (
            child_xml_unmarshaller.next_attribute != NULL
            ||
            child_xml_unmarshaller.next_node != NULL
          )
            value.unmarshal( child_xml_unmarshaller );
        }
      }

      const xml_node<>* XMLUnmarshaller::read_node( const Object& key )
      {
        debug_assert_ne( root_node, NULL );

        const xml_node<>* node;

        switch ( key.get_type_id() )
        {
          case Null::TYPE_ID: node = next_node; break;

          case String::TYPE_ID:
          {
            node = root_node->first_node
                   (
                     static_cast<const String&>( key ).c_str(),
                     static_cast<const String&>( key ).size()
                   );
          }
          break;

          case StringLiteral::TYPE_ID:
          {
            node = root_node->first_node
                   (
                     static_cast<const StringLiteral&>( key ),
                     strlen( static_cast<const StringLiteral&>( key ) )
                   );
          }
          break;

          default: DebugBreak(); return NULL;
        }

        debug_assert_valid_node( node );

        if ( next_node != NULL )
          next_node = next_node->next_sibling();

        return node;
      }

      void XMLUnmarshaller::read_object( const Object& key, Object& value )
      {
        const xml_node<>* node = read_node( key );
        if ( node != NULL )
        {
          XMLUnmarshaller child_xml_unmarshaller( *node );
          value.unmarshal( child_xml_unmarshaller );
        }
      }

      void XMLUnmarshaller::read_sequence( const Object& key, Sequence& value )
      {
        const xml_node<>* node = read_node( key );
        if ( node != NULL )
        {
          XMLUnmarshaller child_xml_unmarshaller( *node );

          while
          (
            child_xml_unmarshaller.next_attribute != NULL
            ||
            child_xml_unmarshaller.next_node != NULL
          )
            value.unmarshal( child_xml_unmarshaller );
        }
      }

      void XMLUnmarshaller::read_string( const Object& key, string& value )
      {
        const xml_attribute<>* attr = read_attribute( key );
        if ( attr != NULL )
          value = attr->value();
      }

      uint64_t XMLUnmarshaller::read_uint64( const Object& key )
      {
        const xml_attribute<>* attr = read_attribute( key );
        if ( attr != NULL )
        {
          uint64_t value;
          sscanf( attr->value(), "%llu", &value );
          return value;
        }
        else
          return 0;
      }
    }
  }
}
