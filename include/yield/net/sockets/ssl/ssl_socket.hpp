// yield/net/sockets/ssl/ssl_socket.hpp

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


#ifndef _YIELD_NET_SOCKETS_SSL_SSL_SOCKET_HPP_
#define _YIELD_NET_SOCKETS_SSL_SSL_SOCKET_HPP_


#include "yield/net/sockets/tcp_socket.hpp"


#ifdef YIELD_HAVE_OPENSSL
struct ssl_st;
typedef ssl_st SSL;
#endif


namespace yield {
namespace net {
namespace sockets {
#ifdef YIELD_HAVE_OPENSSL
namespace ssl {
class SSLSocket : public TCPSocket {
public:
  class AIOAcceptCallback : public StreamSocket::AIOAcceptCallback {
  public:
    virtual void
    onAcceptCompletion
    (
      SSLSocket& accepted_ssl_socket,
      void* context,
      SocketAddress& peername,
      Buffer* recv_buffer
    ) = 0;

    // StreamSocket::AIOAcceptCallback
    void
    onAcceptCompletion
    (
      StreamSocket& accepted_stream_socket,
      void* context,
      SocketAddress& peername,
      Buffer* recv_buffer
    ) {
      onAcceptCompletion
      (
        static_cast<SSLSocket&>( accepted_stream_socket ),
        context,
        peername,
        recv_buffer
      );
    }
  };

public:
  virtual ~SSLSocket();

  static YO_NEW_REF SSLSocket* create( SSLContext& );
  static YO_NEW_REF SSLSocket* create( int domain, SSLContext& );
  operator SSL*() const {
    return ssl;
  }

  // Socket
  virtual bool connect( const SocketAddress& peername );
  virtual ssize_t recv( void* buf, size_t buflen, int );
  virtual ssize_t send( const void* buf, size_t buflen, int );
  virtual ssize_t sendmsg( const iovec* iov, int iovlen, int );
  virtual bool want_recv() const;
  virtual bool want_send() const;

  // StreamSocket
  YO_NEW_REF StreamSocket* dup() {
    return dup( get_domain() );
  }
  virtual YO_NEW_REF StreamSocket* dup( int domain );
  virtual bool listen();
  virtual bool shutdown();

protected:
  SSLSocket( int domain, socket_t, SSL*, SSLContext& );

  SSLContext& get_ssl_context() const {
    return ssl_context;
  }

private:
  // StreamSocket
  virtual YO_NEW_REF StreamSocket* dup2( socket_t );

private:
  SSL* ssl;
  SSLContext& ssl_context;
};
}
#endif
}
}
}


#endif
