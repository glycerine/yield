// mutex_test.hpp

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

#ifndef _YIELD_THREAD_MUTEX_TEST_HPP_
#define _YIELD_THREAD_MUTEX_TEST_HPP_

#include "yield/assert.hpp"
#include "yield/time.hpp"
#include "yield/thread/mutex.hpp"
#include "yield/thread/thread.hpp"
#include "yunit.hpp"


namespace yield {
namespace thread {
template <class MutexType>
class MutexTest : public yunit::Test {
public:
  // Test
  void setup() {
    mutex = new MutexType;
  }

  void teardown() {
    delete mutex;
  }

protected:
  MutexType* mutex;
};


template <class MutexType>
class MutexLockTest : public MutexTest<MutexType> {
public:
  // Test
  void run() {
    auto_Object<Thread> thread = new Thread(thread_run, this);

    bool lock_ret = this->mutex->lock();
    throw_assert(lock_ret);
    Thread::self()->nanosleep(0.1);
    this->mutex->unlock();

    thread->join();
  }

private:
  static void thread_run(void* this_) {
    static_cast<MutexLockTest<MutexType>*>(this_)->mutex->lock();
    Thread::self()->nanosleep(0.1);
    static_cast<MutexLockTest<MutexType>*>(this_)->mutex->unlock();
  }
};


template <class MutexType>
class MutexTryLockTest : public MutexTest<MutexType> {
public:
  // Test
  void run() {
    auto_Object<Thread> thread = new Thread(thread_run, this);

    bool lock_ret = this->mutex->trylock();
    throw_assert(lock_ret);
    Thread::self()->nanosleep(0.1);
    this->mutex->unlock();

    thread->join();
  }

private:
  static void thread_run(void* this_) {
    static_cast<MutexTryLockTest<MutexType>*>(this_)->mutex->lock();
    Thread::self()->nanosleep(0.1);
    static_cast<MutexTryLockTest<MutexType>*>(this_)->mutex->unlock();
  }
};


template <class MutexType>
class MutexTestSuite : public yunit::TestSuite {
public:
  MutexTestSuite() {
    add("Mutex::lock", new MutexLockTest<MutexType>);
    add("Mutex::trylock", new MutexTryLockTest<MutexType>);
  }
};
}
}

#endif
