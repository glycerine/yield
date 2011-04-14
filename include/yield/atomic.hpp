// yield/atomic.hpp

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

#ifndef _YIELD_ATOMIC_HPP_
#define _YIELD_ATOMIC_HPP_

#include "yield/types.hpp"

#if defined(_WIN64)
extern "C"
{
  __int64 _InterlockedCompareExchange64
  (
    volatile __int64* cur_value,
    __int64 new_value,
    __int64 old_value
  );

  __int64 _InterlockedIncrement64(volatile __int64* cur_value);
  __int64 _InterlockedDecrement64(volatile __int64* cur_value);
}
#elif defined(_WIN32)
extern "C"
{
  __declspec(dllimport) long __stdcall
  InterlockedCompareExchange
  (
    volatile long* cur_value,
    long new_value,
    long old_value
  );

  __declspec(dllimport) long __stdcall
  InterlockedIncrement
  (
    volatile long* cur_value
  );

  __declspec(dllimport) long __stdcall
  InterlockedDecrement
  (
    volatile long* cur_value
  );
}
#elif defined(__sun)
#include <atomic.h>
#elif defined(__arm__)
// gcc atomic builtins are not defined on ARM
#elif defined(__GNUC__) && \
      ( ( __GNUC__ == 4 && __GNUC_MINOR__ >= 1 ) || __GNUC__ > 4 )
#define HAVE_GNUC_ATOMIC_BUILTINS 1
#endif


namespace yield {
static inline atomic_t
atomic_cas
(
  volatile atomic_t* cur_value,
  atomic_t new_value,
  atomic_t old_value
) {
#if defined(_WIN64)
  return _InterlockedCompareExchange64(cur_value, new_value, old_value);
#elif defined(_WIN32)
  return InterlockedCompareExchange(cur_value, new_value, old_value);
#elif defined(__sun)
#if sizeof( atomic_t ) == sizeof( uint64_t )
  return atomic_cas_64
         (
           reinterpret_cast<volatile uint64_t*>(cur_value),
           static_cast<uint64_t>(old_value),
           static_cast<uint64_t>(new_value)
         );
#else
  return atomic_cas_32
         (
           reinterpret_cast<volatile uint32_t*>(cur_value),
           static_cast<uint32_t>(old_value),
           static_cast<uint32_t>(new_value)
         );
#endif
#elif defined(HAVE_GNUC_ATOMIC_BUILTINS)
  return __sync_val_compare_and_swap(cur_value, old_value, new_value);
#elif defined(__arm__)
#if __ARM_ARCH__ >= 6
  atomic_t prev;
  asm volatile
  (
    "@ atomic_cmpxchg\n"
    "ldrex  %1, [%2]\n"
    "mov    %0, #0\n"
    "teq    %1, %3\n"
    "strexeq %0, %4, [%2]\n"
    : "=&r"(prev), "=&r"(old_value)
    : "r"(cur_value), "Ir"(old_value), "r"(new_value)
    : "cc"
  );
  return prev;
#else // ARM architectures < 6 are uniprocessor only
  if (*cur_value == old_value) {
    *cur_value = new_value;
    return old_value;
  } else
    return *cur_value;
#endif
#elif defined(__i386__)
  atomic_t prev;
  asm volatile("lock\n"
               "cmpxchgl %1,%2\n"
               : "=a"(prev)
               : "r"(new_value), "m"(*cur_value) , "0"(old_value)
               : "memory"
              );
  return prev;
#elif defined(__ppc__)
  atomic_t prev;
  asm volatile("          \n\
              1:  ldarx   %0,0,%2 \n\
              cmpd    0,%0,%3 \n\
              bne     2f    \n\
              stdcx.  %4,0,%2 \n\
              bne-    1b    \n\
              sync\n"
               "2:"
               : "=&r"(prev), "=m"(*cur_value)
               : "r"(cur_value), "r"(old_value), "r"(new_value),
               "m"(*cur_value)
               : "cc", "memory"
              );
  return prev;
#elif defined(__x86_64__)
  atomic_t prev;
  asm volatile("lock\n"
               "cmpxchgq %1,%2\n"
               : "=a"(prev)
               : "r"(new_value), "m"(*cur_value) , "0"(old_value)
               : "memory"
              );
  return prev;
#endif
}

static inline atomic_t atomic_dec(volatile atomic_t* cur_value) {
#if defined(_WIN64)
  return _InterlockedDecrement64(cur_value);
#elif defined(_WIN32)
  return InterlockedDecrement(cur_value);
#elif defined(__sun)
#if sizeof( atomic_t ) == sizeof( uint64_t )
  return atomic_dec_64_nv
         (
           reinterpret_cast<volatile uint64_t*>(cur_value)
         );
#else
  return atomic_dec_32_nv
         (
           reinterpret_cast<volatile uint32_t*>(cur_value)
         );
#endif
#elif defined(HAVE_GNUC_ATOMIC_BUILTINS)
  return __sync_sub_and_fetch(cur_value, 1);
#else
  atomic_t old_value, new_value;

  do {
    old_value = *cur_value;
    new_value = old_value - 1;
  } while (atomic_cas(cur_value, new_value, old_value) != old_value);

  return new_value;
#endif
}

static inline atomic_t atomic_inc(volatile atomic_t* cur_value) {
#if defined(_WIN64)
  return _InterlockedIncrement64(cur_value);
#elif defined(_WIN32)
  return InterlockedIncrement(cur_value);
#elif defined(__sun)
#if sizeof( atomic_t ) == sizeof( uint64_t )
  return atomic_inc_64_nv
         (
           reinterpret_cast<volatile uint64_t*>(cur_value)
         );
#else
  return atomic_inc_32_nv
         (
           reinterpret_cast<volatile uint32_t*>(cur_value)
         );
#endif
#elif defined(HAVE_GNUC_ATOMIC_BUILTINS)
  return __sync_add_and_fetch(cur_value, 1);
#else
  atomic_t old_value, new_value;

  do {
    old_value = *cur_value;
    new_value = old_value + 1;
  } while (atomic_cas(cur_value, new_value, old_value) != old_value);

  return new_value;
#endif
}
}

#endif
