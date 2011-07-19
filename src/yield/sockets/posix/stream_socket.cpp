// yield/sockets/posix/stream_socket.cpp

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

#include "yield/sockets/stream_socket.hpp"

#include <errno.h>
#ifdef __linux__
#include <sys/sendfile.h>
#endif
#include <sys/socket.h>

namespace yield {
namespace sockets {
const int StreamSocket::TYPE = SOCK_STREAM;

const int StreamSocket::Option::KEEPALIVE = SO_KEEPALIVE;
const int StreamSocket::Option::LINGER = SO_LINGER;

StreamSocket* StreamSocket::accept(SocketAddress& peername) {
  socklen_t peernamelen = peername.len();

  socket_t peer_socket = ::accept(*this, peername, &peernamelen);

  if (peer_socket != -1) {
    return dup2(peer_socket);
  } else {
    return NULL;
  }
}

bool StreamSocket::listen() {
  return ::listen(*this, SOMAXCONN) != -1;
}

ssize_t StreamSocket::sendfile(fd_t fd, off_t offset, size_t nbytes) {
#if defined(__linux__)
  off_t* p_offset = &offset;
  ssize_t sendfile_ret = ::sendfile(*this, fd, p_offset, nbytes);
  if (sendfile_ret > 0) {
    return sendfile_ret;
  } else if (sendfile_ret == 0 && errno == EWOULDBLOCK) { // Linux "feature"
    return -1;
  } else {
    return sendfile_ret;
  }
#elif defined(__FreeBSD__)
  off_t sbytes;
  int sendfile_ret = ::sendfile(fd, *this, offset, nbytes, NULL, &sbytes, 0);
  if (sendfile_ret == 0) {
    return sbytes;
  } else {
    return sendfile_ret;
  }
#elif defined(__MACH__)
  off_t len = nbytes;
  int sendfile_ret = ::sendfile(fd, *this, offset, &len, NULL, 0);
  if (sendfile_ret == 0) {
    return static_cast<ssize_t>(len);
  } else {
    return sendfile_ret;
  }
#else
#error
#endif
}

bool StreamSocket::setsockopt(int option_name, int option_value) {
  if (option_name == Option::LINGER) {
    linger optval;
    if (option_value > 0) {
      optval.l_onoff = 1;
      optval.l_linger = static_cast<u_short>(option_value);
    } else {
      optval.l_onoff = 0;
      optval.l_linger = 0;
    }

    return ::setsockopt(
             *this,
             SOL_SOCKET,
             SO_LINGER,
             reinterpret_cast<char*>(&optval),
             static_cast<int>(sizeof(optval))
           ) == 0;
  } else {
    return Socket::setsockopt(option_name, option_value);
  }
}

bool StreamSocket::shutdown(bool shut_rd, bool shut_wr) {
  int how;
  if (shut_rd && shut_wr) {
    how = SHUT_RDWR;
  } else if (shut_rd) {
    how = SHUT_RD;
  } else if (shut_wr) {
    how = SHUT_WR;
  } else {
    return false;
  }

  return ::shutdown(*this, how) != -1;
}

bool StreamSocket::want_accept() const {
  return errno == EWOULDBLOCK;
}

bool StreamSocket::want_connect() const {
  switch (errno) {
  case EALREADY:
  case EINPROGRESS:
  case EWOULDBLOCK:
    return true;
  default:
    return false;
  }
}
}
}
