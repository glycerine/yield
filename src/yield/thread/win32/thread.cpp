// yield/thread/win32/thread.cpp

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
#include "yield/thread/processor_set.hpp"
#include "yield/thread/runnable.hpp"
#include "yield/thread/thread.hpp"

#include <Windows.h>

namespace yield {
namespace thread {
Thread::Thread(Runnable& runnable)
  : runnable(&runnable) {
  state = STATE_READY;

  handle = CreateThread(NULL, 0, run, this, NULL, &id);

  if (handle != NULL) {
    while (state == STATE_READY) {
      yield();
    }
  } else {
    throw Exception();
  }
}

Thread::Thread(HANDLE handle, DWORD id)
  : handle(handle), id(id), runnable(NULL) {
  state = STATE_RUNNING;
}

Thread::~Thread() {
  if (get_runnable() != NULL) {
    if (is_running()) {
      cancel();
      join();
    }

    CloseHandle(handle);
  }

  Runnable::dec_ref(runnable);
}

bool Thread::cancel() {
  return TerminateThread(handle, 0) == TRUE;
}

void* Thread::getspecific(uintptr_t key) {
  return TlsGetValue(key);
}

bool Thread::join() {
  return WaitForSingleObject(handle, INFINITE) == WAIT_OBJECT_0;
}

uintptr_t Thread::key_create() {
  return TlsAlloc();
}

bool Thread::key_delete(uintptr_t key) {
  return TlsFree(key) == TRUE;
}

unsigned long __stdcall Thread::run(void* this_) {
  return static_cast<Thread*>(this_)->run();
}

unsigned long Thread::run() {
  state = STATE_RUNNING;
  runnable->run();
  state = STATE_SUSPENDED;
  return 0;
}

auto_Object<Thread> Thread::self() {
  return new Thread(GetCurrentThread(), GetCurrentThreadId());
}

//
// Usage: SetThreadName (-1, "MainThread");
// from http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vsdebug/html/vxtsksettingthreadname.asp
//
typedef struct tagTHREADNAME_INFO {
  DWORD dwType; // must be 0x1000
  LPCSTR szName; // pointer to name (in user addr space)
  DWORD dwThreadID; // thread ID (-1=caller thread)
  DWORD dwFlags; // reserved for future use, must be zero
}
THREADNAME_INFO;

void Thread::set_name(const char* thread_name) {
  THREADNAME_INFO info;
  info.dwType = 0x1000;
  info.szName = thread_name;
  info.dwThreadID = id;
  info.dwFlags = 0;

  __try {
    RaiseException
    (
      0x406D1388,
      0,
      sizeof(info) / sizeof(DWORD),
      reinterpret_cast<const ULONG_PTR*>(&info)
    );
  }
  __except(EXCEPTION_CONTINUE_EXECUTION)
  { }
}

bool Thread::setaffinity(uint16_t logical_processor_i) {
  if (id != 0) {
    return SetThreadAffinityMask(handle, (1L << logical_processor_i)) != 0;
  } else {
    return false;
  }
}

bool Thread::setaffinity(const ProcessorSet& logical_processor_set) {
  if (id != 0) {
    return SetThreadAffinityMask
           (
             handle,
             logical_processor_set
           ) != 0;
  } else {
    return false;
  }
}

bool Thread::setspecific(uintptr_t key, void* value) {
  return TlsSetValue(key, value) == TRUE;
}

void Thread::sleep(const Time& timeout) {
  Sleep(static_cast<DWORD>(timeout.ms()));
}

void Thread::yield() {
  SwitchToThread();
}
}
}
