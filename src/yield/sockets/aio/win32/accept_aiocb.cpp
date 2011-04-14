// yield/sockets/aio/win32/accept_aiocb.cpp

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
#include "yield/sockets/aio/accept_aiocb.hpp"
#include "yield/sockets/win32/winsock.hpp"

namespace yield {
namespace sockets {
namespace aio {
static LPFN_GETACCEPTEXSOCKADDRS lpfnGetAcceptExSockaddrs = NULL;

acceptAIOCB::acceptAIOCB(StreamSocket& socket_, YO_NEW_REF Buffer* recv_buffer)
  : AIOCB(socket_),
    peername(*new SocketAddress)
{
  accepted_socket = NULL;

  if (recv_buffer == NULL)
    this->recv_buffer = new Buffer((this->peername.len() + 16) * 2);
  else {
    debug_assert_eq(recv_buffer->get_next_buffer(), NULL);
    debug_assert_ge(
      recv_buffer->capacity() - recv_buffer->size(),
      static_cast<size_t>((this->peername.len() + 16) * 2)
    );
    this->recv_buffer = recv_buffer;
  }
}

void acceptAIOCB::set_return(ssize_t return_) {
  if (lpfnGetAcceptExSockaddrs == NULL) {
    DWORD dwBytes;
    GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
    WSAIoctl(
      get_socket(),
      SIO_GET_EXTENSION_FUNCTION_POINTER,
      &GuidGetAcceptExSockAddrs,
      sizeof(GuidGetAcceptExSockAddrs),
      &lpfnGetAcceptExSockaddrs,
      sizeof(lpfnGetAcceptExSockaddrs),
      &dwBytes,
      NULL,
      NULL
    );

    if (lpfnGetAcceptExSockaddrs == NULL)
      return;
  }

  int optval = get_socket();
  setsockopt(
    *get_accepted_socket(),
    SOL_SOCKET,
    SO_UPDATE_ACCEPT_CONTEXT,
    reinterpret_cast<char*>(&optval),
    sizeof(optval)
  );

  sockaddr* peername = NULL;
  socklen_t peernamelen;
  sockaddr* sockname;
  socklen_t socknamelen;

  lpfnGetAcceptExSockaddrs(
    static_cast<char*>(*recv_buffer) + recv_buffer->size(),
    recv_buffer->capacity()
      - recv_buffer->size()
      - ((this->peername.len() + 16) * 2),
    this->peername.len() + 16,
    this->peername.len() + 16,
    &sockname,
    &socknamelen,
    &peername,
    &peernamelen
  );

  if (peername != NULL) {
    get_peername().assign(
      *peername,
      accepted_socket->get_domain()
    );
  }

  if (return_ > 0)
      recv_buffer->resize(recv_buffer->size() + return_);

  AIOCB::set_return(return_);
}
}
}
}
