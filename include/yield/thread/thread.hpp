// yield/thread/win32/thread.hpp

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

#ifndef _YIELD_THREAD_WIN32_THREAD_HPP_
#define _YIELD_THREAD_WIN32_THREAD_HPP_

#include "yield/auto_object.hpp"

#ifndef _WIN32
#include <pthread.h>
#ifdef __sun
#include <thread.h>
#endif
#endif

namespace yield {
class Time;

namespace thread {
class ProcessorSet;
class Runnable;

class Thread : public Object {
public:
  Thread(YO_NEW_REF Runnable&);
  ~Thread();

public:
  bool cancel();

public:
  Runnable* get_runnable() const {
    return runnable;
  }

public:
  void* getspecific(uintptr_t key);

public:
  bool join();

public:
  bool is_running() {
    return state == STATE_RUNNING;
  }

public:
  uintptr_t key_create();
  bool key_delete(uintptr_t key);

public:
  void nanosleep(const Time&);

public:
  static auto_Object<Thread> self();

public:
  void set_name(const char* name);

public:
  bool setaffinity(uint16_t logical_processor_i);
  bool setaffinity(const ProcessorSet& logical_processor_set);

public:
  bool setspecific(uintptr_t key, void* value);

public:
  void yield();

private:
#ifdef _WIN32
  Thread(void* handle, unsigned long id);
#else
  Thread(pthread_t pthread);
#endif

private:
  static unsigned long __stdcall run(void*);

private:
#ifdef _WIN32
  void* handle;
  unsigned long id;
#else
  pthread_t pthread;
#if defined(__linux__)
  pid_t tid;
#elif defined(__sun)
  thread_t thread;
#endif
#endif

  Runnable* runnable;
  enum { STATE_READY, STATE_RUNNING, STATE_SUSPENDED } state;
};
}
}

#endif
