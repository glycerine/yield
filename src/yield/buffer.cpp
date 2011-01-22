// yield/buffer.cpp

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


#include "yield/buffer.hpp"


namespace yield {
Buffer::Buffer( size_t capacity )
  : capacity_( capacity ) {
  next_buffer = NULL;
  size_ = 0;
}

Buffer::~Buffer() {
  Buffer::dec_ref( next_buffer );
}

Buffer::operator const char*() const {
  return static_cast<const char*>( data() );
}

Buffer::operator const uint8_t*() const {
  return static_cast<const uint8_t*>( data() );
}

bool Buffer::operator==( const Buffer& other ) const {
  if ( size() == other.size() ) {
    const void* this_data = static_cast<const void*>( *this );
    const void* other_data = static_cast<const void*>( other );
    if ( this_data != NULL && other_data != NULL )
      return memcmp( this_data, other_data, size() ) == 0;
    else
      return false;
  } else
    return false;
}

bool Buffer::operator!=( const Buffer& other ) const {
  return !operator==( other );
}

void Buffer::put( const Buffer& buf ) {
  put( buf, buf.size() );
}

void Buffer::put( char buf, size_t repeat_count ) {
  for ( size_t char_i = 0; char_i < repeat_count; char_i++ )
    put( &buf, 1 );
}

void Buffer::put( const iovec& iov ) {
  put( iov.iov_base, iov.iov_len );
}

void Buffer::put( const char* buf ) {
  put( buf, strlen( buf ) );
}

void Buffer::put( const string& buf ) {
  put( buf.c_str(), buf.size() );
}

void Buffer::put( const void* buf, size_t len ) {
  reserve( size() + len );

  memcpy_s
  (
    static_cast<char*>( *this ) + size(),
    capacity() - size(),
    buf,
    len
  );

  resize( size() + len );
}

void Buffer::resize( size_t new_size ) {
  reserve( new_size );
  size_ = new_size;
}

void Buffer::set_next_buffer( Buffer* next_buffer ) {
  if ( next_buffer != NULL )
    set_next_buffer( *next_buffer );
  else {
    Buffer::dec_ref( this->next_buffer );
    this->next_buffer = NULL;
  }
}

void Buffer::set_next_buffer( Buffer& next_buffer ) {
  Buffer::dec_ref( this->next_buffer );
  this->next_buffer = &next_buffer;
}
}
