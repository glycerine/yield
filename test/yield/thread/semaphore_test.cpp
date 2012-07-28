// semaphore_test.cpp

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

#include "yield/auto_object.hpp"
#include "yield/time.hpp"
#include "yield/thread/runnable.hpp"
#include "yield/thread/semaphore.hpp"
#include "yield/thread/thread.hpp"
#include "gtest/gtest.h"

namespace yield {
namespace thread {
class SemaphoreTest : public ::testing::Test {
public:
  void SetUp() {
    semaphore = new Semaphore();
  }

  void TearDown() {
    delete semaphore;
    semaphore = NULL;
  }

protected:
  class OtherThread : public Runnable {
  public:
    OtherThread(uint8_t& exit_count, Semaphore& semaphore)
      : exit_count(exit_count), semaphore(semaphore) {
    }

  public:
    // yield::thread::Runnable
    void run() {
      semaphore.wait();
      exit_count++;
    }

  private:
    uint8_t& exit_count;
    Semaphore& semaphore;
  };

protected:
  SemaphoreTest() {
    exit_count = 0;
  }

protected:
  Semaphore* semaphore;
  uint8_t exit_count;
};


TEST_F(SemaphoreTest, threaded) {
  semaphore->post();
  Thread thread(*new OtherThread(exit_count, *semaphore));
  while (exit_count < 1) {
    Thread::self()->yield();
  }
  thread.join();
}

TEST_F(SemaphoreTest, timedwait) {
  semaphore->post();
  ASSERT_TRUE(semaphore->timedwait(0.1));
  Time start_time(Time::now());
  semaphore->timedwait(0.1);
  Time elapsed_time(Time::now() - start_time);
  ASSERT_GE(elapsed_time, Time(0.1));
}

TEST_F(SemaphoreTest, trywait) {
  semaphore->post();
  ASSERT_TRUE(semaphore->trywait());
  ASSERT_FALSE(semaphore->trywait());
  semaphore->post();
  ASSERT_TRUE(semaphore->trywait());
}

TEST_F(SemaphoreTest, wait) {
  semaphore->post();
  ASSERT_TRUE(semaphore->wait());
}
}
}
