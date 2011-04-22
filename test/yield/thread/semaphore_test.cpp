// semaphore_test.cpp

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
#include "yield/thread/semaphore.hpp"
#include "yield/thread/thread.hpp"
#include "yunit.hpp"


TEST_SUITE(Semaphore);

namespace yield {
namespace thread {
class SemaphoreTest : public yunit::Test {
public:
  // yunit::Test
  void setup() {
    semaphore = new Semaphore();
  }

  void teardown() {
    delete semaphore;
    semaphore = NULL;
  }

protected:
  SemaphoreTest() {
    exit_count = 0;
  }

  static void thread_run(void* this_) {
    static_cast<SemaphoreTest*>(this_)->semaphore->wait();
    static_cast<SemaphoreTest*>(this_)->exit_count++;
  }

protected:
  Semaphore* semaphore;
  uint8_t exit_count;
};


TEST_EX(Semaphore, threaded, SemaphoreTest) {
  semaphore->post();
  auto_Object<Thread> thread = new Thread(thread_run, this);
  while (exit_count < 1)
    Thread::self()->yield();
}

TEST_EX(Semaphore, timedwait, SemaphoreTest) {
  semaphore->post();
  throw_assert(semaphore->wait(0.1));
  Time start_time(Time::now());
  semaphore->wait(0.1);
  Time elapsed_time(Time::now() - start_time);
  throw_assert_ge(elapsed_time, Time(0.1));
}

TEST_EX(Semaphore, trywait, SemaphoreTest) {
  semaphore->post();
  throw_assert(semaphore->trywait());
  throw_assert_false(semaphore->trywait());
  semaphore->post();
  throw_assert(semaphore->trywait());
}

TEST_EX(Semaphore, wait, SemaphoreTest) {
  semaphore->post();
  throw_assert(semaphore->wait());
}
}
}
