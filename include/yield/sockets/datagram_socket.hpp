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
/**
  A connectionless, datagram-type socket.
*/
class DatagramSocket : public Socket {
public:
  /**
    The type of the socket in the (domain, type, protocol) tuple.
    Equivalent to the SOCK_DGRAM constant on POSIX systems.
  */
  const static int TYPE;

public:
  /**
    Construct a DatagramSocket with the given domain.
    @param domain domain for the new socket e.g., AF_INET.
    @param protocol protocol for the new socket e.g., IPPROTO_UDP.
  */
  DatagramSocket(int domain, int protocol = PROTOCOL_DEFAULT)
    : Socket(domain, TYPE, protocol)
  { }

  /**
    Empty virtual destructor.
  */
  virtual ~DatagramSocket() { }

public:
  /**
    Read from the socket into a Buffer, also recording the sender's address.
    May be a recvfrom(void*, size_t, const MessageFlags&, SocketAddress&) or a
      recvmsg(const iovec*, int, const MessageFlags&, SocketAddress&),
      depending on whether buffer is a single buffer or a linked list of them.
    Updates buffer's size depending on how many bytes were read.
    @param[in, out] buffer the buffer to read data into
    @param[out] peername the sender's address
    @param flags flags altering the behavior of the underlying system call
    @return the number of bytes read on success, -1+errno on failure
  */
  ssize_t
  recvfrom(
    Buffer& buffer,
    const MessageFlags& flags,
    SocketAddress& peername
  ) {
    if (buffer.get_next_buffer() == NULL) {
      iovec iov = buffer.as_read_iovec();
      ssize_t recv_ret = recvfrom(iov.iov_base, iov.iov_len, flags, peername);
      if (recv_ret > 0) {
        buffer.put(NULL, static_cast<size_t>(recv_ret));
      }
      return recv_ret;
    } else {
      vector<iovec> iov;
      Buffers::as_read_iovecs(buffer, iov);
      ssize_t recv_ret = recvmsg(&iov[0], iov.size(), flags, peername);
      if (recv_ret > 0) {
        Buffers::put(buffer, NULL, static_cast<size_t>(recv_ret));
      }
      return recv_ret;
    }
  }

  /**
    Read from the socket into a single buffer, also recording the sender's address.
    @param[in, out] buf pointer to the buffer
    @param buflen the length of the memory region pointed to by buf
    @param flags flags altering the behavior of the underlying system call
    @param[out] peername the sender's address
    @return the number of bytes read on success, -1+errno on failure
  */
  virtual ssize_t
  recvfrom(
    void* buf,
    size_t buflen,
    const MessageFlags& flags,
    SocketAddress& peername
  );

  /**
    Read from the socket into multiple buffers (scatter I/O),
      also recording the sender's address.
    @param[in, out] iov array of I/O vectors describing the buffers
    @param iovlen length of the I/O vectors array
    @param flags flags altering the behavior of the underlying system call
    @param[out] peername the sender's address
    @return the number of bytes read on success, -1+errno on failure
  */
  virtual ssize_t
  recvmsg(
    const iovec* iov,
    int iovlen,
    const MessageFlags& flags,
    SocketAddress& peername
  );

public:
  /**
    Write to the socket and a specific peer from multiple buffers (gather I/O).
    @param iov array of I/O vectors describing the buffers
    @param iovlen length of the I/O vectors array
    @param flags flags altering the behavior of the underlying system call
    @param peername address of the receiver
    @return the number of bytes written on success, -1+errno on failure
  */
  virtual ssize_t
  sendmsg(
    const iovec* iov,
    int iovlen,
    const MessageFlags& flags,
    const SocketAddress& peername
  );

  /**
    Write to the socket and a specific peer from a Buffer.
    May be a sendto(const void*, size_t, const MessageFlags&, const SocketAddress&) or a
      sendmsg(const iovec*, int, const MessageFlags&, const SocketAdress&),
      depending on whether buffer is a single buffer or a linked list of them.
    @param buffer the buffer to write from
    @param flags flags altering the behavior of the underlying system call
    @param peername address of the receiver
    @return the number of bytes written on success, -1+errno on failure
  */
  ssize_t
  sendto(
    const Buffer& buffer,
    const MessageFlags& flags,
    const SocketAddress& peername
  ) {
    if (buffer.get_next_buffer() == NULL) {
      iovec iov = buffer.as_write_iovec();
      return sendto(iov.iov_base, iov.iov_len, flags, peername);
    } else {
      vector<iovec> iov;
      Buffers::as_write_iovecs(buffer, iov);
      return sendmsg(&iov[0], iov.size(), flags, peername);
    }
  }

  /**
    Write to the socket and a specific peer from a single buffer.
    @param buf pointer to the buffer
    @param buflen the length of the memory region pointed to by buf
    @param flags flags altering the behavior of the underlying system call
    @param peername address of the receiver
    @return the number of bytes written on success, -1+errno on failure
  */
  virtual ssize_t
  sendto(
    const void* buf,
    size_t buflen,
    const MessageFlags& flags,
    const SocketAddress& peername
  );

public:
  // yield::Object
  DatagramSocket& inc_ref() {
    return Object::inc_ref(*this);
  }

private:
  friend class DatagramSocketPair;

  DatagramSocket(int domain, int protocol, socket_t socket_)
    : Socket(domain, TYPE, protocol, socket_) {
  }
};
}
}

#endif
