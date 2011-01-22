// yield/marshal/marshaller_test_types.hpp

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


#ifndef _YIELD_MARSHAL_MARSHALLER_TEST_TYPES_HPP_
#define _YIELD_MARSHAL_MARSHALLER_TEST_TYPES_HPP_


#include "yield/marshal/double.hpp"
#include "yield/marshal/integer.hpp"
#include "yield/marshal/map.hpp"
#include "yield/marshal/marshaller.hpp"
#include "yield/marshal/null.hpp"
#include "yield/marshal/object.hpp"
#include "yield/marshal/sequence.hpp"
#include "yield/marshal/string.hpp"
#include "yield/marshal/string_literal.hpp"
#include "yield/marshal/unmarshaller.hpp"
#include <map>
#include <string>





namespace yield {
namespace marshal {
class BooleanStruct : public ::yield::marshal::Object {
public:
  const static uint32_t TYPE_ID = 6UL;

public:
  BooleanStruct()
    : value( false )
  { }

  BooleanStruct( bool value )
    : value( value )
  { }

  BooleanStruct( const BooleanStruct& other )
    : value( other.get_value() )
  { }

  virtual ~BooleanStruct() {  }

  bool get_value() const {
    return value;
  }
  void set_value( bool value ) {
    this->value = value;
  }

  bool operator==( const BooleanStruct& other ) const {
    return get_value() == other.get_value();
  }

  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }
  const char* get_type_name() const {
    return "BooleanStruct";
  }
  BooleanStruct& inc_ref() {
    return ::yield::Object::inc_ref( *this );
  }

  void marshal( ::yield::marshal::Marshaller& marshaller ) const {
    marshaller.write( ::yield::marshal::StringLiteral( "value" ), get_value() );
  }

  void unmarshal( ::yield::marshal::Unmarshaller& unmarshaller ) {
    value = unmarshaller.read_bool( ::yield::marshal::StringLiteral( "value" ) );
  }

protected:
  bool value;
};


class DoubleMap : public ::yield::marshal::Map, public std::map< ::yield::marshal::Double, std::string > {
public:
  virtual ~DoubleMap() { }

  virtual bool operator==( const DoubleMap& other ) const {
    if ( size() == other.size() ) {
      return true;
    } else
      return false;
  }

  // yield::marshal::Object
  const static uint32_t TYPE_ID = static_cast<uint32_t>( 7 );
  uint32_t get_type_id() const {
    return TYPE_ID;
  }
  const char* get_type_name() const {
    return "DoubleMap";
  }
  DoubleMap& inc_ref() {
    return ::yield::Object::inc_ref( *this );
  }

  void marshal( ::yield::marshal::Marshaller& marshaller ) const {
    for ( const_iterator i = begin(); i != end(); i++ ) {
      marshaller.write( i->first, i->second );
    }
  }

  void unmarshal( ::yield::marshal::Unmarshaller& unmarshaller ) {
    ::yield::marshal::Double key;
    unmarshaller.read_key( key );
    std::string value;
    unmarshaller.read_string( key, value );
    ( *this )[key] = value;
  }

  // yield::marshal::Map
  size_t get_size() const {
    return size();
  }
};


class DoubleMapStruct : public ::yield::marshal::Object {
public:
  const static uint32_t TYPE_ID = 8UL;

public:
  DoubleMapStruct() { }

  DoubleMapStruct( const yield::marshal::DoubleMap& value )
    : value( value )
  { }

  DoubleMapStruct( const DoubleMapStruct& other )
    : value( other.get_value() )
  { }

  virtual ~DoubleMapStruct() {  }

  const yield::marshal::DoubleMap& get_value() const {
    return value;
  }
  void set_value( const yield::marshal::DoubleMap&  value ) {
    this->value = value;
  }

  bool operator==( const DoubleMapStruct& other ) const {
    return get_value() == other.get_value();
  }

  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }
  const char* get_type_name() const {
    return "DoubleMapStruct";
  }
  DoubleMapStruct& inc_ref() {
    return ::yield::Object::inc_ref( *this );
  }

  void marshal( ::yield::marshal::Marshaller& marshaller ) const {
    marshaller.write( ::yield::marshal::StringLiteral( "value" ), get_value() );
  }

