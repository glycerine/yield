// yield/marshal/xml/xml_encoder.cpp

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


#include "yield/assert.hpp"
#include "yield/string_buffer.hpp"
#include "yield/marshal/xml/xml_encoder.hpp"

extern "C"
{
#include "genx.h"
};

#include <sstream> // For std::ostringstream


namespace yield {
namespace marshal {
namespace xml {
static genxStatus genx_send( void* userData, constUtf8 s ) {
  static_cast<Buffer*>( userData )->put
  (
    reinterpret_cast<const char*>( s )
  );

  return GENX_SUCCESS;
}

static
genxStatus
genx_sendBounded
(
  void* userData,
  constUtf8 start,
  constUtf8 end
) {
  static_cast<Buffer*>( userData )->put( start, end - start );
  return GENX_SUCCESS;
}

static genxStatus genx_flush( void* ) {
  return GENX_SUCCESS;
}

static genxSender genx_sender;


XMLEncoder::XMLEncoder()
  : buffer( *new StringBuffer ) {
  init();
}

XMLEncoder::XMLEncoder( Buffer& buffer )
  : buffer( buffer.inc_ref() ) {
  init();
}

XMLEncoder::~XMLEncoder() {
  genxDispose( writer );
  Buffer::dec_ref( buffer );
}

void
XMLEncoder::add_attribute
(
  const char* xmlns,
  const char* name,
  bool value
) {
  if ( value )
    add_attribute( xmlns, name, "true" );
  else
    add_attribute( xmlns, name, "false" );
}

void
XMLEncoder::add_attribute
(
  const char* xmlns,
  const char* name,
  double value
) {
  if ( value == 0.0 )
    add_attribute( xmlns, name, "0.0" );
  else {
    std::ostringstream value_oss;
    value_oss << value;
    add_attribute( xmlns, name, value_oss.str() );
  }
}

void
XMLEncoder::add_attribute
(
  const char* xmlns,
  const char* name,
  int64_t value
) {
  if ( value == 0 )
    add_attribute( xmlns, name, "0" );
  else {
    std::ostringstream value_oss;
    value_oss << value;
    add_attribute( xmlns, name, value_oss.str() );
  }
}

void
XMLEncoder::add_attribute
(
  const char* xmlns,
  const char* name,
  const char* value
) {
  genxStatus status
  = genxAddAttributeLiteral
    (
      writer,
      reinterpret_cast<constUtf8>( xmlns ),
      reinterpret_cast<constUtf8>( name ),
      reinterpret_cast<constUtf8>( value )
    );

  debug_assert_eq( status, GENX_SUCCESS );
}

void
XMLEncoder::add_attribute
(
  const char* xmlns,
  const char* name,
  uint64_t value
) {
  if ( value == 0 )
    add_attribute( xmlns, name, "0" );
  else {
    std::ostringstream value_oss;
    value_oss << value;
    add_attribute( xmlns, name, value_oss.str() );
  }
}

void XMLEncoder::add_text( bool text ) {
  if ( text )
    add_text( "true", 4 );
  else
    add_text( "false", 5 );
}

void XMLEncoder::add_text( double text ) {
  if ( text == 0.0 )
    add_text( "0.0", 3 );
  else {
    std::ostringstream value_oss;
    value_oss << text;
    add_text( value_oss.str() );
  }
}

void XMLEncoder::add_text( int64_t text ) {
  if ( text == 0 )
    add_text( "0", 1 );
  else {
    std::ostringstream value_oss;
    value_oss << text;
    add_text( value_oss.str() );
  }
}

void XMLEncoder::add_text( const char* text ) {
  add_text( text, strlen( text ) );
}

void XMLEncoder::add_text( const string& text ) {
  add_text( text.data(), text.size() );
}

void XMLEncoder::add_text( const char* text, size_t value_len ) {
  genxStatus status
  = genxAddCountedText
    (
      writer,
      reinterpret_cast<constUtf8>( text ),
      value_len
    );

  debug_assert_eq( status, GENX_SUCCESS );
}

void XMLEncoder::add_text( uint64_t text ) {
  if ( text == 0 )
    add_text( "0", 1 );
  else {
    std::ostringstream value_oss;
    value_oss << text;
    add_text( value_oss.str() );
  }
}

void XMLEncoder::end_element() {
  genxStatus status = genxEndElement( writer );
  debug_assert_eq( status, GENX_SUCCESS );
}

void XMLEncoder::finalize() {
  genxStatus status = genxEndDocument( writer );
  debug_assert_eq( status, GENX_SUCCESS );
}

Buffer& XMLEncoder::get_buffer() {
  finalize();
  return buffer;
}

void XMLEncoder::init() {
  writer = genxNew( NULL, NULL, &buffer );

  genx_sender.flush = genx_flush;
  genx_sender.send = genx_send;
  genx_sender.sendBounded = genx_sendBounded;

  genxStatus status = genxStartDocSender( writer, &genx_sender );
  debug_assert_eq( status, GENX_SUCCESS );
}

void XMLEncoder::start_element( const char* xmlns, const char* type ) {
  genxStatus status
  = genxStartElementLiteral
    (
      writer,
      reinterpret_cast<constUtf8>( xmlns ),
      reinterpret_cast<constUtf8>( type )
    );
  debug_assert_eq( status, GENX_SUCCESS );
}
}
}
}
