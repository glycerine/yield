// yield/thread/fiber.cpp

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


#if defined(_WIN32)
#include "win32/fiber.hpp"
#elif defined(YIELD_HAVE_UNIX_PTH)
#include "unix/fiber.hpp"
#endif
#include "yield/thread/fiber.hpp"


namespace yield {
namespace thread {
#if defined(_WIN32) || defined(YIELD_HAVE_UNIX_PTH)
Fiber::Fiber(Runnable& runnable) {
#if defined(_WIN32)
  pimpl = new win32::Fiber(runnable);
#elif defined(YIELD_HAVE_UNIX_PTH)
  pimpl = new unix::Fiber(runnable);
#endif
}

Fiber::~Fiber() {
  delete pimpl;
}

void* Fiber::getspecific(uintptr_t key) {
  return pimpl->getspecific(key);
}

uintptr_t Fiber::key_create() {
  return pimpl->key_create();
}

bool Fiber::key_delete(uintptr_t key) {
  return pimpl->key_delete(key);
}

auto_Object<Fiber> Fiber::self() {
#if defined(_WIN32)
  return new Fiber(win32::Fiber::self());
#elif defined(YIELD_HAVE_UNIX_PTH)
  return new Fiber(unix::Fiber::self());
#endif
}

bool Fiber::setspecific(uintptr_t key, void* value) {
  return pimpl->setspecific(key, value);
}

void Fiber::yield() {
  return pimpl->yield();
}

void Fiber::yield(Fiber& to_fiber) {
  return pimpl->yield(*to_fiber.pimpl);
}
#endif
}
}
