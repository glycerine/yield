// yield/thread/win32/condition_variable.hpp

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

#ifndef _YIELD_THREAD_WIN32_CONDITION_VARIABLE_HPP_
#define _YIELD_THREAD_WIN32_CONDITION_VARIABLE_HPP_

#ifdef _WIN32
#include "yield/thread/lightweight_mutex.hpp"
#include "yield/thread/mutex.hpp"
#include "yield/thread/semaphore.hpp"
#else
#include "yield/config.hpp"

#include <pthread.h>
#endif

namespace yield {
namespace thread {
/**
  Condition variable synchronization primitive.
*/
class ConditionVariable {
public:
  ConditionVariable();
  ~ConditionVariable();

public:
  /**
    Broadcast a signal to all waiters.
  */
  void broadcast();

  /**
    Lock the mutex associated with this condition variable before going
      into timedwait or wait.
    Blocks until the mutex is acquired or is destroyed.
    @return true if the caller now holds the mutex
  */
  bool lock_mutex();

  /**
    Signal a single waiter.
  */
  void signal();

  /**
    Wait on the condition variable for the specified time out.
    The caller must acquire the mutex before calling this method or wait.
    On a successful return, the caller holds the mutex and must
      unlock it (with unlock_mutex) again.
    @param timeout time to wait for a signal
    @return true if signal or broadcast was called in the wait period
  */
  bool timedwait(const Time& timeout);

  /**
    Try to lock the mutex associated with this condition variable before going
      into timedwait or wait. Do not block on failure.
    @return true if the caller now holds the mutex
  */
  bool trylock_mutex();

  /**
    Unlock the mutex associated with this condition variable.
  */
  void unlock_mutex();

  /**
    Wait on the condition variable for the specified time out.
    The caller must acquire the mutex before calling this method or timedwait.
    On a successful return, the caller holds the mutex and must
      unlock it (with unlock_mutex) again.
    @return true if signal or broadcast was called
  */
  bool wait();

private:
#ifdef _WIN32
  bool last_signal_was_broadcast;
  Mutex mutex;
  uint32_t waiters_count;
  LightweightMutex waiters_count_lock;
  Semaphore wait_barrier;
  void* wait_barrier_clear_signal;
#else
  pthread_cond_t cond;
  pthread_mutex_t mutex;
#endif
};
}
}

#endif
