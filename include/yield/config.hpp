// yield/config.hpp

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


#ifndef _YIELD_CONFIG_HPP_
#define _YIELD_CONFIG_HPP_


#if defined(__FreeBSD__)
  #ifdef _FILE_OFFSET_BITS
    #error
  #endif
  #define _FILE_OFFSET_BITS 64
#elif defined(__linux__)
  #ifdef _FILE_OFFSET_BITS
    #error
  #endif
  #define _FILE_OFFSET_BITS 64
  #undef linux
#elif defined(__MACH__)
  #ifdef _FILE_OFFSET_BITS
    #error
  #endif
  #define _FILE_OFFSET_BITS 64
#elif defined(__sun)
  #define upgrade_the_compiler_to_use_STL 1
  #define _REENTRANT
#elif defined(_WIN32)
  #define _CRT_DISABLE_PERFCRIT_LOCKS 1
  #ifndef UNICODE
    #define UNICODE 1
  #endif
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN 1
  #endif
#endif


#ifdef unix
  #undef unix
#endif


#endif
