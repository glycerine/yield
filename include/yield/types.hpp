// yield/types.hpp

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

#ifndef _YIELD_TYPES_HPP_
#define _YIELD_TYPES_HPP_

#include "yield/config.hpp"

#define __STDC_LIMIT_MACROS 1
#include <stdint.h>

#ifdef _WIN32
struct iovec {
  size_t iov_len;
  void* iov_base;
};

typedef int64_t _off_t;
typedef int64_t off_t;
#define _OFF_T_DEFINED
typedef intptr_t ssize_t;
#else
#include <sys/types.h>
#include <sys/uio.h> // For struct iovec
#ifdef __sun
// Solaris's unistd.h defines a function yield that conflicts with the
// namespace yield. This ugliness is necessary to get around that.
#ifdef __XOPEN_OR_POSIX
#error
#endif
#define __XOPEN_OR_POSIX 1
#ifdef __EXTENSIONS__
#undef __EXTENSIONS__
#include <unistd.h> // For ssize_t
#define __EXTENSIONS__ 1
#else
#include <unistd.h> // For ssize_t
#endif
#undef __XOPEN_OR_POSIX
#endif
#endif

#ifndef _WIN32
#include <cstring>
using std::memcpy;
#endif
#include <string>
using std::string;
#include <vector>
using std::vector;

#ifndef IN
#define IN
#endif
#ifndef INOUT
#define INOUT
#endif
#ifndef OUT
#define OUT
#endif

namespace yield {
/**
  Platform-specific type of integer operands in atomic operations (atomic_*).
*/
#if defined(_WIN64)
typedef __int64 atomic_t;
#elif defined(_WIN32)
typedef long atomic_t;
#else
typedef intptr_t atomic_t;
#endif

/**
  Platform-specific file descriptor type (e.g., int on POSIX systems).
*/
#ifdef _WIN32
typedef void* fd_t;
const static fd_t INVALID_FD = reinterpret_cast<fd_t>(-1);
#else
typedef int fd_t;
const static fd_t INVALID_FD = -1;
#endif

/**
  Platform-specific socket descriptor type (e.g., int on POSIX systems).
*/
#if defined(_WIN64)
typedef uint64_t socket_t;
typedef int socklen_t;
#elif defined(_WIN32)
typedef uint32_t socket_t;
typedef int socklen_t;
#else
typedef int socket_t;
#endif
}

#ifndef _WIN32
/**
  Checked variant of memcpy.
  @param dest memory location to copy to
  @param dest_len length of the memory region pointed to by dest
  @param src memory location to copy from
  @param src_len length of the memory region pointed to by src
*/
inline void
memcpy_s(
  void* dest,
  size_t dest_len,
  const void* src,
  size_t src_len
) {
  memcpy(dest, src, src_len);
}
#endif

#endif
