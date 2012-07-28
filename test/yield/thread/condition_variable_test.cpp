// condition_variable_test.cpp

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

#include "yield/time.hpp"
#include "yield/thread/condition_variable.hpp"
#include "yield/thread/runnable.hpp"
#include "yield/thread/thread.hpp"
#include "gtest/gtest.h"

namespace yield {
namespace thread {
class ConditionVariableTest : public ::testing::Test {
public:
  // ::testing::Test
  void SetUp() {
    cond = new ConditionVariable;
  }

  void TearDown() {
    delete cond;
    cond = NULL;
  }

protected:
  class OtherThread : public Runnable {
  public:
    OtherThread(ConditionVariable& cond)
      : cond(cond) {
    }

    // yield::thread::Runnable
    void run() {
      cond.lock_mutex();
      cond.signal();
      cond.unlock_mutex();
    }

  private:
    ConditionVariable& cond;
  };

protected:
  ConditionVariable* cond;
};



TEST_F(ConditionVariableTest, timedwait) {
  bool lock_ret = cond->lock_mutex();
  ASSERT_TRUE(lock_ret);

  bool wait_ret = cond->timedwait(0.1);
  ASSERT_FALSE(wait_ret);

  Thread thread(*new OtherThread(*cond));

  Thread::sleep(0.01);

  cond->unlock_mutex();

  thread.join();
}

TEST_F(ConditionVariableTest, wait) {
  bool lock_ret = cond->lock_mutex();
  ASSERT_TRUE(lock_ret);

  Thread thread(*new OtherThread(*cond));

  bool wait_ret = cond->wait();
  ASSERT_TRUE(wait_ret);

  Thread::sleep(0.1);

  cond->unlock_mutex();

  thread.join();
}
}
}
