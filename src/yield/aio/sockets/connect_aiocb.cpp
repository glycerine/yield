// yield/aio/sockets/connect_aiocb.cpp

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

#include "yield/assert.hpp"
#include "yield/buffer.hpp"
#include "yield/aio/sockets/connect_aiocb.hpp"
#include "yield/sockets/socket_address.hpp"
#include "yield/sockets/stream_socket.hpp"


namespace yield {
namespace aio {
namespace sockets {
using yield::sockets::SocketAddress;
using yield::sockets::StreamSocket;


connectAIOCB::connectAIOCB
(
  StreamSocket& socket_,
  SocketAddress& peername,
  YO_NEW_REF Buffer* send_buffer
)
  : AIOCB(socket_, NULL, 0),
    peername(peername.inc_ref()),
    send_buffer(send_buffer)
{ }

connectAIOCB::connectAIOCB(connectAIOCB& other)
  : AIOCB(other.get_socket(), NULL, 0),
    peername(other.peername.inc_ref()),
    send_buffer(Object::inc_ref(other.send_buffer))
{ }

connectAIOCB::~connectAIOCB() {
  SocketAddress::dec_ref(peername);
  Buffer::dec_ref(send_buffer);
}

const SocketAddress& connectAIOCB::get_peername() const {
  return peername;
}

connectAIOCB::RetryStatus connectAIOCB::retry() {
  debug_assert_eq(get_send_buffer(), NULL);   // Not implemented yet

  StreamSocket& socket_ = static_cast<StreamSocket&>(get_socket());

  if (socket_.connect(get_peername())) {
    set_return(0);
    return RETRY_STATUS_COMPLETE;
  } else if (socket_.want_connect())
    return RETRY_STATUS_WANT_WRITE;
  else {
    set_error(Exception::get_last_error_code());
    return RETRY_STATUS_ERROR;
  }
}
}
}
}