// yield/aio/net/sockets/accept_aiocb.cpp

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


#include "yield/buffer.hpp"
#include "yield/aio/net/sockets/accept_aiocb.hpp"
#include "yield/net/sockets/socket_address.hpp"
#include "yield/net/sockets/stream_socket.hpp"


namespace yield
{
  namespace aio
  {
    namespace net
    {
      namespace sockets
      {
        using yield::net::sockets::SocketAddress;
        using yield::net::sockets::StreamSocket;


        acceptAIOCB::~acceptAIOCB()
        {
          StreamSocket::dec_ref( accepted_socket );
          SocketAddress::dec_ref( peername );
          Buffer::dec_ref( recv_buffer );
        }


        StreamSocket* acceptAIOCB::get_accepted_socket() const
        {
          return accepted_socket;
        }

        SocketAddress& acceptAIOCB::get_peername() const
        {
          return peername;
        }

        acceptAIOCB::RetryStatus acceptAIOCB::retry()
        {
          accepted_socket
            = static_cast<StreamSocket&>( get_socket() )
                .accept( get_peername() );

          if ( accepted_socket != NULL )
          {
            set_return( 0 );
            return RETRY_STATUS_COMPLETE;
          }
          else if ( static_cast<StreamSocket&>( get_socket() ).want_accept() )
            return RETRY_STATUS_WANT_READ;
          else
          {
            set_error( Exception::get_last_error_code() );
            return RETRY_STATUS_ERROR;
          }
        }
      }
    }
  }
}
