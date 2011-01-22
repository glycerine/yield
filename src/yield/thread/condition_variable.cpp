// yield/thread/condition_variable.cpp

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


#ifdef _WIN32
#include "win32/condition_variable.hpp"
#else
#include "posix/condition_variable.hpp"
#endif
#include "yield/thread/condition_variable.hpp"


namespace yield {
namespace thread {
ConditionVariable::ConditionVariable() {
#ifdef _WIN32
  pimpl = new win32::ConditionVariable;
#else
  pimpl = new posix::ConditionVariable;
#endif
}

ConditionVariable::~ConditionVariable() {
  delete pimpl;
}

void ConditionVariable::broadcast() {
  return pimpl->broadcast();
}

bool ConditionVariable::lock_mutex() {
  return pimpl->lock_mutex();
}

void ConditionVariable::signal() {
  return pimpl->signal();
}

bool ConditionVariable::timedwait( const Time& timeout ) {
  return pimpl->timedwait( timeout );
}

bool ConditionVariable::trylock_mutex() {
  return pimpl->trylock_mutex();
}

void ConditionVariable::unlock_mutex() {
  return pimpl->unlock_mutex();
}

bool ConditionVariable::wait() {
  return pimpl->wait();
}
}
}
