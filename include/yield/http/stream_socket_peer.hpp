// yield/http/stream_socket_peer.hpp

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


#ifndef _YIELD_HTTP_STREAM_SOCKET_PEER_HPP_
#define _YIELD_HTTP_STREAM_SOCKET_PEER_HPP_


#include "yield/aio/net/sockets/recv_aiocb.hpp"
#include "yield/aio/net/sockets/send_aiocb.hpp"


namespace yield {
namespace net {
namespace sockets {
class StreamSocket;
}
}


namespace http {
template <class SocketPeerType>
class StreamSocketPeer : public SocketPeerType {
protected:
  class recvAIOCB;
  class sendAIOCB;


  class Connection : public EventHandler {
  public:
    Connection
    (
      StreamSocketPeer&,
      yield::net::sockets::SocketAddress& peername,
      YO_NEW_REF yield::net::sockets::StreamSocket& socket_
    );

    virtual ~Connection();

    void close();

    const string& get_log_prefix();
    yield::net::sockets::SocketAddress& get_peername() const {
      return peername;
    }
    yield::net::sockets::StreamSocket& get_socket() const {
      return *socket_;
    }

    virtual void handle( YO_NEW_REF recvAIOCB& recv_aiocb ) = 0;
    virtual void handle( YO_NEW_REF sendAIOCB& send_aiocb ) = 0;

    // Object
    virtual const char* get_type_name() const {
      return "yield::net::sockets::StreamSocketPeer::Connection";
    }

    Connection& inc_ref() {
      return Object::inc_ref( *this );
    }

  protected:
    void enqueue( YO_NEW_REF recvAIOCB& recv_aiocb );
    void enqueue( YO_NEW_REF sendAIOCB& send_aiocb );

    yield::aio::net::sockets::AIOQueue& get_aio_queue() const {
      return aio_queue;
    }
    Log* get_error_log() const {
      return error_log;
    }
    Log* get_trace_log() const {
      return trace_log;
    }

  private:
    yield::aio::net::sockets::AIOQueue& aio_queue;
    Log* error_log;
    string log_prefix;
    yield::net::sockets::SocketAddress& peername;
    yield::net::sockets::StreamSocket* socket_;
    Log* trace_log;
  };


  class AIOCB {
  public:
    Connection& get_connection() {
      return connection;
    }

  protected:
    AIOCB( Connection& connection )
      : connection( connection.inc_ref() )
    { }

  private:
    Connection& connection;
  };


  class recvAIOCB
    : public yield::aio::net::sockets::recvAIOCB,
      public StreamSocketPeer::AIOCB {
  public:
    recvAIOCB( Connection& connection, YO_NEW_REF Buffer& buffer )
      : yield::aio::net::sockets::recvAIOCB( connection.get_socket(), buffer, 0 ),
        StreamSocketPeer::AIOCB( connection )
    { }
  };


  class sendAIOCB
    : public yield::aio::net::sockets::sendAIOCB,
      public StreamSocketPeer::AIOCB {
  public:
    sendAIOCB( Connection& connection, YO_NEW_REF Buffer& buffer )
      : yield::aio::net::sockets::sendAIOCB( connection.get_socket(), buffer, 0 ),
        StreamSocketPeer::AIOCB( connection )
    { }
  };

protected:
  StreamSocketPeer( Log* error_log, Log* trace_log );
  virtual ~StreamSocketPeer() { }

  // Stage
  virtual void service( YO_NEW_REF Event& event );
};
}
}


#endif
