// yield/http/client/http_client.cpp

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

#include "yield/auto_object.hpp"
#include "yield/assert.hpp"
#include "yield/log.hpp"
#include "yield/http/http_request.hpp"
#include "yield/http/http_response.hpp"
#include "yield/http/http_response_parser.hpp"
#include "yield/http/client/http_client.hpp"
#include "yield/sockets/socket_address.hpp"
#include "yield/sockets/tcp_socket.hpp"
#include "yield/thread/synchronized_response_queue.hpp"

#include <iostream> // for std::cout

namespace yield {
namespace http {
namespace client {
using yield::sockets::SocketAddress;
using yield::sockets::StreamSocket;
using yield::sockets::TCPSocket;
using yield::sockets::client::StreamSocketClient;
using yield::sockets::peer::StreamSocketPeer;
using yield::uri::URI;

typedef yield::thread::SynchronizedResponseQueue<HTTPResponse> HTTPResponseQueue;

class HTTPClient::Connection : public StreamSocketClient::Connection {
public:
  Connection(HTTPClient& client)
    : StreamSocketClient::Connection(client, *new TCPSocket) {
    connected = false;
  }

  void handle(YO_NEW_REF connectAIOCB& connect_aiocb) {
    debug_assert_false(http_request_queue.empty());

    if (connect_aiocb.get_error() == 0) {
      connected = true;

      Buffer* recv_buffer = new Buffer(Buffer::getpagesize(), Buffer::getpagesize());
      recvAIOCB* recv_aiocb = new recvAIOCB(*this, *recv_buffer);
      StreamSocketPeer<SocketClient>::Connection::enqueue(*recv_aiocb);
    } else {
      Exception* exception = new Exception(connect_aiocb.get_error());
      debug_assert_false(http_request_queue.empty());
      HTTPRequestState* http_request_state = http_request_queue.front();
      http_request_queue.pop();
      http_request_state->get_request().respond(*exception);
      delete http_request_state;
    }

    connectAIOCB::dec_ref(connect_aiocb);
  }

  void handle(YO_NEW_REF HTTPRequest& http_request) {
    Buffer& http_request_buffer = http_request;

    HTTPRequestState* http_request_state
    = new HTTPRequestState(http_request, http_request_buffer.inc_ref());

    http_request_queue.push(http_request_state);

    if (!connected) {
      connectAIOCB* connect_aiocb
      = new connectAIOCB(*this, &http_request_buffer.inc_ref());
      enqueue(*connect_aiocb);
    } else {
      sendAIOCB* send_aiocb
      = new sendAIOCB(*this, http_request_buffer.inc_ref());
      StreamSocketPeer<SocketClient>::Connection::enqueue(*send_aiocb);
    }
  }

  void handle(YO_NEW_REF recvAIOCB& recv_aiocb) {
    debug_assert_false(http_request_queue.empty());

    if (recv_aiocb.get_return() > 0) {
      HTTPResponseParser http_response_parser(recv_aiocb.get_buffer());

      for (;;) {
        Object& object = http_response_parser.parse();

        switch (object.get_type_id()) {
        case Buffer::TYPE_ID: {
          Buffer& next_recv_buffer = static_cast<Buffer&>(object);
          recvAIOCB* next_recv_aiocb = new recvAIOCB(*this, next_recv_buffer);
          StreamSocketPeer<SocketClient>::Connection::enqueue(*next_recv_aiocb);
          return;
        }
        break;

        case HTTPResponse::TYPE_ID: {
          HTTPResponse& http_response = static_cast<HTTPResponse&>(object);
          debug_assert_false(http_request_queue.empty());
          HTTPRequestState* http_request_state = http_request_queue.front();
          http_request_queue.pop();
          http_request_state->get_request().respond(http_response);
          delete http_request_state;
        }
        break;

        default:
          DebugBreak();
          break;
        }
      }
    } else
      DebugBreak();
  }

  void handle(YO_NEW_REF sendAIOCB& send_aiocb) {
    debug_assert_false(http_request_queue.empty());

    if (send_aiocb.get_error() != 0)
      DebugBreak();

    sendAIOCB::dec_ref(send_aiocb);
  }

  // EventHandler
  void handle(YO_NEW_REF Event&) {
    DebugBreak();
  }

private:
  bool connected;
  typedef RequestState<HTTPRequest> HTTPRequestState;
  std::queue<HTTPRequestState*> http_request_queue;
};


HTTPClient::HTTPClient
(
  const yield::uri::URI& uri,
  Log* error_log,
  Log* trace_log
)
  : StreamSocketClient
  (
    Configuration(),
    error_log,
    trace_log,
    uri
  ) {
  init();
}

HTTPClient::HTTPClient
(
  const Configuration& configuration,
  const yield::uri::URI& uri,
  Log* error_log,
  Log* trace_log
)
  : StreamSocketClient
  (
    configuration,
    error_log,
    trace_log,
    uri
  ) {
  init();
}

HTTPClient::~HTTPClient() {
  for
  (
    vector<Connection*>::iterator connection_i = connections.begin();
    connection_i != connections.end();
    ++connection_i
  )
    Connection::dec_ref(**connection_i);
}

YO_NEW_REF HTTPResponse& HTTPClient::GET(const URI& uri) {
  auto_Object<Log> trace_log = Log::open(std::cout, Log::DEBUG);

  auto_Object<HTTPClient> http_client
  = new HTTPClient(uri, NULL, &trace_log.get());

  auto_Object<HTTPResponseQueue> http_response_queue
  = new HTTPResponseQueue;

  HTTPRequest* http_request = new HTTPRequest(HTTPRequest::METHOD_GET, uri);
  http_request->set_response_handler(*http_response_queue);

  http_client->handle(*http_request);

  for (;;) {
    http_client->visit();
    HTTPResponse* http_response = http_response_queue->trydequeue();
    if (http_response != NULL)
      return *http_response;
  }
}

void HTTPClient::init() {
  for
  (
    uint16_t connection_i = 0;
    connection_i < get_configuration().get_concurrency_level();
    connection_i++
  )
    connections.push_back(new Connection(*this));
}

void HTTPClient::service(YO_NEW_REF Event& event) {
  switch (event.get_type_id()) {
  case HTTPRequest::TYPE_ID: {
    connections[0]->handle(static_cast<HTTPRequest&>(event));
  }
  break;

  default:
    StreamSocketClient::service(event);
    break;
  }
}
}
}
}
