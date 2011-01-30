// yield/marshal/unmarshaller.hpp

// Copyright (c) 2011 Minor Gordon
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

#ifndef _YIELD_MARSHAL_UNMARSHALLER_HPP_
#define _YIELD_MARSHAL_UNMARSHALLER_HPP_

#include "yield/marshal/map.hpp"


namespace yield {
namespace marshal {
class Map;
class Sequence;


class Unmarshaller {
public:
  virtual ~Unmarshaller() { }

  virtual bool read_bool(const Object& key) = 0;
  virtual float read_float(const Object& key);
  virtual double read_double(const Object& key) = 0;
  virtual int8_t read_int8(const Object& key);
  virtual int16_t read_int16(const Object& key);
  virtual int32_t read_int32(const Object& key);
  virtual int64_t read_int64(const Object& key) = 0;
  virtual void read_key(Object& key) = 0;
  virtual void read_map(const Object& key, Map& value) = 0;
  virtual void read_object(const Object& key, Object& value) = 0;
  virtual void read_sequence(const Object& key, Sequence& value) = 0;
  virtual void read_string(const Object& key, string& value) = 0;
  virtual uint8_t read_uint8(const Object& key);
  virtual uint16_t read_uint16(const Object& key);
  virtual uint32_t read_uint32(const Object& key);
  virtual uint64_t read_uint64(const Object& key) = 0;
};
}
}

#endif
