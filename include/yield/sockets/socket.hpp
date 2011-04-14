// yield/sockets/socket.hpp

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

#ifndef _YIELD_SOCKETS_SOCKET_HPP_
#define _YIELD_SOCKETS_SOCKET_HPP_

#include "yield/auto_object.hpp"
#include "yield/channel.hpp"
#include "yield/exception.hpp"
#include "yield/sockets/socket_address.hpp"

namespace yield {
namespace sockets {
class Socket : public Channel {
public:
  class MessageFlags {
  public:
    const static int DONTROUTE = 1;
    const static int OOB = 2;
    const static int PEEK = 4;

  public:
    MessageFlags(int = 0);
    operator int() const {
      return platform_message_flags;
    }

  private:
    int platform_message_flags;
  };

public:
  typedef uint32_t Option;
  const static Option OPTION_SO_KEEPALIVE = 1;
  const static Option OPTION_SO_LINGER = 2;

  const static int PROTOCOL_DEFAULT = 0;

public:
  Socket(int domain, int type, int protocol)
    : domain(domain),
      type(type),
      protocol(protocol) {
    socket_ = create(domain, type, protocol);
    if (socket_ == static_cast<socket_t>(-1))
      throw Exception();
  }

  Socket(int domain, int type, int protocol, socket_t socket_)
    : domain(domain),
      type(type),
      protocol(protocol),
      socket_(socket_)
  { }

  virtual ~Socket() {
    close();
  }

public:
  virtual bool bind(const SocketAddress& to_sockaddr);

  virtual bool connect(const SocketAddress& peername);

public:
  int get_domain() const {
    return domain;
  }

public:
  static string getfqdn();
  static string gethostname();

public:
  auto_Object<SocketAddress> getpeername() const throw(Exception) {
    SocketAddress* peername = new SocketAddress;
    if (getpeername(*peername))
      return peername;
    else {
      Exception exception;
      SocketAddress::dec_ref(*peername);
      throw exception;
    }
  }

  bool getpeername(OUT SocketAddress& peername) const;

public:
  int get_protocol() const {
    return protocol;
  }

public:
  auto_Object<SocketAddress> getsockname() const throw(Exception) {
    SocketAddress* sockname = new SocketAddress;
    if (getsockname(*sockname))
      return sockname;
    else {
      Exception exception;
      SocketAddress::dec_ref(*sockname);
      throw exception;
    }
  }

  bool getsockname(OUT SocketAddress& sockname) const;

public:
  int get_type() const {
    return type;
  }

public:
#ifdef _WIN32
  operator socket_t() const {
    return socket_;
  }
#endif

public:
  virtual ssize_t recv(void* buf, size_t len, const MessageFlags&);

  virtual
  ssize_t
  recvfrom(
    void* buf,
    size_t len,
    const MessageFlags&,
    SocketAddress& peername
  );

  virtual ssize_t
  recvmsg(
    const iovec* iov,
    int iovlen,
    const MessageFlags&,
    SocketAddress* peername = NULL
  );

public:
  virtual ssize_t send(const void*, size_t, const MessageFlags&);

  virtual ssize_t
  sendmsg(
    const iovec* iov,
    int iovlen,
    const MessageFlags&,
    const SocketAddress* peername = NULL
  );

  virtual ssize_t
  sendto(
    const void* buf,
    size_t len,
    const MessageFlags&,
    const SocketAddress& peername
  );

public:
  virtual bool setsockopt(Option option, bool onoff);

public:
  virtual bool shutdown(bool shut_rd = true, bool shut_wr = true);

public:
  virtual bool want_recv() const;
  virtual bool want_send() const;

public:
  // yield::Object
  const char* get_type_name() const {
    return "yield::sockets::Socket";
  }

  Socket& inc_ref() {
    return Object::inc_ref(*this);
  }

public:
  // yield::Channel
  virtual bool close();

  operator fd_t() const {
    return socket_to_fd(socket_);
  }

  ssize_t read(void* buf, size_t buflen) {
    return recv(buf, buflen, 0);
  }

  ssize_t readv(const iovec* iov, int iovlen) {
    return recvmsg(iov, iovlen, 0);
  }

  virtual bool set_blocking_mode(bool blocking);

  ssize_t write(const void* buf, size_t buflen) {
    return send(buf, buflen, 0);
  }

  ssize_t writev(const iovec* iov, int iovlen) {
    return sendmsg(iov, iovlen, 0);
  }

protected:
  static socket_t create(int domain, int type, int protocol);

private:
  int domain, type, protocol;
  socket_t socket_;
};

static inline std::ostream& operator<<(std::ostream& os, Socket& socket_) {
    os << 
       socket_.get_type_name() << 
       "(" <<
         *socket_.getsockname() <<
         "/" <<
         *socket_.getpeername() <<
       ")";
    return os;
}
}
}

#endif
