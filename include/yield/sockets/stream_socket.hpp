// yield/sockets/stream_socket.hpp

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

#ifndef _YIELD_SOCKETS_STREAM_SOCKET_HPP_
#define _YIELD_SOCKETS_STREAM_SOCKET_HPP_

#include "yield/sockets/socket.hpp"
#include "yield/sockets/socket_address.hpp"

namespace yield {
namespace sockets {
/**
  A connection-oriented, stream-type socket.
*/
class StreamSocket : public Socket {
public:
  /**
    The type of the socket in the (domain, type, protocol) tuple.
    Equivalent to the SOCK_STREAM constant on POSIX systems.
  */
  const static int TYPE;

public:
  /**
    setsockopt options specifically for connection-oriented sockets.
  */
  class Option : public Socket::Option {
  public:
    /**
      Keep the connection alive even if there's no activity on it.
    */
    const static int KEEPALIVE;

    /**
      Linger on close().
    */
    const static int LINGER;
  };

public:
  /**
    Construct a StreamSocket with the given domain.
    @param domain domain for the new socket e.g., AF_INET.
    @param protocol protocol for the new socket e.g., IPPROTO_TCP.
  */
  StreamSocket(int domain, int protocol = PROTOCOL_DEFAULT)
    : Socket(domain, TYPE, protocol)
  { }

  /**
    Empty virtual destructor.
  */
  virtual ~StreamSocket() { }

public:
  /**
    Accept an incoming connection on the socket.
    @return the accepted StreamSocket on success, NULL+errno on failure
  */
  YO_NEW_REF StreamSocket* accept() {
    SocketAddress peername;
    return accept(peername);
  }

public:
  /**
    Accept an incoming connection on the socket.
    @param[out] peername address of the new peer
    @return the accepted StreamSocket on success, NULL+errno on failure
  */
  virtual YO_NEW_REF StreamSocket* accept(SocketAddress& peername);

public:
  /**
    Duplicate this socket, including its underlying descriptor.
    @return the duplicate StreamSocket on success, NULL+errno on failure.
  */
  virtual YO_NEW_REF StreamSocket* dup() {
    socket_t socket_ = Socket::create(get_domain(), TYPE, get_protocol());
    if (socket_ != static_cast<socket_t>(-1)) {
      return new StreamSocket(get_domain(), get_protocol(), socket_);
    } else {
      return NULL;
    }
  }

public:
  /**
    Listen for connections on the socket.
    @return true on success, false+errno on failure
  */
  virtual bool listen();

public:
  /**
    Send data from a file to the socket without copying into userspace.
    @param fd file descriptor to send data from
    @param offset offset in the file from which to send data
    @param nbytes number of bytes to send
    @return the number of bytes sent on success, -1+errno on failure
  */
  virtual ssize_t sendfile(fd_t fd, off_t offset, size_t nbytes);

public:
  /**
    Shut down part of the socket's connection.
    @param shut_rd shut down the read half of the connection
    @param shut_wr shut down the write half of the connection
    @return true on success, false+errno on failure
  */
  virtual bool shutdown(bool shut_rd = true, bool shut_wr = true);

public:
  /**
    Check if the last accept call failed because it would have blocked.
    Equivalent to errno == EWOULDBLOCK on POSIX systems.
    @return true if the last accept call failed because it would have blocked
  */
  virtual bool want_accept() const;

  /**
    Check if the last connect call failed because it would have blocked.
    Equivalent to errno == EWOULDBLOCK on POSIX systems.
    @return true if the last connect call failed because it would have blocked
  */
  virtual bool want_connect() const;

public:
  // yield::Object
  StreamSocket& inc_ref() {
    return Object::inc_ref(*this);
  }

public:
  // yield::Socket
  virtual bool setsockopt(int option_name, int option_value);

protected:
  friend class StreamSocketPair;

  StreamSocket(int domain, int protocol, socket_t socket_)
    : Socket(domain, TYPE, protocol, socket_)
  { }

protected:
  virtual YO_NEW_REF StreamSocket* dup2(socket_t socket_) {
    return new StreamSocket(get_domain(), get_protocol(), socket_);
  }
};
}
}

#endif
