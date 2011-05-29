// yield/thread/thread.hpp

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

#ifndef _YIELD_THREAD_THREAD_HPP_
#define _YIELD_THREAD_THREAD_HPP_

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

/**
  A kernel-level thread.
*/
class Thread : public Object {
public:
  /**
    Construct and start a thread with the entry point runnable.
    @param runnable the thread entry point.
  */
  Thread(YO_NEW_REF Runnable& runnable);

  /**
    Cancel and destroy the thread.
  */
  ~Thread();

public:
  /**
    Cancel/stop the thread's execution.
  */
  bool cancel();

public:
  /**
    Get the Runnable associated with this thread.
    @return the Runnable associated with this thread
  */
  Runnable* get_runnable() const {
    return runnable;
  }

public:
  /**
    Get a value from the caller's thread-local storage.
    @param key the key to get, created with key_create
    @return the value in thread-local storage associated with key
  */
  static void* getspecific(uintptr_t key);

public:
  /**
    Join the caller's thread to the callee's thread.
    Blocks the caller's thread until the callee thread has exited.
    @return true if the join was successful
  */
  bool join();

public:
  /**
    Check if this thread is running.
    @return true if this thread is running
  */
  bool is_running() {
    return state == STATE_RUNNING;
  }

public:
  /**
    Create a key in the caller's thread-local storage.
    @return a new key for thread-local storage or -1 on failure
  */
  static uintptr_t key_create();

  /**
    Delete a thread-local storage key when it's no longer needed.
    @param key the thread-local storage key
    @return true if the delete succeeded
  */
  static bool key_delete(uintptr_t key);

public:
  /**
    Make the caller's thread sleep for the specified timeout.
    The system sleep time granularity may be coarser than nanoseconds,
      (e.g., milliseconds on Win32).
    @param timeout time to sleep
  */
  static void nanosleep(const Time& timeout);

public:
  /**
    Wrap the caller's thread in a Thread object.
    @return the caller's Thread
  */
  static auto_Object<Thread> self();

public:
  /**
    Set the name of the thread.
    Mainly used in debugging.
    @param name new name of the thread
  */
  void set_name(const char* name);

public:
  /**
    Set the affinity of the thread, binding it to a single [logical] processor.
    @param logical_processor_i index of the logical processor to bind the thread to
    @return true if the binding succeeded.
  */
  bool setaffinity(uint16_t logical_processor_i);

  /**
    Set the affinity of the thread, bindingi t to one or more [logical] processors.
    @param logical_processor_set ProcessorSet describing the processors to bind to
    @return true if the binding succeeded
  */
  bool setaffinity(const ProcessorSet& logical_processor_set);

public:
  /**
    Set a value in thread-local storage.
    @param key key in thread-local storage, returned by key_create
    @param value value to set
    @return true if the set succeeded
  */
  static bool setspecific(uintptr_t key, void* value);

public:
  /**
    Yield the caller's thread to other threads.
  */
  static void yield();

private:
#ifdef _WIN32
  Thread(void* handle, unsigned long id);
#else
  Thread(pthread_t pthread);
#endif

private:
#ifdef _WIN32
  static unsigned long __stdcall run(void*);
  unsigned long run();
#else
  static void* run(void*);
  void* run();
#endif

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
