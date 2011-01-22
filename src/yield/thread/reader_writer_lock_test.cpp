// yield/thread/reader_writer_lock_test.cpp

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
#include "yield/time.hpp"
#include "yield/thread/reader_writer_lock.hpp"
#include "yield/thread/thread.hpp"
#include "yunit.hpp"


TEST_SUITE(ReaderWriterLock);

namespace yield {
namespace thread {
class ReaderWriterLockTest : public yunit::Test {
public:
  // Test
  void setup() {
    rwlock = new ReaderWriterLock;
  }

  void teardown() {
    delete rwlock;
    rwlock = NULL;
  }

protected:
  ReaderWriterLock* rwlock;
};


TEST_EX(ReaderWriterLock, rdlock, ReaderWriterLockTest) {
  bool rdlock_ret = rwlock->rdlock();
  throw_assert(rdlock_ret);
}

class ReaderWriterLockMultiReaderTest : public ReaderWriterLockTest {
public:
  // Test
  virtual void run() {
    bool rdlock_ret = rwlock->rdlock();
    throw_assert(rdlock_ret);

    auto_Object<Thread> thread = new Thread(thread_run, this);
    thread->join();
  }

private:
  static void thread_run(void* this_) {
    bool rdlock_ret
    = static_cast<ReaderWriterLockMultiReaderTest*>(this_)
      ->rwlock->rdlock();
    throw_assert(rdlock_ret);
  }
};

TEST_EX
(
  ReaderWriterLock,
  rdlock_multiple,
  ReaderWriterLockMultiReaderTest
) {
  ReaderWriterLockMultiReaderTest::run();
}

TEST_EX(ReaderWriterLock, rdunlock, ReaderWriterLockTest) {
  bool rdlock_ret = rwlock->rdlock();
  throw_assert(rdlock_ret);
  rwlock->rdunlock();
  rdlock_ret = rwlock->rdlock();
  throw_assert(rdlock_ret);
}

TEST_EX(ReaderWriterLock, tryrdlock, ReaderWriterLockTest) {
  bool tryrdlock_ret = rwlock->tryrdlock();
  throw_assert(tryrdlock_ret);
}

TEST_EX(ReaderWriterLock, trywrlock, ReaderWriterLockTest) {
  bool trywrlock_ret = rwlock->trywrlock();
  throw_assert(trywrlock_ret);
}

TEST_EX(ReaderWriterLock, wrlock, ReaderWriterLockTest) {
  bool wrlock_ret = rwlock->wrlock();
  throw_assert(wrlock_ret);
}

TEST_EX(ReaderWriterLock, wrunlock, ReaderWriterLockTest) {
  bool wrlock_ret = rwlock->wrlock();
  throw_assert(wrlock_ret);
  rwlock->wrunlock();
  wrlock_ret = rwlock->wrlock();
  throw_assert(wrlock_ret);
}
}
}
