// yield/types.hpp

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


#ifndef _YIELD_TYPES_HPP_
#define _YIELD_TYPES_HPP_


#include "yield/config.hpp"

#define __STDC_LIMIT_MACROS 1
#include <stdint.h>

#ifndef _WIN32
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
using std::wstring;
#include <vector>
using std::vector;


#ifndef DLLEXPORT
  #ifdef _WIN32
    #define DLLEXPORT extern "C" __declspec(dllexport)
  #else
    #if defined( __GNUC__ ) && __GNUC__ >= 4
      #define DLLEXPORT extern "C" __attribute__ ( visibility( "default" ) )
    #else
      #define DLLEXPORT extern "C"
    #endif
  #endif
#endif

#ifndef IN
  #define IN
#endif
#ifndef INOUT
  #define INOUT
#endif
#ifndef OUT
  #define OUT
#endif


namespace yield
{
  #if defined(_WIN64)
    typedef __int64 atomic_t;
  #elif defined(_WIN32)
    typedef long atomic_t;
  #else
    typedef intptr_t atomic_t;
  #endif

  #ifdef _WIN32
    typedef void* fd_t;
    const static fd_t INVALID_FD = reinterpret_cast<fd_t>( -1 );
  #else
    typedef int fd_t;
    const static fd_t INVALID_FD = -1;
  #endif

  #ifdef _WIN32
    typedef int16_t gid_t; // The st_gid in struct stat is a short.
  #endif

  #ifdef _WIN32
    struct iovec { size_t iov_len; void* iov_base; };
  #endif

  #ifdef _WIN32
    typedef uint16_t mode_t;
  #endif

  #ifdef _WIN32
    typedef int32_t pid_t;
  #endif

  #if defined(_WIN64)
    typedef uint64_t socket_t;
    typedef int socklen_t;
  #elif defined(_WIN32)
    typedef uint32_t socket_t;
    typedef int socklen_t;
  #else
    typedef int socket_t;
  #endif

  #ifdef _WIN32
    typedef intptr_t ssize_t;
  #endif

  #ifdef _WIN32
    typedef int16_t uid_t; // The st_uid in struct stat is a short.
  #endif


  inline socket_t fd_to_socket( fd_t fd )
  {
    #ifdef _WIN32
      return reinterpret_cast<socket_t>( fd );
    #else
      return fd;
    #endif
  }

  inline fd_t socket_to_fd( socket_t socket_ )
  {
    #ifdef _WIN32
      return reinterpret_cast<fd_t>( socket_ );
    #else
      return socket_;
    #endif
  }

  #ifndef _WIN32
    inline void
    memcpy_s
    (
      void* dest,
      size_t dest_len,
      const void* src,
      size_t src_len
    )
    {
      memcpy( dest, src, src_len );
    }
  #endif
}


#endif
