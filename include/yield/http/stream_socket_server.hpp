// yield/http/stream_socket_server.hpp

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


#ifndef _YIELD_HTTP_STREAM_SOCKET_SERVER_HPP_
#define _YIELD_HTTP_STREAM_SOCKET_SERVER_HPP_


#include "yield/http/socket_server.hpp"
#include "yield/http/stream_socket_peer.hpp"


namespace yield
{
  namespace aio
  {
    namespace net
    {
      namespace sockets
      {
        class acceptAIOCB;
      }
    }
  }


  namespace http
  {
    class StreamSocketServer : public StreamSocketPeer<SocketServer>
    {
    protected:
      class Connection : public StreamSocketPeer<SocketServer>::Connection
      {
      public:
        Connection
        (
          StreamSocketServer&,
          YO_NEW_REF yield::net::sockets::SocketAddress& peername,
          YO_NEW_REF yield::net::sockets::StreamSocket& socket_
        );

        virtual void handle( YO_NEW_REF yield::aio::net::sockets::acceptAIOCB& accept_aiocb ) = 0;
      };

    protected:
      StreamSocketServer
      (
        Log* error_log,
        YO_NEW_REF yield::net::sockets::StreamSocket& socket_,
        const yield::net::sockets::SocketAddress& sockname,
        Log* trace_log
      );

      virtual ~StreamSocketServer();

      virtual Connection&
      create_connection
      (
        yield::net::sockets::SocketAddress& peername,
        yield::net::sockets::StreamSocket& socket_
      ) = 0;

      void enqueue( YO_NEW_REF yield::aio::net::sockets::acceptAIOCB& accept_aiocb );

      yield::net::sockets::StreamSocket& get_socket() const { return socket_; }

      // Stage
      virtual void service( YO_NEW_REF Event& event );

    private:
      yield::net::sockets::StreamSocket& socket_;
    };
  }
}


#endif
