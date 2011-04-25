// yield/sockets/aio/sendfile_aiocb.cpp

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

#include "yield/assert.hpp"
#include "yield/exception.hpp"
#include "yield/sockets/stream_socket.hpp"
#include "yield/sockets/aio/sendfile_aiocb.hpp"

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

namespace yield {
namespace sockets {
namespace aio {
sendfileAIOCB::sendfileAIOCB(StreamSocket& socket_, fd_t fd)
  : AIOCB(socket_) {
  init(fd);

#ifdef _WIN32
  ULARGE_INTEGER uliFilePointer;
  LONG lFilePointerHigh = 0;
  uliFilePointer.LowPart
    = SetFilePointer(fd, 0, &lFilePointerHigh, FILE_CURRENT);
  if (uliFilePointer.LowPart != INVALID_SET_FILE_POINTER) {
    uliFilePointer.HighPart = lFilePointerHigh;
    set_offset(static_cast<off_t>(uliFilePointer.QuadPart));

    ULARGE_INTEGER uliFileSize;
    uliFileSize.LowPart = GetFileSize(fd, &uliFileSize.HighPart);
    if (uliFileSize.LowPart != INVALID_FILE_SIZE)
      this->nbytes = static_cast<size_t>(uliFileSize.QuadPart);
    else
      throw Exception();
  } else
    throw Exception();
#else
  off_t offset = lseek(fd, 0, SEEK_CUR);
  if (offset != static_cast<off_t>(-1)) {
    struct stat stbuf;
    if (fstat(fd, &stbuf) == 0)
      init(fd, stbuf.st_size, offset);
    else
      throw Exception();
  } else
    throw Exception();
#endif
}

sendfileAIOCB::sendfileAIOCB(
  StreamSocket& socket_,
  fd_t fd,
  off_t offset,
  size_t nbytes
) : AIOCB(socket_, offset),
    nbytes(nbytes) {
  init(fd);
}

sendfileAIOCB::~sendfileAIOCB() {
#ifdef _WIN32
  CloseHandle(fd);
#else
  close(fd);
#endif
}

StreamSocket& sendfileAIOCB::get_socket() {
  return static_cast<StreamSocket&>(AIOCB::get_socket());
}

void sendfileAIOCB::init(fd_t fd) {
#ifdef _WIN32
  if (
    !DuplicateHandle(
      GetCurrentProcess(),
      fd,
      GetCurrentProcess(),
      &this->fd,
      0,
      FALSE,
      DUPLICATE_SAME_ACCESS
    )
  )
    throw Exception();
#else
  this->fd = dup(fd);
  if (this->fd == -1)
    throw Exception();
#endif
}

std::ostream& operator<<(std::ostream& os, sendfileAIOCB& sendfile_aiocb) {
  os <<
    sendfile_aiocb.get_type_name() <<
    "(" <<
      "error=" << sendfile_aiocb.get_error() <<
      ", " <<
      "fd=" << sendfile_aiocb.get_fd() <<
      ", " <<
      "nbytes=" << sendfile_aiocb.get_nbytes() <<
      ", " <<
      "offset=" << sendfile_aiocb.get_offset() <<
      ", " <<
      "return=" << sendfile_aiocb.get_return() <<
      "," <<
      "socket=" << sendfile_aiocb.get_socket() <<
    ")";
  return os;
}
}
}
}