  void unmarshal( ::yield::marshal::Unmarshaller& unmarshaller ) {
    unmarshaller.read_map( ::yield::marshal::StringLiteral( "value" ), value );
  }

protected:
  yield::marshal::DoubleMap value;
};


class DoubleSet : public ::yield::marshal::Sequence, public std::vector< double > {
public:
  DoubleSet() { }

  DoubleSet( double first_value ) {
    vector<double>::push_back( first_value );
  }

  DoubleSet( size_type size )
    : vector<double>( size )
  { }

  virtual ~DoubleSet() { }

  virtual bool operator==( const DoubleSet& other ) const {
    if ( size() == other.size() ) {
      size_t i_max = size();
      for ( size_t i = 0; i < i_max; i++ ) {
        if ( !( ( *this )[i] == other[i] ) )
          return false;
      }

      return true;
    } else
      return false;
  }

  // yield::Object
  const static uint32_t TYPE_ID = static_cast<uint32_t>( 9 );
  uint32_t get_type_id() const {
    return TYPE_ID;
  }
  const char* get_type_name() const {
    return "DoubleSet";
  }
  DoubleSet& inc_ref() {
    return ::yield::Object::inc_ref( *this );
  }

  // yield::marshal::Object
  void marshal( ::yield::marshal::Marshaller& marshaller ) const {
    size_type value_i_max = size();
    for ( size_type value_i = 0; value_i < value_i_max; value_i++ ) {
      marshaller.write( ::yield::marshal::Null(), ( *this )[value_i] );
    }
  }

  void unmarshal( ::yield::marshal::Unmarshaller& unmarshaller ) {
    double value;
    value = unmarshaller.read_double( ::yield::marshal::Null() );
    push_back( value );
  }

  // yield::marshal::Sequence
  size_t get_size() const {
    return size();
  }
};


class DoubleSetStruct : public ::yield::marshal::Object {
public:
  const static uint32_t TYPE_ID = 10UL;

public:
  DoubleSetStruct() { }

  DoubleSetStruct( const yield::marshal::DoubleSet& value )
    : value( value )
  { }

  DoubleSetStruct( const DoubleSetStruct& other )
    : value( other.get_value() )
  { }

  virtual ~DoubleSetStruct() {  }

  const yield::marshal::DoubleSet& get_value() const {
    return value;
  }
  void set_value( const yield::marshal::DoubleSet&  value ) {
    this->value = value;
  }

  bool operator==( const DoubleSetStruct& other ) const {
    return get_value() == other.get_value();
  }

  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }
  const char* get_type_name() const {
    return "DoubleSetStruct";
  }
  DoubleSetStruct& inc_ref() {
    return ::yield::Object::inc_ref( *this );
  }

  void marshal( ::yield::marshal::Marshaller& marshaller ) const {
    marshaller.write( ::yield::marshal::StringLiteral( "value" ), get_value() );
  }

  void unmarshal( ::yield::marshal::Unmarshaller& unmarshaller ) {
    unmarshaller.read_sequence( ::yield::marshal::StringLiteral( "value" ), value );
  }

protected:
  yield::marshal::DoubleSet value;
};


class DoubleStruct : public ::yield::marshal::Object {
public:
  const static uint32_t TYPE_ID = 11UL;

public:
  DoubleStruct()
    : value( 0 )
  { }

  DoubleStruct( double value )
    : value( value )
  { }

  DoubleStruct( const DoubleStruct& other )
    : value( other.get_value() )
  { }

  virtual ~DoubleStruct() {  }

  double get_value() const {
    return value;
  }
  void set_value( double value ) {
    this->value = value;
  }

  bool operator==( const DoubleStruct& other ) const {
    return get_value() == other.get_value();
  }

  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }
  const char* get_type_name() const {
    return "DoubleStruct";
  }
  DoubleStruct& inc_ref() {
    return ::yield::Object::inc_ref( *this );
  }

  void marshal( ::yield::marshal::Marshaller& marshaller ) const {
    marshaller.write( ::yield::marshal::StringLiteral( "value" ), get_value() );
  }

  void unmarshal( ::yield::marshal::Unmarshaller& unmarshaller ) {
    value = unmarshaller.read_double( ::yield::marshal::StringLiteral( "value" ) );
  }

protected:
  double value;
};


class EmptyStruct : public ::yield::marshal::Object {
public:
  const static uint32_t TYPE_ID = 12UL;

public:
  virtual ~EmptyStruct() {  }

