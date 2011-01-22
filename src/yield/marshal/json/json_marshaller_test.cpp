// yield/marshal/json/json_marshaller_test.cpp

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
#include "yield/marshal/pretty_printer.hpp"
#include "yield/marshal/json/json_marshaller.hpp"
#include "yield/marshal/json/json_unmarshaller.hpp"
#include "../marshaller_test.hpp"

#include <iostream>


namespace yield {
namespace marshal {
namespace json {
template <class ObjectType>
class JSONMarshallerTest : public MarshallerTest<ObjectType> {
public:
  JSONMarshallerTest(YO_NEW_REF ObjectType* object)
    : MarshallerTest<ObjectType>(object)
  { }

  // yunit::Test
  void run() {
    JSONMarshaller json_marshaller;
    json_marshaller.write(Null(), *this->object);
    Buffer& buffer = json_marshaller.get_buffer();

    std::cout << string(buffer, buffer.size()) << std::endl;

    JSONUnmarshaller json_unmarshaller(buffer);
    json_unmarshaller.read_object(Null(), *this->empty_object);

    if (!(*this->object == *this->empty_object)) {
      PrettyPrinter pretty_printer(std::cout);
      this->empty_object->marshal(pretty_printer);
      std::cout << string(buffer, buffer.size()) << std::endl;
      throw_assert_eq(*this->object, *this->empty_object);
    }
  }
};
}
}
}

TEST_SUITE_EX(JSONMarshaller, yield::marshal::MarshallerTestSuite<yield::marshal::json::JSONMarshallerTest>);
