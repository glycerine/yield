// yield/thread/thread_test.cpp

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


#include "yield/assert.hpp"
#include "yield/auto_object.hpp"
#include "yield/time.hpp"
#include "yield/thread/mutex.hpp"
#include "yield/thread/thread.hpp"
#include "yunit.hpp"


TEST_SUITE(Thread);

namespace yield {
namespace thread {
class ThreadTest : public yunit::Test {
public:
  // Test
  void setup() {
    mutex = new Mutex;
    mutex->lock();
  }

  void teardown() {
    delete mutex;
    mutex = NULL;
  }

protected:
  static void thread_run(void* this_) {
    static_cast<ThreadTest*>(this_)->mutex->lock();
  }

protected:
  Mutex* mutex;
};


TEST_EX(Thread, key_create, ThreadTest) {
  uintptr_t key = Thread::self()->key_create();
  throw_assert_ne(key, 0);
}

TEST_EX(Thread, key_delete, ThreadTest) {
  uintptr_t key = Thread::self()->key_create();
  throw_assert_ne(key, 0);
  if (!Thread::self()->key_delete(key))
    throw Exception();
}

TEST_EX(Thread, getspecific, ThreadTest) {
  uintptr_t key = Thread::self()->key_create();
  if (!Thread::self()->setspecific(key, reinterpret_cast<void*>(42)))
    throw Exception();
  void* ret_value = Thread::self()->getspecific(key);
  throw_assert_eq(reinterpret_cast<uintptr_t>(ret_value), 42);
}

TEST_EX(Thread, nanosleep, ThreadTest) {
  Time start_time(Time::now());
  Thread::self()->nanosleep(0.05);
  Time slept_time(Time::now() - start_time);
  throw_assert_ge(slept_time.ms(), 50);
}

TEST_EX(Thread, set_name, ThreadTest) {
  auto_Object<Thread> thread = new Thread(thread_run);
  thread->set_name("test thread");
}

TEST_EX(Thread, setaffinity, ThreadTest) {
  auto_Object<Thread> thread = new Thread(thread_run);
  bool success = thread->setaffinity(0);
  mutex->unlock();
  thread->join();
  throw_assert(success);
}

TEST_EX(Thread, setspecific, ThreadTest) {
  uintptr_t key = Thread::self()->key_create();
  if (!Thread::self()->setspecific(key, reinterpret_cast<void*>(42)))
    throw Exception();
}

TEST_EX(Thread, yield, ThreadTest) {
  Thread::self()->yield();
}
}
}
