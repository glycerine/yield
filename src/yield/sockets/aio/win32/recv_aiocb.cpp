// yield/sockets/aio/win32/recv_aiocb.cpp

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
#include "yield/sockets/aio/recv_aiocb.hpp"
#include "yield/sockets/win32/winsock.hpp"

namespace yield {
namespace sockets {
namespace aio {
static int
WSARecvFrom(
  Socket& socket_,
  Buffer& buffer,
  const Socket::MessageFlags& flags,
  SocketAddress& peername,
  LPWSAOVERLAPPED lpOverlapped,
  LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
) {
  DWORD dwFlags = static_cast<DWORD>(flags);
  sockaddr* lpFrom = peername;
  socklen_t lpFromlen = peername.len();

  //if (buffer.get_next_buffer() == NULL) {
    WSABUF wsabuf;
    wsabuf.buf = static_cast<char*>(buffer)
                 + buffer.size();
    wsabuf.len = buffer.capacity() - buffer.size();

    return WSARecvFrom
           (
             socket_,
             &wsabuf,
             1,
             NULL,
             &dwFlags,
             lpFrom,
             &lpFromlen,
             lpOverlapped,
             lpCompletionRoutine
           );
  //} else { // Scatter I/O
  //  vector<WSABUF> wsabufs;
  //  Buffer* next_buffer = &buffer;
  //  do {
  //    WSABUF wsabuf;
  //    wsabuf.buf = static_cast<char*>(*next_buffer)
  //                 + next_buffer->size();
  //    wsabuf.len = next_buffer->capacity() - next_buffer->size();
  //    wsabufs.push_back(wsabuf);
  //    next_buffer = next_buffer->get_next_buffer();
  //  } while (next_buffer != NULL);

  //  return WSARecvFrom(
  //           socket_,
  //           &wsabufs[0],
  //           wsabufs.size(),
  //           NULL,
  //           &dwFlags,
  //           lpFrom,
  //           &lpFromlen,
  //           lpOverlapped,
  //           lpCompletionRoutine
  //         );
  //}
}


bool recvAIOCB::issue(EventHandler& completion_handler) {
  set_completion_handler(completion_handler);

  return WSARecvFrom
         (
           get_socket(),
           get_buffer(),
           get_flags(),
           get_peername(),
           *this,
           CompletionRoutine
         ) == 0
         ||
         WSAGetLastError() == WSA_IO_PENDING;
}

bool recvAIOCB::issue(yield::aio::win32::AIOQueue&) {
  return WSARecvFrom(
           get_socket(),
           get_buffer(),
           get_flags(),
           get_peername(),
           *this,
           NULL
         ) == 0
         ||
         WSAGetLastError() == WSA_IO_PENDING;
}
}
}
}
