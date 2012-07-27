// yield/sockets/ssl/ssl_socket.hpp

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

#ifndef _YIELD_SOCKETS_SSL_SSL_SOCKET_HPP_
#define _YIELD_SOCKETS_SSL_SSL_SOCKET_HPP_

#include "yield/sockets/tcp_socket.hpp"
#ifdef YIELD_HAVE_OPENSSL
#include "yield/sockets/ssl/ssl_context.hpp"
#endif

#ifdef YIELD_HAVE_OPENSSL
struct ssl_st;
typedef ssl_st SSL;
#endif

namespace yield {
namespace sockets {
#ifdef YIELD_HAVE_OPENSSL
namespace ssl {
class SSLSocket : public TCPSocket {
public:
  SSLSocket(int domain = DOMAIN_DEFAULT);
  SSLSocket(SSLContext& ssl_context, int domain = DOMAIN_DEFAULT);
  ~SSLSocket();

public:
  bool do_handshake();

public:
  operator SSL* () const {
    return ssl;
  }

public:
  // yield::sockets::Socket
  bool connect(const SocketAddress& peername);
  ssize_t recv(Buffer& buffer, const MessageFlags& flags);
  ssize_t recvmsg(const iovec* iov, int iovlen, const MessageFlags& flags);
  ssize_t recv(void* buf, size_t buflen, const MessageFlags&);
  ssize_t send(const void* buf, size_t buflen, const MessageFlags&);
  ssize_t sendmsg(const iovec* iov, int iovlen, const MessageFlags&);
  bool shutdown(bool shut_rd = true, bool shut_wr = true);
  bool want_recv() const;
  bool want_send() const;

public:
  // yield::sockets::StreamSocket
  YO_NEW_REF SSLSocket* accept();
  YO_NEW_REF SSLSocket* accept(SocketAddress& peername);
  YO_NEW_REF SSLSocket* dup();
  bool listen();

private:
  SSLSocket(int domain, socket_t socket_, SSL* ssl, SSLContext& ssl_context);

private:
  void init(SSLContext& ssl_context);
  void init(SSL* ssl);

private:
  // yield::sockets::StreamSocket
  YO_NEW_REF SSLSocket* dup2(socket_t);

private:
  SSL* ssl;
  SSLContext& ssl_context;
};
}
#endif
}
}

#endif
