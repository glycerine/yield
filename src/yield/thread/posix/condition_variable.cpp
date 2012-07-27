// yield/thread/posix/condition_variable.cpp

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

#include "yield/exception.hpp"
#include "yield/time.hpp"
#include "yield/thread/condition_variable.hpp"

namespace yield {
namespace thread {
ConditionVariable::ConditionVariable() {
  if (pthread_cond_init(&cond, NULL) != 0) {
    throw Exception();
  }

  if (pthread_mutex_init(&mutex, NULL) != 0) {
    throw Exception();
  }
}

ConditionVariable::~ConditionVariable() {
  pthread_cond_destroy(&cond);
  pthread_mutex_destroy(&mutex);
}

void ConditionVariable::broadcast() {
  pthread_cond_broadcast(&cond);
}

bool ConditionVariable::lock_mutex() {
  return pthread_mutex_lock(&mutex) == 0;
}

void ConditionVariable::signal() {
  pthread_cond_signal(&cond);
}

bool ConditionVariable::timedwait(const Time& timeout) {
  timespec timeout_ts = Time::now() + timeout;
  return pthread_cond_timedwait(&cond, &mutex, &timeout_ts) == 0;
}

bool ConditionVariable::trylock_mutex() {
  return pthread_mutex_trylock(&mutex) == 0;
}

void ConditionVariable::unlock_mutex() {
  pthread_mutex_unlock(&mutex);
}

bool ConditionVariable::wait() {
  return pthread_cond_wait(&cond, &mutex) == 0;
}
}
}
