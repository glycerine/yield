// yield/sockets/aio/recv_aiocb.cpp

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
#include "yield/sockets/aio/recv_aiocb.hpp"

namespace yield {
namespace sockets {
namespace aio {
recvAIOCB::~recvAIOCB() {
  Buffer::dec_ref(buffer);
}

void recvAIOCB::set_return(ssize_t return_) {
  if (return_ > 0) {
    Buffer* buffer = &get_buffer();
    size_t ret = static_cast<size_t>(return_);

    for (;;) {
      size_t buffer_left = buffer->capacity() - buffer->size();

      if (ret <= buffer_left) {
        buffer->resize(buffer->size() + ret);
        break;
      } else {
        buffer->resize(buffer->capacity());
        ret -= buffer_left;
        buffer = buffer->get_next_buffer();
      }
    }
  }

  AIOCB::set_return(return_);
}

recvAIOCB::RetryStatus recvAIOCB::retry() {
  ssize_t recv_ret;

  if (get_buffer().get_next_buffer() == NULL) {
    recv_ret
    = get_socket().recvfrom(
        static_cast<char*>(get_buffer()) + get_buffer().size(),
        get_buffer().capacity() - get_buffer().size(),
        get_flags(),
        get_peername()
      );
  } else {
    DebugBreak();
    recv_ret = -1;
  }

  if (recv_ret >= 0) {
    set_return(recv_ret);
    return RETRY_STATUS_COMPLETE;
  } else if (get_socket().want_recv())
    return RETRY_STATUS_WANT_READ;
  else if (get_socket().want_send())
    return RETRY_STATUS_WANT_WRITE;
  else {
    set_error(Exception::get_last_error_code());
    return RETRY_STATUS_ERROR;
  }
}
}
}
}
