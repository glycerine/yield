// yield/net/sockets/ssl/ssl_socket.cpp

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


#ifdef YIELD_HAVE_OPENSSL

  #include "yield/net/sockets/ssl/ssl_socket.hpp"

  #include <openssl/ssl.h>


  namespace yield
  {
    namespace net
    {
      namespace sockets
      {
        namespace ssl
        {
          SSLSocket::SSLSocket
          (
            int domain,
            socket_t socket_,
            SSL* ssl,
            SSLContext& ssl_context
          )
            : TCPSocket( domain, socket_ ),
              ssl( ssl ),
              ssl_context( ssl_context.inc_ref() )
          {
            SSL_set_fd( ssl, socket_ );
          }

          SSLSocket::~SSLSocket()
          {
            SSL_free( ssl );
            SSLContext::dec_ref( ssl_context );
          }

          bool SSLSocket::connect( const SocketAddress& peername )
          {
            if ( TCPSocket::connect( peername ) )
            {
              SSL_set_connect_state( ssl );
              return true;
            }
            else
              return false;
          }

          SSLSocket* SSLSocket::create( SSLContext& ssl_context )
          {
            return create( DOMAIN_DEFAULT, ssl_context );
          }

          SSLSocket* SSLSocket::create( int domain, SSLContext& ssl_context )
          {
            SSL* ssl = SSL_new( ssl_context );
            if ( ssl != NULL )
            {
              socket_t socket_ = Socket::create( domain, TYPE, PROTOCOL );
              if ( socket_ != -1 )
                return new SSLSocket( domain, socket_, ssl, ssl_context );
              else
              {
                SSL_free( ssl );
                return NULL;
              }
            }
            else
              return NULL;
          }

          StreamSocket* SSLSocket::dup( int domain )
          {
            return StreamSocket::dup2( create( domain, ssl_context ) );
          }

          StreamSocket* SSLSocket::dup2( socket_t socket_ )
          {
            SSL* ssl = SSL_new( ssl_context );
            if ( ssl != NULL )
            {
              return StreamSocket::dup2
                     (
                       new SSLSocket( get_domain(), socket_, ssl, ssl_context )
                     );
            }
            else
              return NULL;
          }

          bool SSLSocket::listen()
          {
            SSL_set_accept_state( ssl );
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

          ssize_t SSLSocket::recv( void* buf, size_t buflen, int )
          {
            return SSL_read( ssl, buf, static_cast<int>( buflen ) );
          }

          ssize_t SSLSocket::send( const void* buf, size_t buflen, int )
          {
            return SSL_write( ssl, buf, static_cast<int>( buflen ) );
          }

          ssize_t SSLSocket::sendmsg( const iovec* iov, int iovlen, int )
          {
            // Concatenate the buffers
            return Channel::writev( iov, iovlen );
          }

          bool SSLSocket::shutdown()
          {
            if ( SSL_shutdown( ssl ) != -1 )
              return TCPSocket::shutdown();
            else
              return false;
          }

          bool SSLSocket::want_recv() const
          {
            return SSL_want_read( ssl ) == 1;
          }

          bool SSLSocket::want_send() const
          {
            return SSL_want_write( ssl ) == 1;
          }
        }
      }
    }
  }

#endif
//
