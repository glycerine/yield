// yield/thread/posix/thread.cpp

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

#include "yield/exception.hpp"
#include "yield/time.hpp"
#include "yield/thread/runnable.hpp"
#include "yield/thread/thread.hpp"

#ifdef __linux__
#include <sys/syscall.h>
#endif

namespace yield {
namespace thread {
Thread::Thread(YO_NEW_REF Runnable& runnable)
  : runnable(&runnable) {
  state = STATE_READY;

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  if (pthread_create(&pthread, &attr, &run, this) == 0) {
    pthread_attr_destroy(&attr);

    while (state == STATE_READY)
      nanosleep(0);
  } else {
    pthread_attr_destroy(&attr);
    throw Exception();
  }
}

Thread::Thread(pthread_t pthread)
  : pthread(pthread) {
  runnable = NULL;
  state = STATE_READY;
}

Thread::~Thread() {
  if (is_running()) {
    cancel();
    join();
  }

  Runnable::dec_ref(runnable);
}

bool Thread::cancel() {
  return pthread_cancel(pthread) == 0;
}

void* Thread::getspecific(uintptr_t key) {
  return pthread_getspecific(key);
}

bool Thread::join() {
  return pthread_join(pthread, NULL) != -1;
}

uintptr_t Thread::key_create() {
  pthread_key_t key;
  if (pthread_key_create(&key, NULL) == 0)
    return key;
  else
    return static_cast<uintptr_t>(-1);
}

bool Thread::key_delete(uintptr_t key) {
  return pthread_key_delete(key) == 0;
}

void Thread::nanosleep(const Time& timeout) {
  timespec timeout_ts = timeout;
  ::nanosleep(&timeout_ts, NULL);
}

auto_Object<Thread> Thread::self() {
  Thread* thread = new Thread(pthread_self());
#if defined(__linux__)
  thread->tid = syscall(SYS_gettid);
#elif defined(__sun)
  thread->thread = thr_self();
#endif
  return thread;
}

void* Thread::run(void* this_) {
  return static_cast<Thread*>(this_)->run();
}

void* Thread::run() {
#if defined(__linux__)
  tid = syscall(SYS_gettid);
#elif defined(__sun)
  thread = thr_self();
#endif
  state = STATE_RUNNING;
  runnable->run();
  state = STATE_SUSPENDED;
  return NULL;
}

void Thread::set_name(const char* name) {
}

bool Thread::setspecific(uintptr_t key, void* value) {
  return pthread_setspecific(key, value) == 0;
}
}
}
