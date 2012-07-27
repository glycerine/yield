// yield/sockets/ssl/ssl_socket.cpp

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

#include "ssl_exception.hpp"
#include "yield/sockets/ssl/ssl_socket.hpp"

#ifdef YIELD_HAVE_OPENSSL
#include <openssl/ssl.h>
#endif

namespace yield {
namespace sockets {
#ifdef YIELD_HAVE_OPENSSL
namespace ssl {
SSLSocket::SSLSocket(int domain)
  : TCPSocket(domain),
    ssl_context(*new SSLContext()) {
  init(ssl_context);
}

SSLSocket::SSLSocket(SSLContext& ssl_context, int domain)
  : TCPSocket(domain),
    ssl_context(ssl_context.inc_ref()) {
  init(ssl_context);
}

SSLSocket::SSLSocket(
  int domain,
  socket_t socket_,
  SSL* ssl,
  SSLContext& ssl_context
) : TCPSocket(domain, socket_),
  ssl(ssl),
  ssl_context(ssl_context.inc_ref()) {
  init(ssl);
}

SSLSocket::~SSLSocket() {
  SSL_free(ssl);
  SSLContext::dec_ref(ssl_context);
}

YO_NEW_REF SSLSocket* SSLSocket::accept() {
  SocketAddress peername;
  return accept(peername);
}

YO_NEW_REF SSLSocket* SSLSocket::accept(SocketAddress& peername) {
  SSLSocket* ssl_socket = static_cast<SSLSocket*>(TCPSocket::accept(peername));
  if (ssl_socket != NULL) {
    SSL_set_accept_state(*ssl_socket);
  }
  return ssl_socket;
}

bool SSLSocket::connect(const SocketAddress& peername) {
  if (TCPSocket::connect(peername)) {
    SSL_set_connect_state(ssl);
    return true;
  } else {
    return false;
  }
}

bool SSLSocket::do_handshake() {
  return SSL_do_handshake(ssl) == 1;
}

YO_NEW_REF SSLSocket* SSLSocket::dup() {
  socket_t socket_ = Socket::create(get_domain(), TYPE, PROTOCOL);
  if (socket_ != -1) {
    SSL* ssl = SSL_new(ssl_context);
    if (ssl != NULL) {
      return new SSLSocket(get_domain(), socket_, ssl, ssl_context);
    } else {
      SSL_free(ssl);
      return NULL;
    }
  } else {
    return NULL;
  }
}

YO_NEW_REF SSLSocket* SSLSocket::dup2(socket_t socket_) {
  SSL* ssl = SSL_new(ssl_context);
  if (ssl != NULL) {
    return new SSLSocket(get_domain(), socket_, ssl, ssl_context);
  } else {
    return NULL;
  }
}

void SSLSocket::init(SSLContext& ssl_context) {
  ssl = SSL_new(ssl_context);
  if (ssl != NULL) {
    try {
      init(ssl);
    } catch (SSLException&) {
      SSL_free(ssl);
      throw;
    }
  } else {
    throw SSLException();
  }
}

void SSLSocket::init(SSL* ssl) {
  BIO* bio = BIO_new_socket(static_cast<socket_t>(*this), BIO_NOCLOSE);
  if (bio != NULL) {
    SSL_set_bio(ssl, bio, bio);
  } else {
    throw SSLException();
  }

  //if (!SSL_set_fd(ssl, *this))
  //  throw SSLException();
}

bool SSLSocket::listen() {
  SSL_set_accept_state(ssl);
  return TCPSocket::listen();
}

/*
void SSLSocket::info_callback( const SSL* ssl, int where, int ret )
{
  ostringstream info;

  int w = where & ~SSL_ST_MASK;
  if ( ( w & SSL_ST_CONNECT ) == SSL_ST_CONNECT ) info << "SSL_connect:";
  else if ( ( w & SSL_ST_ACCEPT ) == SSL_ST_ACCEPT ) info << "SSL_accept:";
  else info << "undefined:";

  if ( ( where & SSL_CB_LOOP ) == SSL_CB_LOOP )
    info << SSL_state_string_long( ssl );
  else if ( ( where & SSL_CB_ALERT ) == SSL_CB_ALERT )
  {
    if ( ( where & SSL_CB_READ ) == SSL_CB_READ )
      info << "read:";
    else
      info << "write:";
    info << "SSL3 alert" << SSL_alert_type_string_long( ret ) << ":" <<
            SSL_alert_desc_string_long( ret );
  }
  else if ( ( where & SSL_CB_EXIT ) == SSL_CB_EXIT )
  {
    if ( ret == 0 )
      info << "failed in " << SSL_state_string_long( ssl );
    else
      info << "error in " << SSL_state_string_long( ssl );
  }
  else
    return;

  reinterpret_cast<SSLSocket*>( SSL_get_app_data( const_cast<SSL*>( ssl ) ) )
    ->log->get_stream( Log::NOTICE ) << "SSLSocket: " << info.str();
}
*/

ssize_t SSLSocket::recv(Buffer& buffer, const MessageFlags& flags) {
  return TCPSocket::recv(buffer, flags);
}

ssize_t SSLSocket::recv(void* buf, size_t buflen, const MessageFlags&) {
  return SSL_read(ssl, buf, static_cast<int>(buflen));
  //ssize_t read_ret = SSL_read(ssl, buf, static_cast<int>(buflen));
  //if (read_ret >= 0)
  //  return read_ret;
  //else
  //  throw SSLException();
}

ssize_t SSLSocket::recvmsg(const iovec* iov, int iovlen, const MessageFlags& flags) {
  if (iovlen == 1) {
    return recv(iov[0].iov_base, iov[0].iov_len, flags);
  } else {
    return -1;
  }
}

ssize_t SSLSocket::send(const void* buf, size_t buflen, const MessageFlags&) {
  return SSL_write(ssl, buf, static_cast<int>(buflen));
  //ssize_t write_ret = SSL_write(ssl, buf, static_cast<int>(buflen));
  //if (write_ret >= 0)
  //  return write_ret;
  //else
  //  throw SSLException();
}

ssize_t SSLSocket::sendmsg(const iovec* iov, int iovlen, const MessageFlags&) {
  return Channel::writev(iov, iovlen);
}

bool SSLSocket::shutdown(bool shut_rd, bool shut_wr) {
  if (SSL_shutdown(ssl) != -1) {
    return TCPSocket::shutdown(shut_rd, shut_wr);
  } else {
    return false;
  }
}

bool SSLSocket::want_recv() const {
  return SSL_want_read(ssl) == 1;
}

bool SSLSocket::want_send() const {
  return SSL_want_write(ssl) == 1;
}
}
#endif
}
}
