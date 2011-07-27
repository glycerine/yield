// object_test.cpp

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

#include "test_object.hpp"
#include "yield/assert.hpp"
#include "yunit.hpp"


TEST_SUITE(Object);

namespace yield {
bool TestObject::deleted = false;


TEST(Object, dec_ref) {
  TestObject* test_object = new TestObject;
  Object::dec_ref(test_object);
  throw_assert(TestObject::deleted);

  test_object = new TestObject;
  Object::inc_ref(test_object);
  Object::dec_ref(test_object);
  throw_assert_false(TestObject::deleted);
  Object::dec_ref(test_object);
  throw_assert(TestObject::deleted);
}

TEST(Object, inc_ref) {
  TestObject* test_object = new TestObject;
  Object::inc_ref(test_object);
}

TEST(Object, rtti) {
  TestObject test_object;
  throw_assert_eq(test_object.get_type_id(), 0);
  throw_assert_eq(strcmp(test_object.get_type_name(), "yield::Object"), 0);
}
}
