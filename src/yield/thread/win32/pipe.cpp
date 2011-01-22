// yield/thread/win32/pipe.cpp

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


#include "yield/config.hpp"
#include "yield/exception.hpp"
#include "yield/thread/pipe.hpp"

#include <Windows.h>


namespace yield {
namespace thread {
bool Pipe::pipe(fd_t ends[2]) {
  SECURITY_ATTRIBUTES pipe_security_attributes;
  pipe_security_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
  pipe_security_attributes.bInheritHandle = TRUE;
  pipe_security_attributes.lpSecurityDescriptor = NULL;
  if (CreatePipe(&ends[0], &ends[1], &pipe_security_attributes, 0)) {
    if
    (
      SetHandleInformation(ends[0], HANDLE_FLAG_INHERIT, 0)
      &&
      SetHandleInformation(ends[1], HANDLE_FLAG_INHERIT, 0)
    )
      return true;
    else {
      CloseHandle(ends[0]);
      CloseHandle(ends[1]);
    }
  }

  return false;
}


bool Pipe::End::close() {
  if (fd != INVALID_HANDLE_VALUE) {
    if (CloseHandle(fd)) {
      fd = INVALID_HANDLE_VALUE;
      return true;
    } else
      return false;
  } else
    return false;
}

ssize_t Pipe::End::read(void* buf, size_t buflen) {
  DWORD dwBytesRead;
  if
  (
    ReadFile
    (
      *this,
      buf,
      static_cast<DWORD>(buflen),
      &dwBytesRead,
      NULL
    )
  )
    return static_cast<ssize_t>(dwBytesRead);
  else
    return -1;
}

ssize_t Pipe::End::readv(const iovec* iov, int iovlen) {
  if (iovlen == 1)
    return read(iov[0].iov_base, iov[0].iov_len);
  else {
    SetLastError(ERROR_NOT_SUPPORTED);
    return -1;
  }
}

bool Pipe::End::set_blocking_mode(bool blocking_mode) {
  SetLastError(ERROR_NOT_SUPPORTED);
  return false;
}

ssize_t Pipe::End::write(const void* buf, size_t buflen) {
  DWORD dwBytesWritten;
  if
  (
    WriteFile
    (
      *this,
      buf,
      static_cast<DWORD>(buflen),
      &dwBytesWritten,
      NULL
    )
  )
    return static_cast<ssize_t>(dwBytesWritten);
  else
    return -1;
}

ssize_t Pipe::End::writev(const iovec* iov, int iovlen) {
  if (iovlen == 1)
    return write(iov[0].iov_base, iov[0].iov_len);
  else {
    string buf;

    for (int iov_i = 0; iov_i < iovlen; iov_i++) {
      buf.append
      (
        static_cast<char*>(iov[iov_i].iov_base),
        iov[iov_i].iov_len
      );
    }

    return write(buf.data(), buf.size());
  }
}
}
}
