// yield/thread/condition_variable_test.cpp

// Copyright (c) 2010 Minor Gordon
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
#include "yield/thread/condition_variable.hpp"
#include "yield/thread/thread.hpp"
#include "yunit.hpp"


TEST_SUITE( ConditionVariable );

namespace yield {
namespace thread {
class ConditionVariableTest : public yunit::Test {
public:
  // Test
  void setup() {
    cond = new ConditionVariable;
  }

  void teardown() {
    delete cond;
    cond = NULL;
  }

protected:
  static void thread_run( void* this_ ) {
    static_cast<ConditionVariableTest*>( this_ )->cond->lock_mutex();
    static_cast<ConditionVariableTest*>( this_ )->cond->signal();
    static_cast<ConditionVariableTest*>( this_ )->cond->unlock_mutex();
  }

protected:
  ConditionVariable* cond;
};


TEST_EX( ConditionVariable, timedwait, ConditionVariableTest ) {
  bool lock_ret = cond->lock_mutex();
  throw_assert( lock_ret );

  bool wait_ret = cond->wait( 0.1 );
  throw_assert_false( wait_ret );

  auto_Object<Thread> thread = new Thread( thread_run, this );

  Thread::self()->nanosleep( 0.01 );

  cond->unlock_mutex();

  thread->join();
}

TEST_EX( ConditionVariable, wait, ConditionVariableTest ) {
  bool lock_ret = cond->lock_mutex();
  throw_assert( lock_ret );

  auto_Object<Thread> thread = new Thread( thread_run, this );

  bool wait_ret = cond->wait();
  throw_assert( wait_ret );

  Thread::self()->nanosleep( 0.1 );

  cond->unlock_mutex();

  thread->join();
}
}
}
