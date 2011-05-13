// yield/sockets/datagram_socket.hpp

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

#ifndef _YIELD_SOCKETS_DATAGRAM_SOCKET_HPP_
#define _YIELD_SOCKETS_DATAGRAM_SOCKET_HPP_

#include "yield/sockets/socket.hpp"

namespace yield {
namespace sockets {
class DatagramSocket : public Socket {
public:
  const static int TYPE; // SOCK_DGRAM

public:
  DatagramSocket(int domain, int protocol = PROTOCOL_DEFAULT)
    : Socket(domain, TYPE, protocol)
  { }

  virtual ~DatagramSocket() { }

public:
  ssize_t
  recvfrom(
    Buffer& buffer,
    const MessageFlags& flags,
    SocketAddress& peername
  ) {
    if (buffer.get_next_buffer() == NULL) {
      iovec iov = buffer.as_read_iovec();
      ssize_t recv_ret = recvfrom(iov.iov_base, iov.iov_len, flags, peername);
      if (recv_ret > 0)
        buffer.put(NULL, static_cast<size_t>(recv_ret));
      return recv_ret;
    } else {
      vector<iovec> iov;
      Buffers::as_read_iovecs(buffer, iov);
      ssize_t recv_ret = recvmsg(&iov[0], iov.size(), flags, peername);
      if (recv_ret > 0)
        Buffers::put(buffer, NULL, static_cast<size_t>(recv_ret));
      return recv_ret;
    }
  }

  virtual ssize_t
  recvfrom(
    void* buf,
    size_t len,
    const MessageFlags& flags,
    SocketAddress& peername
  );

  virtual ssize_t
  recvmsg(
    const iovec* iov,
    int iovlen,
    const MessageFlags& flags,
    SocketAddress& peername
  );

public:
  virtual ssize_t
  sendmsg(
    const iovec* iov,
    int iovlen,
    const MessageFlags& flags,
    const SocketAddress& peername
  );

  ssize_t
  sendto(
    const Buffer& buffer,
    const MessageFlags& flags,
    const SocketAddress& peername
  ) {
    if (buffer.get_next_buffer() == NULL) {
      iovec iov = buffer.as_write_iovec();
      return sendto(iov.iov_base, iov.iov_len, flags, peername);
    }
    else {
      vector<iovec> iov;
      Buffers::as_write_iovecs(buffer, iov);
      return sendmsg(&iov[0], iov.size(), flags, peername);
    }
  }

  virtual ssize_t
  sendto(
    const void* buf,
    size_t len,
    const MessageFlags& flags,
    const SocketAddress& peername
  );

public:
  // yield::Object
  DatagramSocket& inc_ref() {
    return Object::inc_ref(*this);
  }
};
}
}

#endif
