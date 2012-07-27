// thread_test.cpp

// Copyright (c) 2012 Minor Gordon
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
#include "yield/auto_object.hpp"
#include "yield/exception.hpp"
#include "yield/time.hpp"
#include "yield/thread/thread.hpp"
#include "yunit.hpp"

TEST_SUITE(Thread);

namespace yield {
namespace thread {
TEST(Thread, key_create) {
  uintptr_t key = Thread::self()->key_create();
  if (key != 0) {
    Thread::self()->key_delete(key);
  } else {
    throw Exception();
  }
}

TEST(Thread, key_delete) {
  uintptr_t key = Thread::self()->key_create();
  throw_assert_ne(key, 0);
  if (!Thread::self()->key_delete(key)) {
    throw Exception();
  }
}

TEST(Thread, getspecific) {
  uintptr_t key = Thread::self()->key_create();
  if (!Thread::self()->setspecific(key, reinterpret_cast<void*>(42))) {
    throw Exception();
  }
  void* ret_value = Thread::self()->getspecific(key);
  throw_assert_eq(reinterpret_cast<uintptr_t>(ret_value), 42);
  Thread::self()->key_delete(key);
}

TEST(Thread, set_name) {
  Thread::self()->set_name("test thread");
}

TEST(Thread, setaffinity) {
  throw_assert_true(Thread::self()->setaffinity(0));
}

TEST(Thread, setspecific) {
  uintptr_t key = Thread::self()->key_create();
  if (!Thread::self()->setspecific(key, reinterpret_cast<void*>(42))) {
    throw Exception();
  }
  Thread::self()->key_delete(key);
}

TEST(Thread, sleep) {
  Time start_time(Time::now());
  Thread::sleep(0.05);
  Time slept_time(Time::now() - start_time);
  throw_assert_ge(slept_time.ms(), 50);
}

TEST(Thread, yield) {
  Thread::self()->yield();
}
}
}