  bool operator==( const EmptyStruct& ) const {
    return true;
  }

  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }
  const char* get_type_name() const {
    return "EmptyStruct";
  }
  EmptyStruct& inc_ref() {
    return ::yield::Object::inc_ref( *this );
  }

  void marshal( ::yield::marshal::Marshaller& ) const { }
  void unmarshal( ::yield::marshal::Unmarshaller& ) { }
};


class FloatStruct : public ::yield::marshal::Object {
public:
  const static uint32_t TYPE_ID = 13UL;

public:
  FloatStruct()
    : value( 0 )
  { }

  FloatStruct( float value )
    : value( value )
  { }

  FloatStruct( const FloatStruct& other )
    : value( other.get_value() )
  { }

  virtual ~FloatStruct() {  }

  float get_value() const {
    return value;
  }
  void set_value( float value ) {
    this->value = value;
  }

  bool operator==( const FloatStruct& other ) const {
    return get_value() == other.get_value();
  }

  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }
  const char* get_type_name() const {
    return "FloatStruct";
  }
  FloatStruct& inc_ref() {
    return ::yield::Object::inc_ref( *this );
  }

  void marshal( ::yield::marshal::Marshaller& marshaller ) const {
    marshaller.write( ::yield::marshal::StringLiteral( "value" ), get_value() );
  }

  void unmarshal( ::yield::marshal::Unmarshaller& unmarshaller ) {
    value = unmarshaller.read_float( ::yield::marshal::StringLiteral( "value" ) );
  }

protected:
  float value;
};


class Int8Struct : public ::yield::marshal::Object {
public:
  const static uint32_t TYPE_ID = 14UL;

public:
  Int8Struct()
    : value( 0 )
  { }

  Int8Struct( int8_t value )
    : value( value )
  { }

  Int8Struct( const Int8Struct& other )
    : value( other.get_value() )
  { }

  virtual ~Int8Struct() {  }

  int8_t get_value() const {
    return value;
  }
  void set_value( int8_t value ) {
    this->value = value;
  }

  bool operator==( const Int8Struct& other ) const {
    return get_value() == other.get_value();
  }

  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }
  const char* get_type_name() const {
    return "Int8Struct";
  }
  Int8Struct& inc_ref() {
    return ::yield::Object::inc_ref( *this );
  }

  void marshal( ::yield::marshal::Marshaller& marshaller ) const {
    marshaller.write( ::yield::marshal::StringLiteral( "value" ), get_value() );
  }

  void unmarshal( ::yield::marshal::Unmarshaller& unmarshaller ) {
    value = unmarshaller.read_int8( ::yield::marshal::StringLiteral( "value" ) );
  }

protected:
  int8_t value;
};


class Int16Struct : public ::yield::marshal::Object {
public:
  const static uint32_t TYPE_ID = 15UL;

public:
  Int16Struct()
    : value( 0 )
  { }

  Int16Struct( int16_t value )
    : value( value )
  { }

  Int16Struct( const Int16Struct& other )
    : value( other.get_value() )
  { }

  virtual ~Int16Struct() {  }

  int16_t get_value() const {
    return value;
  }
  void set_value( int16_t value ) {
    this->value = value;
  }

  bool operator==( const Int16Struct& other ) const {
    return get_value() == other.get_value();
  }

  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }
  const char* get_type_name() const {
    return "Int16Struct";
  }
  Int16Struct& inc_ref() {
    return ::yield::Object::inc_ref( *this );
  }

  void marshal( ::yield::marshal::Marshaller& marshaller ) const {
    marshaller.write( ::yield::marshal::StringLiteral( "value" ), get_value() );
  }

  void unmarshal( ::yield::marshal::Unmarshaller& unmarshaller ) {
    value = unmarshaller.read_int16( ::yield::marshal::StringLiteral( "value" ) );
  }

protected:
  int16_t value;
};


class Int32Struct : public ::yield::marshal::Object {
public:
  const static uint32_t TYPE_ID = 16UL;

public:
  Int32Struct()
    : value( 0 )
  { }

  Int32Struct( int32_t value )
    : value( value )
  { }

  Int32Struct( const Int32Struct& other )
    : value( other.get_value() )
  { }

  virtual ~Int32Struct() {  }

  int32_t get_value() const {
    return value;
  }
  void set_value( int32_t value ) {
    this->value = value;
  }

