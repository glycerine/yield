// yield/marshal/xml/xml_marshaller.hpp

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


#ifndef _YIELD_MARSHAL_XML_XML_MARSHALLER_HPP_
#define _YIELD_MARSHAL_XML_XML_MARSHALLER_HPP_


#include "yield/marshal/marshaller.hpp"
#include "yield/marshal/xml/xml_encoder.hpp"


namespace yield {
namespace marshal {
namespace xml {
class XMLMarshaller : public Marshaller, private XMLEncoder {
public:
  XMLMarshaller();
  XMLMarshaller( Buffer& buffer );

  Buffer& get_buffer() {
    return XMLEncoder::get_buffer();
  }

  // Marshaller
  void write( const Object& key, bool value );
  void write( const Object& key, double value  );
  void write( const Object& key, int64_t value );
  void write( const Object& key, const Null& value );
  void write( const Object& key, const Object& value );
  void write( const Object& key, const Sequence& value );
  void write( const Object& key, const char* value, size_t );
  void write( const Object& key, uint64_t value );

private:
  template <typename ValueType>
  void add_attribute( const Object& key, ValueType value );
  void end_element( const Object& key );
  void start_element( const Object& key );

private:
  uint64_t next_anonymous_key_i;
};
}
}
}


#endif
