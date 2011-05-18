// yield/http/server/http_connection.cpp

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

#include "http_request_parser.hpp"
#include "yield/assert.hpp"
#include "yield/log.hpp"
#include "yield/fs/file.hpp"
#include "yield/http/server/http_connection.hpp"

namespace yield {
namespace http {
namespace server {
using yield::fs::File;
using yield::sockets::SocketAddress;
using yield::sockets::TCPSocket;
using yield::sockets::aio::acceptAIOCB;

HTTPConnection::HTTPConnection(
  EventQueue& aio_queue,
  EventHandler& http_request_handler,
  SocketAddress& peername,
  TCPSocket& socket_,
  Log* log
) : aio_queue(aio_queue.inc_ref()),
    http_request_handler(http_request_handler.inc_ref()),
    log(Object::inc_ref(log)),
    peername(peername.inc_ref()),
    socket_(static_cast<TCPSocket&>(socket_.inc_ref()))
{
  state = STATE_CONNECTED;
}

HTTPConnection::~HTTPConnection() {
  EventQueue::dec_ref(aio_queue);
  EventHandler::dec_ref(http_request_handler);
  Log::dec_ref(log);
  TCPSocket::dec_ref(socket_);
}

void HTTPConnection::handle(YO_NEW_REF acceptAIOCB& accept_aiocb) {
  if (
    accept_aiocb.get_recv_buffer() != NULL
    &&
    accept_aiocb.get_return() > 0
  )
    parse(*accept_aiocb.get_recv_buffer());
  else {
    Buffer* recv_buffer
      = new Buffer(Buffer::getpagesize(), Buffer::getpagesize());
    recvAIOCB* recv_aiocb = new recvAIOCB(*this, *recv_buffer);
    if (!aio_queue.enqueue(*recv_aiocb)) {
      recvAIOCB::dec_ref(*recv_aiocb);
      state = STATE_ERROR;
    }
  }

  acceptAIOCB::dec_ref(accept_aiocb);
}

void
HTTPConnection::handle(
  YO_NEW_REF ::yield::http::HTTPMessageBodyChunk& http_message_body_chunk
) {
  Buffer* send_buffer;
  if (http_message_body_chunk.data() != NULL)
    send_buffer = &http_message_body_chunk.data()->inc_ref();
  else
    send_buffer = &Buffer::copy("0\r\n\r\n", 5);
  HTTPMessageBodyChunk::dec_ref(http_message_body_chunk);

  sendAIOCB* send_aiocb = new sendAIOCB(*this, *send_buffer);
  if (!aio_queue.enqueue(*send_aiocb)) {
    sendAIOCB::dec_ref(*send_aiocb);
    state = STATE_ERROR;
  }
}

void
HTTPConnection::handle(
  YO_NEW_REF ::yield::http::HTTPResponse& http_response
) {
  if (log != NULL) {
    log->get_stream(Log::Level::DEBUG) << get_type_name() 
      << ": sending " << http_response;
  }

  http_response.finalize();
  Buffer& http_response_header = http_response.get_header().inc_ref();
  Object* http_response_body = Object::inc_ref(http_response.get_body());
  HTTPResponse::dec_ref(http_response);

  if (http_response_body != NULL) {
    switch (http_response_body->get_type_id()) {
    case Buffer::TYPE_ID: {
      http_response_header.set_next_buffer(
        static_cast<Buffer*>(http_response_body)
      );
    }
    break;

    case File::TYPE_ID: {
      sendAIOCB* send_aiocb = new sendAIOCB(*this, http_response_header);
      if (aio_queue.enqueue(*send_aiocb)) {
        sendfileAIOCB* sendfile_aiocb
          = new sendfileAIOCB(*this, *static_cast<File*>(http_response_body));
        if (aio_queue.enqueue(*sendfile_aiocb))
          return;
        else {
          sendfileAIOCB::dec_ref(*sendfile_aiocb);
          state = STATE_ERROR;
        }
      } else {
        sendAIOCB::dec_ref(*send_aiocb);
        state = STATE_ERROR;
      }
    }
    break;

    default: debug_break();
    }
  }

  sendAIOCB* send_aiocb = new sendAIOCB(*this, http_response_header);
  if (!aio_queue.enqueue(*send_aiocb)) {
    sendAIOCB::dec_ref(*send_aiocb);
    state = STATE_ERROR;
  }
}

void
HTTPConnection::handle(
  YO_NEW_REF ::yield::sockets::aio::recvAIOCB& recv_aiocb
) {
  if (recv_aiocb.get_return() > 0)
    parse(recv_aiocb.get_buffer());

  ::yield::sockets::aio::recvAIOCB::dec_ref(recv_aiocb);
}

void
HTTPConnection::handle(
  YO_NEW_REF ::yield::sockets::aio::sendAIOCB& send_aiocb
){
  sendAIOCB::dec_ref(send_aiocb);
}

void
HTTPConnection::handle(
  YO_NEW_REF ::yield::sockets::aio::sendfileAIOCB& sendfile_aiocb
) {
  ::yield::sockets::aio::sendfileAIOCB::dec_ref(sendfile_aiocb);
}

void HTTPConnection::parse(Buffer& recv_buffer) {
  debug_assert_false(recv_buffer.empty());

  HTTPRequestParser http_request_parser(*this, recv_buffer);

  for (;;) {
    Object& object = http_request_parser.parse();

    switch (object.get_type_id()) {
    case Buffer::TYPE_ID: {
      Buffer& next_recv_buffer = static_cast<Buffer&>(object);
      recvAIOCB* recv_aiocb = new recvAIOCB(*this, next_recv_buffer);
      if (!aio_queue.enqueue(*recv_aiocb)) {
        recvAIOCB::dec_ref(*recv_aiocb);
        state = STATE_ERROR;
      }
    }
    return;

    case HTTPRequest::TYPE_ID: {
      HTTPRequest& http_request = static_cast<HTTPRequest&>(object);

      if (log != NULL) {
        log->get_stream(Log::Level::DEBUG) << get_type_name() 
          << ": parsed " << http_request;
      }

      http_request_handler.handle(http_request);
    }
    break;

    case HTTPResponse::TYPE_ID: {
      HTTPResponse& http_response = static_cast<HTTPResponse&>(object);
      debug_assert_eq(http_response.get_status_code(), 400);

      if (log != NULL) {
        log->get_stream(Log::Level::DEBUG) << get_type_name() 
          << ": parsed " << http_response;
      }

      handle(http_response);
      return;
    }
    break;

    default:
      debug_break();
      break;
    }
  }
}
}
}
}
