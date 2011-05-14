// yield/http/server/http_request_queue.cpp

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

#include "yield/log.hpp"
#include "yield/http/server/http_connection.hpp"
#include "yield/http/server/http_request_queue.hpp"
#include "yield/sockets/tcp_socket.hpp"
#include "yield/sockets/aio/accept_aiocb.hpp"
#include "yield/sockets/aio/aio_queue.hpp"

namespace yield {
namespace http {
namespace server {
using yield::sockets::Socket;
using yield::sockets::SocketAddress;
using yield::sockets::TCPSocket;
using yield::sockets::aio::acceptAIOCB;
using yield::sockets::aio::AIOQueue;

template <class AIOQueueType>
HTTPRequestQueue<AIOQueueType>::HTTPRequestQueue(
  const SocketAddress& sockname,
  YO_NEW_REF Log* log
) throw (Exception) : aio_queue(*new AIOQueueType(log)),
    log(Object::inc_ref(log)),
    socket_(*new TCPSocket(sockname.get_family())) {
  init(sockname);
}

template <class AIOQueueType>
HTTPRequestQueue<AIOQueueType>::HTTPRequestQueue(
  YO_NEW_REF TCPSocket& socket_,
  const SocketAddress& sockname,
  YO_NEW_REF Log* log
) throw (Exception) : aio_queue(*new AIOQueueType(log)),
    log(Object::inc_ref(log)),
    socket_(socket_) {
  init(sockname);
}

template <class AIOQueueType>
HTTPRequestQueue<AIOQueueType>::~HTTPRequestQueue() {
  for (
    vector<HTTPConnection*>::iterator connection_i = connections.begin();
    connection_i != connections.end();
    ++connection_i
  ) {
    HTTPConnection* connection = *connection_i;
    TCPSocket& socket_ = connection->get_socket();
    socket_.set_blocking_mode(true);
    socket_.setsockopt(TCPSocket::Option::LINGER, 30);
    socket_.shutdown();
    socket_.close();
    HTTPConnection::dec_ref(*connection);
  }

  socket_.close();

  AIOQueue::dec_ref(aio_queue);
  Log::dec_ref(log);
  TCPSocket::dec_ref(socket_);
}

template <class AIOQueueType>
bool HTTPRequestQueue<AIOQueueType>::enqueue(YO_NEW_REF Event& event) {
  return aio_queue.enqueue(event);
}

template <class AIOQueueType>
void HTTPRequestQueue<AIOQueueType>::handle(YO_NEW_REF acceptAIOCB& accept_aiocb) {
  if (accept_aiocb.get_return() >= 0) {
    TCPSocket& accepted_socket
      = static_cast<TCPSocket&>(*accept_aiocb.get_accepted_socket());

    if (aio_queue.associate(accepted_socket)) {
      HTTPConnection* connection
        = new HTTPConnection(
                  aio_queue,
                  aio_queue,
                  *accept_aiocb.get_peername(),
                  static_cast<TCPSocket&>(accepted_socket),
                  log                  
                );

      connection->handle(accept_aiocb);

      if (connection->get_state() == HTTPConnection::STATE_CONNECTED)
        connections.push_back(connection);
      else
        HTTPConnection::dec_ref(*connection);
    } else {
      accepted_socket.shutdown();
      accepted_socket.close();
      TCPSocket::dec_ref(accepted_socket);
    }
  } else {
    acceptAIOCB::dec_ref(accept_aiocb);
  }

  Buffer* recv_buffer
    = new Buffer(Buffer::getpagesize(), Buffer::getpagesize());
  acceptAIOCB* next_accept_aiocb = new acceptAIOCB(socket_, recv_buffer);
  if (!aio_queue.enqueue(*next_accept_aiocb))
    acceptAIOCB::dec_ref(next_accept_aiocb);
}

template <class AIOQueueType>
template <class AIOCBType>
void HTTPRequestQueue<AIOQueueType>::handle(YO_NEW_REF AIOCBType& aiocb) {
  HTTPConnection& connection = aiocb.get_connection();
  if (connection.get_state() == HTTPConnection::STATE_CONNECTED) {
    connection.handle(aiocb);

    if (connection.get_state() == HTTPConnection::STATE_ERROR) {
      for (
        vector<HTTPConnection*>::iterator connection_i = connections.begin();
        connection_i != connections.end();
        ++connection_i
      ) {
        if (*connection_i == &connection) {
          connections.erase(connection_i);
          connection.get_socket().close();
          HTTPConnection::dec_ref(connection);
        }
      }
    }
  }
  else
    AIOCBType::dec_ref(aiocb);
}

template <class AIOQueueType>
void
HTTPRequestQueue<AIOQueueType>::init(
  const SocketAddress& sockname
) throw (Exception) {
  if (aio_queue.associate(socket_)) {
#ifdef __linux__
    if (socket_.setsockopt(Socket::Option::REUSEADDR, true)) {
#endif
      if (socket_.bind(sockname)) {
        if (socket_.listen()) {
          Buffer* recv_buffer
            = new Buffer(Buffer::getpagesize(), Buffer::getpagesize());
          acceptAIOCB* accept_aiocb = new acceptAIOCB(socket_, recv_buffer);
          if (aio_queue.enqueue(*accept_aiocb))
            return;
        }
      }
#ifdef __linux__
    }
#endif
  }

  throw Exception();
}

template <class AIOQueueType>
YO_NEW_REF Event* HTTPRequestQueue<AIOQueueType>::timeddequeue(const Time& timeout) {
  Time timeout_remaining(timeout);

  for (;;) {
    Time start_time = Time::now();

    Event* event = aio_queue.timeddequeue(timeout_remaining);

    if (event != NULL) {
      switch (event->get_type_id()) {
      case acceptAIOCB::TYPE_ID: {
        handle(static_cast<acceptAIOCB&>(*event));
      }
      break;

      case HTTPConnection::recvAIOCB::TYPE_ID: {
        handle<HTTPConnection::recvAIOCB>(
          static_cast<HTTPConnection::recvAIOCB&>(*event)
        );
      }
      break;

      case HTTPConnection::sendAIOCB::TYPE_ID: {
        handle<HTTPConnection::sendAIOCB>(
          static_cast<HTTPConnection::sendAIOCB&>(*event)
        );
      }
      break;

      case HTTPConnection::sendfileAIOCB::TYPE_ID: {
        handle<HTTPConnection::sendfileAIOCB>(
          static_cast<HTTPConnection::sendfileAIOCB&>(*event)
        );
      }
      break;

      default: return event;
      }
    } else if (timeout_remaining > static_cast<uint64_t>(0)) {
      Time elapsed_time = Time::now() - start_time;
      if (timeout_remaining > elapsed_time)
        timeout_remaining -= elapsed_time;
      else
        timeout_remaining = 0;
    } else
      return NULL;
  }
}

#ifdef _WIN32
template class HTTPRequestQueue<yield::sockets::aio::win32::AIOQueue>;
#endif
template class HTTPRequestQueue<yield::sockets::aio::NBIOQueue>;
}
}
}
