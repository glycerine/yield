// yield/thread/fiber.hpp

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


#ifndef _YIELD_THREAD_FIBER_HPP_
#define _YIELD_THREAD_FIBER_HPP_


#include "yield/auto_object.hpp"
#include "yield/object.hpp"


namespace yield {
namespace thread {
#if defined(_WIN32)
namespace win32 {
class Fiber;
}
#elif defined(YIELD_HAVE_UNIX_PTH)
namespace unix {
class Fiber;
}
#endif


#if defined(_WIN32) || defined(YIELD_HAVE_UNIX_PTH)
class Fiber : public Object {
public:
  Fiber(Runnable& runnable);
  ~Fiber();

  void* getspecific(uintptr_t key);
  uintptr_t key_create();
  bool key_delete(uintptr_t key);
  static auto_Object<Fiber> self();
  bool setspecific(uintptr_t key, void* value);
  void yield();
  void yield(Fiber& to_fiber);

private:
#if defined(_WIN32)
  Fiber(win32::Fiber* pimpl)
#elif defined(YIELD_HAVE_UNIX_PTH)
  Fiber(unix::Fiber* pimpl)
#endif
    : pimpl(pimpl)
  { }

private:
#if defined(_WIN32)
  win32::Fiber* pimpl;
#elif defined(YIELD_HAVE_UNIX_PTH)
  unix::Fiber* pimpl;
#endif
};
#endif
}
}


#endif
