// yield/aio/net/sockets/accept_aiocb.hpp

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


#ifndef _YIELD_AIO_NET_SOCKETS_ACCEPT_AIOCB_HPP_
#define _YIELD_AIO_NET_SOCKETS_ACCEPT_AIOCB_HPP_


#include "yield/aio/net/sockets/aiocb.hpp"


namespace yield {
class Buffer;


namespace net {
namespace sockets {
class SocketAddress;
class StreamSocket;
}
}


namespace aio {
namespace net {
namespace sockets {
class acceptAIOCB : public AIOCB {
public:
  const static uint32_t TYPE_ID = 3895043741UL;

public:
  acceptAIOCB
  (
    yield::net::sockets::StreamSocket& socket_,
    YO_NEW_REF Buffer* recv_buffer = NULL
  );
  ~acceptAIOCB();

  yield::net::sockets::StreamSocket* get_accepted_socket() const;
  yield::net::sockets::SocketAddress& get_peername() const;
  Buffer* get_recv_buffer() const {
    return recv_buffer;
  }

  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }
  const char* get_type_name() const {
    return "yield::aio::net::sockets::acceptAIOCB";
  }

  // yield::aio::AIOCB
#ifdef _WIN32
  bool issue(yield::aio::win32::AIOQueue&);
#endif
  RetryStatus retry();
  void set_return(ssize_t return_);

private:
  acceptAIOCB(acceptAIOCB& other);

#ifdef _WIN32
  void* get_output_buffer() const;
  uint32_t get_peername_length() const;
  uint32_t get_recv_data_length() const;
  uint32_t get_sockname_length() const;
#endif

private:
  yield::net::sockets::StreamSocket* accepted_socket;
  yield::net::sockets::SocketAddress& peername;
  Buffer* recv_buffer;
};
}
}
}
}


#endif
