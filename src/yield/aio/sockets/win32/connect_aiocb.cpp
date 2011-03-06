// yield/aio/sockets/win32/connect_aiocb.cpp

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
#include "yield/aio/sockets/connect_aiocb.hpp"
#include "yield/sockets/socket.hpp"
#include "yield/sockets/socket_address.hpp"
#include "yield/sockets/win32/winsock.hpp"


namespace yield {
namespace aio {
namespace sockets {
using yield::sockets::SocketAddress;


LPFN_CONNECTEX lpfnConnectEx = NULL;


bool connectAIOCB::issue(yield::aio::win32::AIOQueue&) {
  if (lpfnConnectEx == NULL) {
    GUID GuidConnectEx = WSAID_CONNECTEX;
    DWORD dwBytes;
    WSAIoctl
    (
      get_socket(),
      SIO_GET_EXTENSION_FUNCTION_POINTER,
      &GuidConnectEx,
      sizeof(GuidConnectEx),
      &lpfnConnectEx,
      sizeof(lpfnConnectEx),
      &dwBytes,
      NULL,
      NULL
    );

    if (lpfnConnectEx == NULL)
      return false;
  }

  const SocketAddress* peername
  = get_peername().filter(get_socket().get_domain());

  if (peername != NULL) {
    PVOID lpSendBuffer;
    DWORD dwSendDataLength;
    if (get_send_buffer() != NULL) {
      lpSendBuffer = *get_send_buffer();
      dwSendDataLength = get_send_buffer()->size();
    } else {
      lpSendBuffer = NULL;
      dwSendDataLength = 0;
    }

    DWORD dwBytesSent;

    return lpfnConnectEx
           (
             get_socket(),
             *peername,
             peername->len(),
             lpSendBuffer,
             dwSendDataLength,
             &dwBytesSent,
             *this
           )
           ||
           WSAGetLastError() == WSA_IO_PENDING;
  } else
    return false;
}
}
}
}
