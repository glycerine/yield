// yield/sockets/aio/send_aiocb.cpp

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

#include "yield/buffer.hpp"
#include "yield/sockets/socket_address.hpp"
#include "yield/sockets/aio/send_aiocb.hpp"

namespace yield {
namespace sockets {
namespace aio {
sendAIOCB::~sendAIOCB() {
  Buffer::dec_ref(buffer);
  SocketAddress::dec_ref(peername);
}

sendAIOCB::RetryStatus sendAIOCB::retry() {
  ssize_t send_ret;

  if (get_buffer().get_next_buffer() == NULL) {
    if (get_peername() == NULL) {
      send_ret
      = get_socket().send(
          get_buffer(),
          get_buffer().size(),
          get_flags()
        );
    } else {
      send_ret
      = get_socket().sendto(
          get_buffer(),
          get_buffer().size(),
          get_flags(),
          *get_peername()
        );
    }
  } else {
    vector<iovec> iovs;
    Buffer* next_buffer = &get_buffer();
    do {
      iovec iov;
      iov.iov_base = *next_buffer;
      iov.iov_len = next_buffer->size();
      next_buffer = next_buffer->get_next_buffer();
    } while (next_buffer != NULL);

    send_ret
    = get_socket().sendmsg(
        &iovs[0],
        iovs.size(),
        get_flags(),
        get_peername()
      );
  }

  if (send_ret >= 0) {
    set_return(send_ret);
    return RETRY_STATUS_COMPLETE;
  } else if (get_socket().want_send())
    return RETRY_STATUS_WANT_WRITE;
  else if (get_socket().want_recv())
    return RETRY_STATUS_WANT_READ;
  else {
    set_error(Exception::get_last_error_code());
    return RETRY_STATUS_ERROR;
  }
}
}
}
}
