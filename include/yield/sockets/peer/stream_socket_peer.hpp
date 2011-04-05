// yield/sockets/peer/stream_socket_peer.hpp

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

#ifndef _YIELD_SOCKETS_PEER_STREAM_SOCKET_PEER_HPP_
#define _YIELD_SOCKETS_PEER_STREAM_SOCKET_PEER_HPP_

#include "yield/assert.hpp"
#include "yield/exception.hpp"
#include "yield/buffer.hpp"
#include "yield/log.hpp"
#include "yield/sockets/stream_socket.hpp"
#include "yield/sockets/aio/aio_queue.hpp"
#include "yield/sockets/aio/recv_aiocb.hpp"
#include "yield/sockets/aio/send_aiocb.hpp"

#include <sstream> // for ostringstream

namespace yield {
namespace sockets {
class SocketAddress;
class StreamSocket;

namespace peer {
template <class SocketPeerType>
class StreamSocketPeer : public SocketPeerType {
protected:
  class recvAIOCB;
  class sendAIOCB;

  class Connection : public EventHandler {
  public:
    Connection(
      StreamSocketPeer& peer,
      SocketAddress& peername,
      YO_NEW_REF StreamSocket& socket_
    ) 
    : aio_queue(peer.get_aio_queue().inc_ref()),
      error_log(Object::inc_ref(peer.get_error_log())),
      peername(peername),
      socket_(&socket_),
      trace_log(Object::inc_ref(peer.get_trace_log()))
    { }

    virtual ~Connection() {
      close();
      Log::dec_ref(error_log);
      StreamSocket::dec_ref(socket_);
      Log::dec_ref(trace_log);
    }

  public:
    void close() {
      get_socket().shutdown();
      get_socket().close();
    }

  public:
    const string& get_log_prefix() {
      if (log_prefix.empty()) {
        SocketAddress sockname;
        get_socket().getsockname(sockname);

        std::ostringstream log_prefix_oss;
        log_prefix_oss << get_type_name() << "(";
        log_prefix_oss << sockname << "/" << peername;
        log_prefix_oss << ")";
        log_prefix = log_prefix_oss.str();
      }

      return log_prefix;
    }

    SocketAddress& get_peername() const {
      return peername;
    }

    StreamSocket& get_socket() const {
      return *socket_;
    }

  public:
    virtual void handle(YO_NEW_REF recvAIOCB& recv_aiocb) = 0;
    virtual void handle(YO_NEW_REF sendAIOCB& send_aiocb) = 0;

  public:
    // Object
    virtual const char* get_type_name() const {
      return "yield::sockets::StreamSocketPeer::Connection";
    }

    Connection& inc_ref() {
      return Object::inc_ref(*this);
    }

  protected:
    void enqueue(YO_NEW_REF recvAIOCB& recv_aiocb) {
      if (!get_aio_queue().enqueue(recv_aiocb)) {
        recvAIOCB::dec_ref(recv_aiocb);
        Connection::dec_ref(*this);
      }
    }

    void enqueue(YO_NEW_REF sendAIOCB& send_aiocb) {
      if (!get_aio_queue().enqueue(send_aiocb)) {
        sendAIOCB::dec_ref(send_aiocb);
        Connection::dec_ref(*this);
      }
    }

  protected:
    ::yield::sockets::aio::AIOQueue& get_aio_queue() const {
      return aio_queue;
    }

    Log* get_error_log() const {
      return error_log;
    }

    Log* get_trace_log() const {
      return trace_log;
    }

  private:
    ::yield::sockets::aio::AIOQueue& aio_queue;
    Log* error_log;
    string log_prefix;
    SocketAddress& peername;
    StreamSocket* socket_;
    Log* trace_log;
  };


  class AIOCB {
  public:
    Connection& get_connection() {
      return connection;
    }

  protected:
    AIOCB(Connection& connection)
      : connection(connection.inc_ref())
    { }

    virtual ~AIOCB() {
      Connection::dec_ref(connection);
    }

  private:
    Connection& connection;
  };


  class recvAIOCB
    : public yield::sockets::aio::recvAIOCB,
      public StreamSocketPeer::AIOCB {
  public:
    recvAIOCB(Connection& connection, YO_NEW_REF Buffer& buffer)
      : yield::sockets::aio::recvAIOCB(connection.get_socket(), buffer, 0),
        StreamSocketPeer::AIOCB(connection)
    { }
  };


  class sendAIOCB
    : public ::yield::sockets::aio::sendAIOCB,
      public StreamSocketPeer::AIOCB {
  public:
    sendAIOCB(Connection& connection, YO_NEW_REF Buffer& buffer)
      : yield::sockets::aio::sendAIOCB(connection.get_socket(), buffer, 0),
        StreamSocketPeer::AIOCB(connection)
    { }
  };

protected:
  StreamSocketPeer(Log* error_log, Log* trace_log)
    : SocketPeerType(error_log, trace_log)
  { }

  virtual ~StreamSocketPeer() { }

  // Stage
  virtual void service(YO_NEW_REF Event& event) {
    switch (event.get_type_id()) {
    case recvAIOCB::TYPE_ID: {
      recvAIOCB& recv_aiocb = static_cast<recvAIOCB&>(event);
      StreamSocketPeer<SocketPeerType>::Connection& connection
      = static_cast<Connection&>(recv_aiocb.get_connection());

      if (recv_aiocb.get_error() == 0) {
        if (recv_aiocb.get_return() > 0) {
          if (this->get_trace_log() != NULL) {
            this->get_trace_log()->get_stream(Log::INFO) <<
                connection.get_log_prefix() << ": received ";
            this->get_trace_log()->write(recv_aiocb.get_buffer(), Log::INFO);
          }

          connection.handle(recv_aiocb);
        } else {
          if (this->get_trace_log() != NULL) {
            this->get_trace_log()->get_stream(Log::INFO) <<
                connection.get_log_prefix() << ": normal connection shutdown";
          }

          connection.handle(recv_aiocb);

          Connection::dec_ref(connection);
        }
      } else {
        if (this->get_error_log() != NULL) {
          this->get_error_log()->get_stream(Log::ERR) <<
              connection.get_log_prefix() << ": " <<
              "error in " << recv_aiocb.get_type_name() << ": " <<
              Exception(recv_aiocb.get_error());
        }

        connection.handle(recv_aiocb);

        Connection::dec_ref(connection);
      }
    }
    break;

    case sendAIOCB::TYPE_ID: {
      sendAIOCB& send_aiocb = static_cast<sendAIOCB&>(event);
      StreamSocketPeer<SocketPeerType>::Connection& connection
      = send_aiocb.get_connection();

      if (send_aiocb.get_error() == 0) {
        if (this->get_trace_log() != NULL) {
          this->get_trace_log()->get_stream(Log::INFO) <<
              connection.get_log_prefix() << ": sent ";
          this->get_trace_log()->write(send_aiocb.get_buffer(), Log::INFO);
        }

        connection.handle(send_aiocb);
      } else {
        if (this->get_error_log() != NULL) {
          this->get_error_log()->get_stream(Log::ERR) <<
              connection.get_log_prefix() << ": " <<
              "error in " << send_aiocb.get_type_name() << ":" <<
              Exception(send_aiocb.get_error());
        }

        connection.handle(send_aiocb);

        Connection::dec_ref(connection);
      }
    }
    break;
    }
  }
};
}
}
}

#endif