  bool operator==( const Int32Struct& other ) const {
    return get_value() == other.get_value();
  }

  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }
  const char* get_type_name() const {
    return "Int32Struct";
  }
  Int32Struct& inc_ref() {
    return ::yield::Object::inc_ref( *this );
  }

  void marshal( ::yield::marshal::Marshaller& marshaller ) const {
    marshaller.write( ::yield::marshal::StringLiteral( "value" ), get_value() );
  }

  void unmarshal( ::yield::marshal::Unmarshaller& unmarshaller ) {
    value = unmarshaller.read_int32( ::yield::marshal::StringLiteral( "value" ) );
  }

protected:
  int32_t value;
};


class Int64Struct : public ::yield::marshal::Object {
public:
  const static uint32_t TYPE_ID = 17UL;

public:
  Int64Struct()
    : value( 0 )
  { }

  Int64Struct( int64_t value )
    : value( value )
  { }

  Int64Struct( const Int64Struct& other )
    : value( other.get_value() )
  { }

  virtual ~Int64Struct() {  }

  int64_t get_value() const {
    return value;
  }
  void set_value( int64_t value ) {
    this->value = value;
  }

  bool operator==( const Int64Struct& other ) const {
    return get_value() == other.get_value();
  }

  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }
  const char* get_type_name() const {
    return "Int64Struct";
  }
  Int64Struct& inc_ref() {
    return ::yield::Object::inc_ref( *this );
  }

  void marshal( ::yield::marshal::Marshaller& marshaller ) const {
    marshaller.write( ::yield::marshal::StringLiteral( "value" ), get_value() );
  }

  void unmarshal( ::yield::marshal::Unmarshaller& unmarshaller ) {
    value = unmarshaller.read_int64( ::yield::marshal::StringLiteral( "value" ) );
  }

protected:
  int64_t value;
};


class StringMap : public ::yield::marshal::Map, public std::map< ::yield::marshal::String, std::string > {
public:
  virtual ~StringMap() { }

  virtual bool operator==( const StringMap& other ) const {
    if ( size() == other.size() ) {
      return true;
    } else
      return false;
  }

  // yield::marshal::Object
  const static uint32_t TYPE_ID = static_cast<uint32_t>( 18 );
  uint32_t get_type_id() const {
    return TYPE_ID;
  }
  const char* get_type_name() const {
    return "StringMap";
  }
  StringMap& inc_ref() {
    return ::yield::Object::inc_ref( *this );
  }

  void marshal( ::yield::marshal::Marshaller& marshaller ) const {
    for ( const_iterator i = begin(); i != end(); i++ ) {
      marshaller.write( i->first, i->second );
    }
  }

  void unmarshal( ::yield::marshal::Unmarshaller& unmarshaller ) {
    ::yield::marshal::String key;
    unmarshaller.read_key( key );
    std::string value;
    unmarshaller.read_string( key, value );
    ( *this )[key] = value;
  }

  // yield::marshal::Map
  size_t get_size() const {
    return size();
  }
};


class StringMapStruct : public ::yield::marshal::Object {
public:
  const static uint32_t TYPE_ID = 19UL;

public:
  StringMapStruct() { }

  StringMapStruct( const yield::marshal::StringMap& value )
    : value( value )
  { }

  StringMapStruct( const StringMapStruct& other )
    : value( other.get_value() )
  { }

  virtual ~StringMapStruct() {  }

  const yield::marshal::StringMap& get_value() const {
    return value;
  }
  void set_value( const yield::marshal::StringMap&  value ) {
    this->value = value;
  }

  bool operator==( const StringMapStruct& other ) const {
    return get_value() == other.get_value();
  }

  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }
  const char* get_type_name() const {
    return "StringMapStruct";
  }
  StringMapStruct& inc_ref() {
    return ::yield::Object::inc_ref( *this );
  }

  void marshal( ::yield::marshal::Marshaller& marshaller ) const {
    marshaller.write( ::yield::marshal::StringLiteral( "value" ), get_value() );
  }

  void unmarshal( ::yield::marshal::Unmarshaller& unmarshaller ) {
    unmarshaller.read_map( ::yield::marshal::StringLiteral( "value" ), value );
  }

protected:
  yield::marshal::StringMap value;
};


class StringSet : public ::yield::marshal::Sequence, public std::vector< std::string > {
public:
  StringSet() { }

