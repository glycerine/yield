// yield/thread/semaphore.hpp

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

#ifndef _YIELD_THREAD_SEMAPHORE_HPP_
#define _YIELD_THREAD_SEMAPHORE_HPP_

#include "yield/config.hpp"

#if defined(__MACH__)
#include <mach/semaphore.h>
#elif !defined(_WIN32)
#include <semaphore.h>
#endif

namespace yield {
class Time;

namespace thread {
/**
  Counting semaphore synchronization primitive.
*/
class Semaphore {
public:
  Semaphore();
  ~Semaphore();

public:
#ifdef _WIN32
  /**
    Get the underlying HANDLE to this semaphore.
    @return the underlying HANDLE to this semaphore
  */
  operator void* () const {
    return hSemaphore;
  }
#endif

public:
  /**
    Post to the semaphore, increasing its count by one and waking up a waiter.
  */
  void post();

  /**
    Wait for a post to the semaphore for the specified timeout, decrementing the
      semaphore's count by one if there was a post.
    @param timeout time to wait for a post
    @return true if there was a post within the specified timeout
  */
  bool timedwait(const Time& timeout);

  /**
    Check for a post to the semaphore, decrementig the semaphore's count by one
      if there was a post but not blocking if there was none.
    @return true if there was a post to the semaphore
  */
  bool trywait();

  /**
    Wait indefinitely for a post to the semaphore, decrementing the semaphore's
      count by one on a successful wait
    @return true if there was a post to the semaphore
  */
  bool wait();

private:
  Semaphore(void* hSemaphore);

private:
#if defined(__MACH__)
  semaphore_t sem;
#elif defined(_WIN32)
  void* hSemaphore;
#else
  sem_t sem;
#endif
};
}
}

#endif
