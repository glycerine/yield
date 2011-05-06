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

#if defined(__linux__)
#include "yield/thread/linux/thread.hpp"
#elif defined(__MACH__)
#include "yield/thread/darwin/thread.hpp"
#elif defined(__sun)
#include "yield/thread/sunos/thread.hpp"
#elif defined(_WIN32)
#include "yield/thread/win32/thread.hpp"
#else
#include "yield/thread/posix/thread.hpp"
#endif

namespace yield {
namespace thread {
#if defined(__linux__)
typedef linux::Thread Thread;
#elif defined(__MACH__)
typedef darwin::Thread Thread;
#elif defined(__sun)
typedef sunos::Thread Thread;
#elif defined(_WIN32)
typedef win32::Thread Thread;
#else
typedef posix::Thread Thread;
#endif
}
}

#endif
