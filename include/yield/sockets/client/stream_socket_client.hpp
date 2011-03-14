// yield/clientserver/stream_socket_client.hpp

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

#ifndef _YIELD_CLIENTSERVER_STREAM_SOCKET_CLIENT_HPP_
#define _YIELD_CLIENTSERVER_STREAM_SOCKET_CLIENT_HPP_

#include "yield/buffer.hpp"
#include "yield/time.hpp"
#include "yield/uri/uri.hpp"
#include "yield/aio/sockets/connect_aiocb.hpp"
#include "yield/clientserver/socket_client.hpp"
#include "yield/clientserver/stream_socket_peer.hpp"


namespace yield {
namespace clientserver {
class StreamSocketClient : public StreamSocketPeer<SocketClient> {
public:
  class Configuration {
  public:
    const static uint16_t CONCURRENCY_LEVEL_DEFAULT = 1;
    const static uint64_t CONNECT_TIMEOUT_DEFAULT = 5 * Time::NS_IN_S;
    const static uint64_t RECV_TIMEOUT_DEFAULT = 5 * Time::NS_IN_S;
    const static uint16_t RECV_TRIES_MAX_DEFAULT = 3;
    const static uint64_t SEND_TIMEOUT_DEFAULT = 5 * Time::NS_IN_S;
    const static uint16_t SEND_TRIES_MAX_DEFAULT = 3;

  public:
    Configuration
    (
      uint16_t concurrency_level = CONCURRENCY_LEVEL_DEFAULT,
      const Time& connect_timeout = CONNECT_TIMEOUT_DEFAULT,
      const Time& recv_timeout = RECV_TIMEOUT_DEFAULT,
      const uint16_t recv_tries_max = RECV_TRIES_MAX_DEFAULT,
      const Time& send_timeout = SEND_TIMEOUT_DEFAULT,
      const uint16_t send_tries_max = SEND_TRIES_MAX_DEFAULT
    );

    uint16_t get_concurrency_level() const {
      return concurrency_level;
    }
    const Time& get_connect_timeout() const {
      return connect_timeout;
    }
    const Time& get_recv_timeout() const {
      return recv_timeout;
    }
    uint16_t get_recv_tries_max() const {
      return recv_tries_max;
    }
    const Time& get_send_timeout() const {
      return send_timeout;
    }
    uint16_t get_send_tries_max() const {
      return send_tries_max;
    }

    void set_connect_timeout(const Time& connect_timeout);
    void set_recv_timeout(const Time& recv_timeout);
    void set_send_timeout(const Time& send_timeout);

  private:
    uint16_t concurrency_level;
    Time connect_timeout;
    Time recv_timeout;
    uint16_t recv_tries_max;
    Time send_timeout;
    uint16_t send_tries_max;
  };

public:
  const Configuration& get_configuration() const {
    return configuration;
  }
  yield::sockets::SocketAddress& get_peername() {
    return peername;
  }

protected:
  class connectAIOCB;


  class Connection : public StreamSocketPeer<SocketClient>::Connection {
  public:
    Connection
    (
      StreamSocketClient&,
      YO_NEW_REF yield::sockets::StreamSocket&
    );

    virtual ~Connection() { }

    virtual void handle(YO_NEW_REF connectAIOCB& connect_aiocb) = 0;

  protected:
    template <class RequestType>
    class RequestState {
    public:
      RequestState
      (
        YO_NEW_REF RequestType& request,
        YO_NEW_REF Buffer& request_buffer
      )
        : request(request),
          request_buffer(request_buffer) {
        recv_tries = send_tries = 0;
      }

      ~RequestState() {
        RequestType::dec_ref(request);
        Buffer::dec_ref(request_buffer);
      }

      RequestType& get_request() {
        return request;
      }
      Buffer& get_request_buffer() {
        return request_buffer;
      }
      uint16_t get_recv_tries() const {
        return recv_tries;
      }
      uint16_t get_send_tries() const {
        return send_tries;
      }
      uint16_t inc_recv_tries() {
        return ++recv_tries;
      }
      uint16_t inc_send_tries() {
        return ++send_tries;
      }

    private:
      RequestType& request;
      Buffer& request_buffer;
      uint16_t recv_tries, send_tries;
    };

  protected:
    void enqueue(YO_NEW_REF connectAIOCB& connect_aiocb);

    const Configuration& get_configuration() const {
      return configuration;
    }

  private:
    Configuration configuration;
  };


  class connectAIOCB
    : public yield::aio::sockets::connectAIOCB,
      public StreamSocketPeer<SocketClient>::AIOCB {
  public:
    connectAIOCB
    (
      Connection& connection,
      YO_NEW_REF Buffer* send_buffer = NULL
    )
      : yield::aio::sockets::connectAIOCB
      (
        connection.get_socket(),
        connection.get_peername(),
        send_buffer
      ),
      StreamSocketPeer<SocketClient>::AIOCB(connection)
    { }
  };

protected:
  StreamSocketClient
  (
    const Configuration& configuration,
    Log* error_log,
    Log* trace_log,
    const yield::uri::URI& uri
  );

  virtual ~StreamSocketClient();

  // Stage
  virtual void service(YO_NEW_REF Event& event);

private:
  Configuration configuration;
  yield::sockets::SocketAddress& peername;
};
}
}

#endif
