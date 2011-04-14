// yield/sockets/aio/connect_aiocb.cpp

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
#include "yield/sockets/stream_socket.hpp"
#include "yield/sockets/aio/connect_aiocb.hpp"

namespace yield {
namespace sockets {
namespace aio {
connectAIOCB::connectAIOCB(
  StreamSocket& socket_,
  SocketAddress& peername,
  YO_NEW_REF Buffer* send_buffer
) : AIOCB(socket_),
    peername(peername.inc_ref()),
    send_buffer(send_buffer) {
  if (send_buffer != NULL)
    debug_assert_eq(send_buffer->get_next_buffer(), NULL);
}

connectAIOCB::~connectAIOCB() {
  SocketAddress::dec_ref(peername);
  Buffer::dec_ref(send_buffer);
}

StreamSocket& connectAIOCB::get_socket() {
  return static_cast<StreamSocket&>(AIOCB::get_socket());
}

std::ostream& operator<<(std::ostream& os, connectAIOCB& connect_aiocb) {
  os <<
    connect_aiocb.get_type_name() <<
    "(" <<
      "error=" << connect_aiocb.get_error() <<
      ", " <<
      "peername=" << connect_aiocb.get_peername() <<
      ", " <<
      "return=" << connect_aiocb.get_return() <<
      ", " <<
      "send_buffer=";
      if (connect_aiocb.get_send_buffer() != NULL)
        os << *connect_aiocb.get_send_buffer();
      else
        os << "NULL";
      os <<
      "," <<
      "socket=" << connect_aiocb.get_socket() <<
    ")";
  return os;
}
}
}
}