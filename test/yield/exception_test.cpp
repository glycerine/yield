// exception_test.cpp

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

#include "yield/auto_object.hpp"
#include "yield/assert.hpp"
#include "yield/exception.hpp"
#include "yunit.hpp"

TEST_SUITE(Exception);

namespace yield {
TEST(Exception, clone) {
  Exception exc1(1, "what");
  auto_Object<Exception> exc2 = exc1.clone();
  throw_assert_eq(exc2->get_error_code(), 1);
  throw_assert_eq(strcmp(exc2->what(), "what"), 0);
}

TEST(Exception, copy) {
  Exception exc1(1, "what");
  Exception exc2(exc1);
  throw_assert_eq(exc2.get_error_code(), 1);
  throw_assert_eq(strcmp(exc2.what(), "what"), 0);
}

TEST(Exception, from_error_code) {
  Exception exc(static_cast<uint32_t>(1));
  throw_assert_eq(exc.get_error_code(), 1);
  throw_assert_gt(strlen(exc.what()), 0)
}

TEST(Exception, from_error_message_c_string) {
  Exception exc("what");
  throw_assert_eq(strcmp(exc.what(), "what"), 0);
}

TEST(Exception, from_error_message_string) {
  Exception exc(string("what"));
  throw_assert_eq(strcmp(exc.what(), "what"), 0);
}

TEST(Exception, from_error_code_error_message_c_string) {
  Exception exc(1, "what");
  throw_assert_eq(exc.get_error_code(), 1);
  throw_assert_eq(strcmp(exc.what(), "what"), 0);
}

TEST(Exception, from_error_code_error_message_string) {
  Exception exc(1, string("what"));
  throw_assert_eq(strcmp(exc.what(), "what"), 0);
}

TEST(Exception, from_last_error_code) {
  Exception exc;
  throw_assert_gt(strlen(exc.what()), 0)
}

TEST(Exception, get_last_error_code) {
  Exception::set_last_error_code(1);
  uint32_t last_error_code = Exception::get_last_error_code();
  throw_assert_eq(last_error_code, 1);
}

TEST(Exception, is_exception) {
  throw_assert(Exception().is_exception());
}

TEST(Exception, rethrow) {
  Exception exc1(1);

  try {
    exc1.rethrow();
  } catch (Exception& exc2) {
    throw_assert_eq(exc1.get_error_code(), exc2.get_error_code());
    return;
  }

  throw_assert(false);
}

TEST(Exception, set_last_error_code) {
  Exception::set_last_error_code(1);
  uint32_t last_error_code = Exception::get_last_error_code();
  throw_assert_eq(last_error_code, 1);
}
}
