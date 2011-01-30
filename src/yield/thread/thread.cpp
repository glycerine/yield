// yield/thread/thread.cpp

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

#if defined(__linux__)
#include "linux/thread.hpp"
#elif defined(__MACH__)
#include "darwin/thread.hpp"
#elif defined(__sun)
#include "sunos/thread.hpp"
#elif defined(_WIN32)
#include "win32/thread.hpp"
#else
#include "posix/thread.hpp"
#endif
#include "yield/thread/static_runnable.hpp"
#include "yield/thread/processor_set.hpp"
#include "yield/thread/thread.hpp"


namespace yield {
namespace thread {
Thread::Thread(Runnable& runnable) {
  init(runnable);
}

Thread::Thread(void (*run)(void*), void* context) {
  StaticRunnable* static_runnable = new StaticRunnable(run, context);
  init(*static_runnable);
  StaticRunnable::dec_ref(*static_runnable);
}

Thread::~Thread() {
  delete pimpl;
}

bool Thread::cancel() {
  return pimpl->cancel();
}

Runnable* Thread::get_runnable() const {
  return pimpl->get_runnable();
}

void* Thread::getspecific(uintptr_t key) {
  return pimpl->getspecific(key);
}

void Thread::init(Runnable& runnable) {
#if defined(__linux__)
  pimpl = new linux::Thread(runnable);
#elif defined(__MACH__)
  pimpl = new darwin::Thread(runnable);
#elif defined(__sun)
  pimpl = new sunos::Thread(runnable);
#elif defined(_WIN32)
  pimpl = new win32::Thread(runnable);
#else
  pimpl = new posix::Thread(runnable);
#endif
}

bool Thread::is_running() const {
  return pimpl->is_running();
}

bool Thread::join() {
  return pimpl->join();
}

uintptr_t Thread::key_create() {
  return pimpl->key_create();
}

bool Thread::key_delete(uintptr_t key) {
  return pimpl->key_delete(key);
}

void Thread::nanosleep(const Time& timeout) {
  return pimpl->nanosleep(timeout);
}

auto_Object<Thread> Thread::self() {
#if defined(__linux__)
  return new Thread(linux::Thread::self());
#elif defined(__MACH__)
  return new Thread(darwin::Thread::self();
#elif defined(__sun)
  return new Thread(sunos::Thread::self());
#elif defined(_WIN32)
  return new Thread(win32::Thread::self());
#else
  return new Thread(posix::Thread::self());
#endif
}

bool Thread::setaffinity(uint16_t logical_processor_i) {
#if defined(__linux__) || defined(__sun) || defined(_WIN32)
  return pimpl->setaffinity(logical_processor_i);
#endif
}

bool Thread::setaffinity(const ProcessorSet& logical_processor_set) {
#if defined(__linux__) || defined(__sun) || defined(_WIN32)
  return pimpl->setaffinity(*logical_processor_set.pimpl);
#endif
}

void Thread::set_name(const char* name) {
#ifdef _WIN32
  return pimpl->set_name(name);
#endif
}

bool Thread::setspecific(uintptr_t key, intptr_t value) {
  return setspecific(key, reinterpret_cast<void*>(value));
}

bool Thread::setspecific(uintptr_t key, uintptr_t value) {
  return setspecific(key, reinterpret_cast<void*>(value));
}

bool Thread::setspecific(uintptr_t key, void* value) {
  return pimpl->setspecific(key, value);
}

void Thread::yield() {
  return pimpl->yield();
}
}
}
