// yield/marshal/xml/xml_unmarshaller.hpp

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


#ifndef _YIELD_MARSHAL_XML_XML_UNMARSHALLER_HPP_
#define _YIELD_MARSHAL_XML_XML_UNMARSHALLER_HPP_


#include "yield/marshal/unmarshaller.hpp"


namespace rapidxml {
template <class> class xml_attribute;
template <class> class xml_document;
template <class> class xml_node;
}


namespace yield {
class Buffer;


namespace marshal {
namespace xml {
class XMLUnmarshaller : public Unmarshaller {
public:
  XMLUnmarshaller(const Buffer& buffer);
  XMLUnmarshaller(const char* xml, size_t xml_len);
  ~XMLUnmarshaller();

  // Unmarshaller
  bool read_bool(const Object& key);
  double read_double(const Object& key);
  int64_t read_int64(const Object& key);
  void read_key(Object& key);
  void read_map(const Object& key, Map& value);
  void read_object(const Object& key, Object& value);
  void read_sequence(const Object& key, Sequence& value);
  void read_string(const Object& key, string& value);
  uint64_t read_uint64(const Object& key);

private:
  XMLUnmarshaller(const rapidxml::xml_node<char>& root_node);
  void init();
  const rapidxml::xml_attribute<char>* read_attribute(const Object& k);
  const rapidxml::xml_node<char>* read_node(const Object& key);

private:
  rapidxml::xml_document<char>* doc;
  const rapidxml::xml_attribute<char>* next_attribute;
  const rapidxml::xml_node<char> *root_node, *next_node;
  string xml;
};
}
}
}


#endif