  StringSet( const std::string& first_value ) {
    vector<std::string>::push_back( first_value );
  }

  StringSet( size_type size )
    : vector<std::string>( size )
  { }

  virtual ~StringSet() { }

  virtual bool operator==( const StringSet& other ) const {
    if ( size() == other.size() ) {
      size_t i_max = size();
      for ( size_t i = 0; i < i_max; i++ ) {
        if ( !( ( *this )[i] == other[i] ) )
          return false;
      }

      return true;
    } else
      return false;
  }

  // yield::Object
  const static uint32_t TYPE_ID = static_cast<uint32_t>( 20 );
  uint32_t get_type_id() const {
    return TYPE_ID;
  }
  const char* get_type_name() const {
    return "StringSet";
  }
  StringSet& inc_ref() {
    return ::yield::Object::inc_ref( *this );
  }

  // yield::marshal::Object
  void marshal( ::yield::marshal::Marshaller& marshaller ) const {
    size_type value_i_max = size();
    for ( size_type value_i = 0; value_i < value_i_max; value_i++ ) {
      marshaller.write( ::yield::marshal::Null(), ( *this )[value_i] );
    }
  }

  void unmarshal( ::yield::marshal::Unmarshaller& unmarshaller ) {
    std::string value;
    unmarshaller.read_string( ::yield::marshal::Null(), value );
    push_back( value );
  }

  // yield::marshal::Sequence
  size_t get_size() const {
    return size();
  }
};


class StringSetStruct : public ::yield::marshal::Object {
public:
  const static uint32_t TYPE_ID = 21UL;

public:
  StringSetStruct() { }

  StringSetStruct( const yield::marshal::StringSet& value )
    : value( value )
  { }

  StringSetStruct( const StringSetStruct& other )
    : value( other.get_value() )
  { }

  virtual ~StringSetStruct() {  }

  const yield::marshal::StringSet& get_value() const {
    return value;
  }
  void set_value( const yield::marshal::StringSet&  value ) {
    this->value = value;
  }

  bool operator==( const StringSetStruct& other ) const {
    return get_value() == other.get_value();
  }

  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }
  const char* get_type_name() const {
    return "StringSetStruct";
  }
  StringSetStruct& inc_ref() {
    return ::yield::Object::inc_ref( *this );
  }

  void marshal( ::yield::marshal::Marshaller& marshaller ) const {
    marshaller.write( ::yield::marshal::StringLiteral( "value" ), get_value() );
  }

  void unmarshal( ::yield::marshal::Unmarshaller& unmarshaller ) {
    unmarshaller.read_sequence( ::yield::marshal::StringLiteral( "value" ), value );
  }

protected:
  yield::marshal::StringSet value;
};


class StringStruct : public ::yield::marshal::Object {
public:
  const static uint32_t TYPE_ID = 22UL;

public:
  StringStruct() { }

  StringStruct( const std::string& value )
    : value( value )
  { }

  StringStruct( const StringStruct& other )
    : value( other.get_value() )
  { }

  virtual ~StringStruct() {  }

  const std::string& get_value() const {
    return value;
  }
  void set_value( const std::string& value ) {
    this->value = value;
  }

  bool operator==( const StringStruct& other ) const {
    return get_value() == other.get_value();
  }

  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }
  const char* get_type_name() const {
    return "StringStruct";
  }
  StringStruct& inc_ref() {
    return ::yield::Object::inc_ref( *this );
  }

  void marshal( ::yield::marshal::Marshaller& marshaller ) const {
    marshaller.write( ::yield::marshal::StringLiteral( "value" ), get_value() );
  }

  void unmarshal( ::yield::marshal::Unmarshaller& unmarshaller ) {
    unmarshaller.read_string( ::yield::marshal::StringLiteral( "value" ), value );
  }

protected:
  std::string value;
};


class Uint8Struct : public ::yield::marshal::Object {
public:
  const static uint32_t TYPE_ID = 23UL;

public:
  Uint8Struct()
    : value( 0 )
  { }

  Uint8Struct( uint8_t value )
    : value( value )
  { }

  Uint8Struct( const Uint8Struct& other )
    : value( other.get_value() )
  { }

  virtual ~Uint8Struct() {  }

  uint8_t get_value() const {
    return value;
  }
  void set_value( uint8_t value ) {
    this->value = value;
  }

