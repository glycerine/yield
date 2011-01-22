// yield/thread/darwin/semaphore.cpp

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


#include "semaphore.hpp"

#include <unistd.h>
#include <mach/clock.h>
#include <mach/mach_init.h>
#include <mach/task.h>


namespace yield {
namespace thread {
namespace darwin {
Semaphore::Semaphore( semaphore_t sem )
  : sem( sem )
{ }

Semaphore::~Semaphore() {
  semaphore_destroy( mach_task_self(), sem );
}

Semaphore* Semaphore::create() {
  semaphore_t sem;
  if
  (
    semaphore_create
    (
      mach_task_self(),
      &sem,
      SYNC_POLICY_FIFO,
      0
    )
    == KERN_SUCCESS
  )
    return new Semaphore( sem );
  else
    return NULL;
}

void Semaphore::post() {
  semaphore_signal( sem );
}

bool Semaphore::timedwait( const Time& timeout ) {
  return semaphore_timedwait( sem, timeout ) == KERN_SUCCESS;
}

bool Semaphore::trywait() {
  mach_timespec_t timeout_m_ts = { 0, 0 };
  return semaphore_timedwait( sem, timeout_m_ts ) == KERN_SUCCESS;
}

bool Semaphore::wait() {
  return semaphore_wait( sem ) == KERN_SUCCESS;
}
}
}
}
