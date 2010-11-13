// yield/marshal/json/json_unmarshaller.cpp

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


#include "object_map.hpp"
#include "object_vector.hpp"
#include "yield/assert.hpp"
#include "yield/marshal/boolean.hpp"
#include "yield/marshal/integer.hpp"
#include "yield/marshal/double.hpp"
#include "yield/marshal/null.hpp"
#include "yield/marshal/string.hpp"
#include "yield/marshal/json/json_decoder.hpp"
#include "yield/marshal/json/json_unmarshaller.hpp"

#include <stdlib.h> // For atof


namespace yield
{
  namespace marshal
  {
    namespace json
    {
      JSONUnmarshaller::JSONUnmarshaller( const Buffer& buffer )
      {
        next_object_i = 0;
        root_object = JSONDecoder().decode( buffer );
      }

      JSONUnmarshaller::JSONUnmarshaller( const char* json, size_t json_len )
      {
        next_object_i = 0;
        root_object = JSONDecoder().decode( json, json_len );
      }

      JSONUnmarshaller::JSONUnmarshaller( Object& root_object )
        : root_object( &root_object.inc_ref() )
      {
        next_object_i = 0;
      }

      JSONUnmarshaller::~JSONUnmarshaller()
      {
        Object::dec_ref( root_object );
      }

      Object* JSONUnmarshaller::read( const Object& key )
      {
        if ( root_object != NULL )
        {
          switch ( root_object->get_type_id() )
          {
            case ObjectMap::TYPE_ID:
            {
              if ( key.get_type_id() == Null::TYPE_ID )
                return root_object;
              else
              {
                ObjectMap& json_object = static_cast<ObjectMap&>( *root_object );

                for
                (
                  ObjectMap::const_iterator object_i = json_object.begin();
                  object_i != json_object.end();
                  ++object_i
                )
                {
                  if ( key == *object_i->first )
                  {
                    next_object_i++;
                    return object_i->second;
                  }
                }
              }
            }
            break;

            case ObjectVector::TYPE_ID:
            {
              debug_assert_eq( key.get_type_id(), Null::TYPE_ID );

              ObjectVector& json_array
                = static_cast<ObjectVector&>( *root_object );

              if ( next_object_i < json_array.size() )
                return json_array[next_object_i++];
              else
                return NULL;
            }
            break;

            default: return root_object;
          }
        }

        return NULL;
      }

      bool JSONUnmarshaller::read_bool( const Object& key )
      {
        Object* object = read( key );
        if ( object != NULL && object->get_type_id() == Boolean::TYPE_ID )
          return *static_cast<const Boolean*>( object );
        else
          return false;
      }

      double JSONUnmarshaller::read_double( const Object& key )
      {
        const Object* object = read( key );
        if ( object != NULL )
        {
          if ( object->get_type_id() == Double::TYPE_ID )
            return *static_cast<const Double*>( object );
          else if ( object->get_type_id() == Integer::TYPE_ID )
            return static_cast<double>( *static_cast<const Integer*>( object ) );
        }

        return 0;
      }

      int64_t JSONUnmarshaller::read_int64( const Object& key )
      {
        const Object* object = read( key );
        if ( object != NULL )
        {
          if ( object->get_type_id() == Double::TYPE_ID )
            return static_cast<int64_t>( *static_cast<const Double*>( object ) );
          else if ( object->get_type_id() == Integer::TYPE_ID )
            return *static_cast<const Integer*>( object );
        }

        return 0;
      }

      void JSONUnmarshaller::read_key( Object& key )
      {
        if ( root_object != NULL )
        {
          debug_assert_eq( root_object->get_type_id(), ObjectMap::TYPE_ID );
          ObjectMap& json_object = static_cast<ObjectMap&>( *root_object );
          debug_assert_lt( next_object_i, json_object.size() );
          ObjectMap::const_iterator json_object_i = json_object.begin();
          for ( size_t object_i = 0; object_i < next_object_i; object_i++ )
            ++json_object_i;
          debug_assert_ne( json_object_i, json_object.end() );
          debug_assert_ne( json_object_i->first, NULL );
          debug_assert_eq
          (
            json_object_i->first->get_type_id(),
            String::TYPE_ID
          );
          const String& json_key
            = static_cast<const String&>( *json_object_i->first );

          switch ( key.get_type_id() )
          {
            case Double::TYPE_ID:
            {
              static_cast<Double&>( key ) = atof( json_key.c_str() );
            }
            break;

            case Integer::TYPE_ID:
            {
              int64_t int64_key;
              sscanf( json_key.c_str(), "%lld", &int64_key );
              static_cast<Integer&>( key ) = int64_key;
            }
            break;

            case String::TYPE_ID:
            {
              static_cast<String&>( key ) = json_key;
            }
            break;

            default: DebugBreak(); break;
          }
        }
      }

      void JSONUnmarshaller::read_map( const Object& key, Map& value )
      {
        Object* object = read( key );
        if ( object != NULL && object->get_type_id() == ObjectMap::TYPE_ID )
        {
          JSONUnmarshaller child_json_unmarshaller( *object );
          size_t size = static_cast<ObjectMap&>( *object ).size();
          while ( child_json_unmarshaller.next_object_i < size )
            value.unmarshal( child_json_unmarshaller );
        }
      }

      void JSONUnmarshaller::read_object( const Object& key, Object& value )
      {
        Object* object = read( key );
        if ( object != NULL && object->get_type_id() == ObjectMap::TYPE_ID )
        {
          JSONUnmarshaller child_json_unmarshaller( *object );
          value.unmarshal( child_json_unmarshaller );
        }
      }

      void JSONUnmarshaller::read_sequence( const Object& key, Sequence& value )
      {
        Object* object = read( key );
        if ( object != NULL && object->get_type_id() == ObjectVector::TYPE_ID )
        {
          JSONUnmarshaller child_json_unmarshaller( *object );
          size_t size = static_cast<ObjectVector&>( *object ).size();
          while ( child_json_unmarshaller.next_object_i < size )
            value.unmarshal( child_json_unmarshaller );
        }
      }

      void JSONUnmarshaller::read_string( const Object& key, string& value )
      {
        Object* object = read( key );
        if ( object != NULL && object->get_type_id() == String::TYPE_ID )
          value.assign( *static_cast<String*>( object ) );
      }

      uint64_t JSONUnmarshaller::read_uint64( const Object& key )
      {
        const Object* object = read( key );
        if ( object != NULL )
        {
          if ( object->get_type_id() == Double::TYPE_ID )
            return static_cast<uint64_t>( *static_cast<const Double*>( object ) );
          else if ( object->get_type_id() == Integer::TYPE_ID )
            return *static_cast<const Integer*>( object );
        }

        return 0;
      }
    }
  }
}
