// yield/aio/sockets/win32/accept_aiocb.cpp

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
#include "yield/page.hpp"
#include "yield/aio/sockets/accept_aiocb.hpp"
#include "yield/sockets/socket_address.hpp"
#include "yield/sockets/stream_socket.hpp"
#include "yield/sockets/win32/winsock.hpp"


namespace yield {
namespace aio {
namespace sockets {
using yield::sockets::SocketAddress;
using yield::sockets::StreamSocket;


static LPFN_ACCEPTEX lpfnAcceptEx = NULL;
static LPFN_GETACCEPTEXSOCKADDRS lpfnGetAcceptExSockaddrs = NULL;


acceptAIOCB::acceptAIOCB
(
  StreamSocket& socket_,
  YO_NEW_REF Buffer* recv_buffer
)
  : AIOCB(socket_, NULL, 0),
    peername(*new SocketAddress),
    recv_buffer(recv_buffer) {
  accepted_socket = NULL;

  if (this->recv_buffer == NULL) {
    size_t recv_buffer_size
    = get_sockname_length() + get_peername_length();
    this->recv_buffer = new Page(recv_buffer_size);
  } else {
    debug_assert_ge
    (
      this->recv_buffer->capacity() - this->recv_buffer->size(),
      get_sockname_length() + get_peername_length()
    );
  }
}

void acceptAIOCB::set_return(ssize_t return_) {
  int optval = get_socket();
  setsockopt
  (
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

  lpfnGetAcceptExSockaddrs
  (
    get_output_buffer(),
    get_recv_data_length(),
    get_sockname_length(),
    get_peername_length(),
    &sockname,
    &socknamelen,
    &peername,
    &peernamelen
  );

  if (peername != NULL) {
    get_peername().assign
    (
      *peername,
      get_accepted_socket()->get_domain()
    );
  }

  if (return_ > 0)
    recv_buffer->resize(recv_buffer->size() + return_);

  AIOCB::set_return(return_);
}

void* acceptAIOCB::get_output_buffer() const {
  return static_cast<char*>(*recv_buffer) + recv_buffer->size();
}

uint32_t acceptAIOCB::get_peername_length() const {
  return get_peername().len() + 16;
}

uint32_t acceptAIOCB::get_recv_data_length() const {
  return recv_buffer->capacity()
         - recv_buffer->size()
         - get_sockname_length()
         - get_peername_length();
}

uint32_t acceptAIOCB::get_sockname_length() const {
  return get_peername().len() + 16;
}

bool acceptAIOCB::issue(yield::aio::win32::AIOQueue&) {
  if (lpfnAcceptEx == NULL) {
    GUID GuidAcceptEx = WSAID_ACCEPTEX;
    DWORD dwBytes;
    WSAIoctl
    (
      get_socket(),
      SIO_GET_EXTENSION_FUNCTION_POINTER,
      &GuidAcceptEx,
      sizeof(GuidAcceptEx),
      &lpfnAcceptEx,
      sizeof(lpfnAcceptEx),
      &dwBytes,
      NULL,
      NULL
    );

    if (lpfnAcceptEx == NULL)
      return false;

    GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
    WSAIoctl
    (
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
      return false;
  }

  accepted_socket = static_cast<StreamSocket&>(get_socket()).dup();

  if (accepted_socket != NULL) {
    DWORD dwBytesReceived;

    return lpfnAcceptEx
           (
             get_socket(),
             *accepted_socket,
             get_output_buffer(),
             get_recv_data_length(),
             get_sockname_length(),
             get_peername_length(),
             &dwBytesReceived,
             *this
           ) == TRUE
           ||
           WSAGetLastError() == WSA_IO_PENDING;
  } else
    return false;
}
}
}
}
