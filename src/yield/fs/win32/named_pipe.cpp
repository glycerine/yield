// yield/fs/win32/named_pipe.cpp

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


#include "named_pipe.hpp"

#include <Windows.h>


namespace yield {
namespace fs {
namespace win32 {
NamedPipe::NamedPipe( HANDLE hNamedPipe )
  : File( hNamedPipe ) {
  connected = false;
}

bool NamedPipe::connect() {
#ifdef _WIN32
  if
  (
    ConnectNamedPipe( *this, NULL ) != 0
    ||
    GetLastError() == ERROR_PIPE_CONNECTED
  ) {
    connected = true;
    return true;
  } else
    return false;
#else
  DebugBreak();
  return false;
#endif
}

bool NamedPipe::datasync() {
  return true;
}

ssize_t NamedPipe::read( void* buf, size_t buflen ) {
  if ( connected || connect() )
    return File::read( buf, buflen );
  else
    return -1;
}

ssize_t NamedPipe::readv( const iovec* iov, int iovlen ) {
  if ( iovlen == 1 )
    return read( iov[0].iov_base, iov[0].iov_len );
  else {
    SetLastError( ERROR_NOT_SUPPORTED );
    return -1;
  }
}

bool NamedPipe::set_blocking_mode( bool blocking_mode ) {
  DWORD lpMode = blocking_mode ? PIPE_WAIT : PIPE_NOWAIT;
  return SetNamedPipeHandleState( *this, &lpMode, NULL, NULL ) == TRUE;
}

bool NamedPipe::sync() {
  return true;
}

ssize_t NamedPipe::write( const void* buf, size_t buflen ) {
  if ( connected || connect() )
    return File::write( buf, buflen );
  else
    return -1;
}

ssize_t NamedPipe::writev( const iovec* iov, int iovlen ) {
  if ( connected || connect() )
    return File::writev( iov, iovlen );
  else
    return -1;
}
}
}
}
