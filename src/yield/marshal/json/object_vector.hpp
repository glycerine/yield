// yield/marshal/json/object_vector.hpp

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


#ifndef _YIELD_MARSHAL_JSON_OBJECT_VECTOR_HPP_
#define _YIELD_MARSHAL_JSON_OBJECT_VECTOR_HPP_


#include "yield/marshal/marshaller.hpp"
#include "yield/marshal/null.hpp"
#include "yield/marshal/sequence.hpp"


namespace yield {
namespace marshal {
namespace json {
class ObjectVector : public Sequence, public vector<Object*> {
public:
  const static uint32_t TYPE_ID = 1404038540UL;

public:
  ~ObjectVector() {
    for (iterator object_i = begin(); object_i != end(); ++object_i)
      Object::dec_ref(**object_i);
  }

  // Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }
  const char* get_type_name() const {
    return "yield::marshal::ObjectVector";
  }

  void marshal(Marshaller& marshaller) const {
    for (const_iterator object_i = begin(); object_i != end(); ++object_i)
      marshaller.write(Null(), **object_i);
  }

  bool operator==(const Object& other) const {
    if (other.get_type_id() == TYPE_ID) {
      if (static_cast<const ObjectVector&>(other).size() == size()) {
        for (size_t object_i = 0; object_i < size(); object_i++) {
          if
          (
            !(
              *(*this)[object_i]
              ==
              *static_cast<const ObjectVector&>(other)[object_i]
            )
          )
            return false;
        }
      }
    }

    return false;
  }

  // Sequence
  size_t get_size() const {
    return size();
  }
};
}
}
}


#endif
