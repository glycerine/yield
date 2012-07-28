// yield/debug.hpp

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

#ifndef _YIELD_DEBUG_HPP_
#define _YIELD_DEBUG_HPP_

#include "yield/config.hpp"

#include <cstdio> // for snprintf
#include <exception> // for std::exception

#if defined(_WIN32)
extern "C" {
  __declspec(dllimport) void __stdcall DebugBreak();
}
#endif

#ifdef _DEBUG
#if defined(_WIN32)
#define debug_break() DebugBreak()
#elif defined(__GNUC__)
#define debug_break() asm("int $3")
#else
#define debug_break() *reinterpret_cast<int*>(0) = 0xabadcafe
#endif

#define debug_assert_true(a) { if (!((a) == true)) debug_break(); }
#define debug_assert(a) debug_assert_true(a)
#define debug_assert_eq(a, b) debug_assert_true((a) == (b));
#define debug_assert_false(a) debug_assert_true((a) == false);
#define debug_assert_ge(a, b) debug_assert_true((a) >= (b));
#define debug_assert_gt(a, b) debug_assert_true((a) > (b));
#define debug_assert_le(a, b) debug_assert_true((a) <= (b));
#define debug_assert_lt(a, b) debug_assert_true((a) < (b));
#define debug_assert_ne(a, b) debug_assert_true((a) != (b));
#else
#define debug_break()
#define debug_assert_true(a)
#define debug_assert(a)
#define debug_assert_eq(a, b)
#define debug_assert_false(a)
#define debug_assert_ge(a, b)
#define debug_assert_gt(a, b)
#define debug_assert_le(a, b)
#define debug_assert_lt(a, b)
#define debug_assert_ne(a, b)
#endif

#endif
