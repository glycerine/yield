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

#include "yield/exception.hpp"
#include "yield/time.hpp"
#include "yield/thread/runnable.hpp"
#include "yield/thread/mutex.hpp"
#include "yield/thread/thread.hpp"
#include "yunit.hpp"

namespace yield {
namespace thread {
template <class MutexType>
class MutexTest : public yunit::Test {
public:
  // yunit::Test
  void setup() {
    mutex = new MutexType;
  }

  void teardown() {
    delete mutex;
  }

protected:
  class OtherThread : public Runnable {
  public:
    OtherThread(MutexType& mutex)
      : mutex(mutex) {
    }

  public:
    // yield::thread::Runnable
    void run() {
      mutex.lock();
      Thread::sleep(0.1);
      mutex.unlock();
    }

  private:
    MutexType& mutex;
  };

protected:
  MutexType* mutex;
};


template <class MutexType>
class MutexLockTest : public MutexTest<MutexType> {
public:
  // yunit::Test
  void run() {
    Thread thread(*new typename MutexTest<MutexType>::OtherThread(*this->mutex));

    if (!this->mutex->lock()) {
      throw Exception();
    }
    Thread::sleep(0.1);
    this->mutex->unlock();

    thread.join();
  }
};


template <class MutexType>
class MutexTryLockTest : public MutexTest<MutexType> {
public:
  // yunit::Test
  void run() {
    Thread thread(*new typename MutexTest<MutexType>::OtherThread(*this->mutex));

    if (!this->mutex->trylock()) {
      throw Exception();
    }
    Thread::sleep(0.1);
    this->mutex->unlock();

    thread.join();
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
