// yield/thread/win32/condition_variable.cpp

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
#include "yield/exception.hpp"
#include "yield/time.hpp"
#include "yield/thread/condition_variable.hpp"

#include <Windows.h>

namespace yield {
namespace thread {
ConditionVariable::ConditionVariable() {
  wait_barrier_clear_signal = CreateEvent(NULL, FALSE, FALSE, NULL);
  if (wait_barrier_clear_signal == NULL) {
    throw Exception();
  }

  last_signal_was_broadcast = false;
  waiters_count = 0;
}

ConditionVariable::~ConditionVariable() {
  CloseHandle(wait_barrier_clear_signal);
}

void ConditionVariable::broadcast() {
  waiters_count_lock.lock();

  if (waiters_count > 0) {
    last_signal_was_broadcast = true;
    ReleaseSemaphore(wait_barrier, waiters_count, 0);
    waiters_count_lock.unlock();
    WaitForSingleObject(wait_barrier_clear_signal, INFINITE);
    last_signal_was_broadcast = false;
  } else {
    waiters_count_lock.unlock();
  }
}

bool ConditionVariable::lock_mutex() {
  return mutex.lock();
}

void ConditionVariable::signal() {
  waiters_count_lock.lock();
  bool have_waiters = waiters_count > 0;
  waiters_count_lock.unlock();

  if (have_waiters) {
    wait_barrier.post();
  }
}

bool ConditionVariable::timedwait(const Time& timeout) {
  waiters_count_lock.lock();
  waiters_count++;
  waiters_count_lock.unlock();

  DWORD dwRet
  = SignalObjectAndWait(
      mutex,
      wait_barrier,
      static_cast<DWORD>(timeout.ms()),
      FALSE
    );

  if (dwRet == WAIT_OBJECT_0) {
    waiters_count_lock.lock();
    waiters_count--;
    bool last_waiter = last_signal_was_broadcast && waiters_count == 0;
    waiters_count_lock.unlock();

    if (last_waiter) {
      dwRet = SignalObjectAndWait(wait_barrier_clear_signal, mutex, INFINITE, FALSE);
      return dwRet == WAIT_OBJECT_0;
    } else {
      return mutex.lock();
    }
  } else {
    waiters_count_lock.lock();
    waiters_count--;
    waiters_count_lock.unlock();
    return false;
  }
}

bool ConditionVariable::trylock_mutex() {
  return mutex.trylock();
}

void ConditionVariable::unlock_mutex() {
  mutex.unlock();
}

bool ConditionVariable::wait() {
  waiters_count_lock.lock();
  waiters_count++;
  waiters_count_lock.unlock();

  DWORD dwRet
  = SignalObjectAndWait
    (
      mutex,
      wait_barrier,
      INFINITE,
      FALSE
    );

  if (dwRet == WAIT_OBJECT_0) {
    waiters_count_lock.lock();
    waiters_count--;
    bool last_waiter = last_signal_was_broadcast && waiters_count == 0;
    waiters_count_lock.unlock();

    if (last_waiter) {
      dwRet = SignalObjectAndWait(wait_barrier_clear_signal, mutex, INFINITE, FALSE);
      return dwRet == WAIT_OBJECT_0;
    } else {
      return mutex.lock();
    }
  } else {
    debug_break();
    waiters_count_lock.lock();
    waiters_count--;
    waiters_count_lock.unlock();
    return false;
  }
}
}
}