  bool operator==( const Uint8Struct& other ) const {
    return get_value() == other.get_value();
  }

  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }
  const char* get_type_name() const {
    return "Uint8Struct";
  }
  Uint8Struct& inc_ref() {
    return ::yield::Object::inc_ref( *this );
  }

  void marshal( ::yield::marshal::Marshaller& marshaller ) const {
    marshaller.write( ::yield::marshal::StringLiteral( "value" ), get_value() );
  }

  void unmarshal( ::yield::marshal::Unmarshaller& unmarshaller ) {
    value = unmarshaller.read_uint8( ::yield::marshal::StringLiteral( "value" ) );
  }

protected:
  uint8_t value;
};


class Uint16Struct : public ::yield::marshal::Object {
public:
  const static uint32_t TYPE_ID = 24UL;

public:
  Uint16Struct()
    : value( 0 )
  { }

  Uint16Struct( uint16_t value )
    : value( value )
  { }

  Uint16Struct( const Uint16Struct& other )
    : value( other.get_value() )
  { }

  virtual ~Uint16Struct() {  }

  uint16_t get_value() const {
    return value;
  }
  void set_value( uint16_t value ) {
    this->value = value;
  }

  bool operator==( const Uint16Struct& other ) const {
    return get_value() == other.get_value();
  }

  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }
  const char* get_type_name() const {
    return "Uint16Struct";
  }
  Uint16Struct& inc_ref() {
    return ::yield::Object::inc_ref( *this );
  }

  void marshal( ::yield::marshal::Marshaller& marshaller ) const {
    marshaller.write( ::yield::marshal::StringLiteral( "value" ), get_value() );
  }

  void unmarshal( ::yield::marshal::Unmarshaller& unmarshaller ) {
    value = unmarshaller.read_uint16( ::yield::marshal::StringLiteral( "value" ) );
  }

protected:
  uint16_t value;
};


class Uint32Struct : public ::yield::marshal::Object {
public:
  const static uint32_t TYPE_ID = 25UL;

public:
  Uint32Struct()
    : value( 0 )
  { }

  Uint32Struct( uint32_t value )
    : value( value )
  { }

  Uint32Struct( const Uint32Struct& other )
    : value( other.get_value() )
  { }

  virtual ~Uint32Struct() {  }

  uint32_t get_value() const {
    return value;
  }
  void set_value( uint32_t value ) {
    this->value = value;
  }

  bool operator==( const Uint32Struct& other ) const {
    return get_value() == other.get_value();
  }

  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }
  const char* get_type_name() const {
    return "Uint32Struct";
  }
  Uint32Struct& inc_ref() {
    return ::yield::Object::inc_ref( *this );
  }

  void marshal( ::yield::marshal::Marshaller& marshaller ) const {
    marshaller.write( ::yield::marshal::StringLiteral( "value" ), get_value() );
  }

  void unmarshal( ::yield::marshal::Unmarshaller& unmarshaller ) {
    value = unmarshaller.read_uint32( ::yield::marshal::StringLiteral( "value" ) );
  }

protected:
  uint32_t value;
};


class Uint64Struct : public ::yield::marshal::Object {
public:
  const static uint32_t TYPE_ID = 26UL;

public:
  Uint64Struct()
    : value( 0 )
  { }

  Uint64Struct( uint64_t value )
    : value( value )
  { }

  Uint64Struct( const Uint64Struct& other )
    : value( other.get_value() )
  { }

  virtual ~Uint64Struct() {  }

  uint64_t get_value() const {
    return value;
  }
  void set_value( uint64_t value ) {
    this->value = value;
  }

  bool operator==( const Uint64Struct& other ) const {
    return get_value() == other.get_value();
  }

  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }
  const char* get_type_name() const {
    return "Uint64Struct";
  }
  Uint64Struct& inc_ref() {
    return ::yield::Object::inc_ref( *this );
  }

  void marshal( ::yield::marshal::Marshaller& marshaller ) const {
    marshaller.write( ::yield::marshal::StringLiteral( "value" ), get_value() );
  }

  void unmarshal( ::yield::marshal::Unmarshaller& unmarshaller ) {
    value = unmarshaller.read_uint64( ::yield::marshal::StringLiteral( "value" ) );
  }

protected:
  uint64_t value;
};
}
}


#endif
