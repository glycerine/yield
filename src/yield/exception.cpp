// yield/exception.cpp

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

#include "yield/debug.hpp"
#include "yield/exception.hpp"

#ifdef _WIN32
#include <lmerr.h>
#include <Windows.h>
#else
#include <errno.h>
#endif

namespace yield {
Exception::Exception()
  : error_message(NULL) {
  set_error_code(get_last_error_code());
}

Exception::Exception(uint32_t error_code)
  : error_message(NULL) {
  set_error_code(error_code);
}

Exception::Exception(const char* error_message)
  : error_code(0), error_message(NULL) {
  if (error_message != NULL) {
    set_error_message(error_message);
  }
}

Exception::Exception(const string& error_message)
  : error_code(0), error_message(NULL) {
  set_error_message(error_message);
}

Exception::Exception(uint32_t error_code, const char* error_message)
  : error_message(NULL) {
  set_error_code(error_code);
  if (error_message != NULL) {
    set_error_message(error_message);
  }
}

Exception::Exception(uint32_t error_code, const string& error_message)
  : error_message(NULL) {
  set_error_code(error_code);
  set_error_message(error_message);
}

Exception::Exception(const Exception& other)
  : error_code(other.error_code), error_message(NULL) {
  if (other.error_message != NULL) {
    set_error_message(other.error_message);
  }
}

Exception::~Exception() throw() {
#ifdef _WIN32
  LocalFree(error_message);
#else
  delete [] error_message;
#endif
}

const char* Exception::get_error_message() const throw() {
  if (error_message != NULL) {
    return error_message;
  } else if (error_code != 0) {
#ifdef _WIN32
    DWORD dwMessageLength
    = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error_code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&const_cast<Exception*>(this)->error_message),
        0,
        NULL
      );

    if (dwMessageLength > 0) {
      if (dwMessageLength > 2) {
        error_message[dwMessageLength - 2] = 0;  // Cut off trailing \r\n
      }

      return error_message;
    } else if (error_code >= NERR_BASE || error_code <= MAX_NERR) {
      HMODULE hModule
      = LoadLibraryEx(
          TEXT("netmsg.dll"),
          NULL,
          LOAD_LIBRARY_AS_DATAFILE
        ); // Let's hope this is cheap..

      if (hModule != NULL) {
        dwMessageLength
        = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            hModule,
            error_code,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            error_message,
            0,
            NULL
          );

        if (dwMessageLength > 0) {
          FreeLibrary(hModule);

          if (dwMessageLength > 2) { // Cut off trailing \r\n
            error_message[dwMessageLength - 2] = 0;
          }

          return error_message;
        } else {
          FreeLibrary(hModule);
        }
      }
    }

    // Could not get an error_message for error_code from FormatMessage
    // Set error_message to a dummy value so we don't have to
    // try this again
    const_cast<Exception*>(this)->error_message
    = static_cast<char*>(LocalAlloc(LMEM_FIXED, 19));
    sprintf_s(error_message, 19, "errno = %u", error_code);
    return error_message;
#else
    // strerror_r is more or less unusable in a portable way,
    // thanks to the GNU-specific implementation.
    // You have to define _XOPEN_SOURCE to get the POSIX implementation,
    // but that apparently breaks libstdc++.
    // So we just use strerror.
    const_cast<Exception*>(this)
    ->set_error_message(strerror(error_code));
    return error_message;
#endif
  } else {
    return "(unknown)";
  }
}

uint32_t Exception::get_last_error_code() {
#ifdef _WIN32
  return static_cast<uint32_t>(GetLastError());
#else
  return static_cast<uint32_t>(errno);
#endif
}

void Exception::set_error_code(uint32_t error_code) {
  this->error_code = error_code;
}

void Exception::set_error_message(const char* error_message) {
  debug_assert_ne(error_message, NULL);

#ifdef _WIN32
  LocalFree(this->error_message);
#else
  delete [] this->error_message;
#endif

  size_t error_message_len = strlen(error_message);
#ifdef _WIN32
  this->error_message
  = static_cast<char*>(
      LocalAlloc(LMEM_FIXED, error_message_len + 1)
    );
#else
  this->error_message = new char[error_message_len + 1];
#endif
  memcpy(this->error_message, error_message, error_message_len + 1);
}

void Exception::set_error_message(const string& error_message) {
#ifdef _WIN32
  LocalFree(this->error_message);
#else
  delete [] this->error_message;
#endif

#ifdef _WIN32
  this->error_message
  = static_cast<char*>(
      LocalAlloc(LMEM_FIXED, error_message.size() + 1)
    );
#else
  this->error_message = new char[error_message.size() + 1];
#endif

  memcpy_s(
    this->error_message,
    error_message.size() + 1,
    error_message.c_str(),
    error_message.size() + 1
  );
}

void Exception::set_last_error_code(uint32_t error_code) {
#ifdef _WIN32
  SetLastError(error_code);
#else
  errno = error_code;
#endif
}

const char* Exception::what() const throw() {
  return get_error_message();
}
}
