// yield/sockets/aio/win32/aio_queue.cpp

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
#include "yield/sockets/stream_socket.hpp"
#include "yield/sockets/aio/accept_aiocb.hpp"
#include "yield/sockets/aio/connect_aiocb.hpp"
#include "yield/sockets/aio/recv_aiocb.hpp"
#include "yield/sockets/aio/send_aiocb.hpp"
#include "yield/sockets/aio/win32/aio_queue.hpp"
#include "yield/sockets/win32/winsock.hpp"

namespace yield {
namespace sockets {
namespace aio {
namespace win32 {
static LPFN_ACCEPTEX lpfnAcceptEx = NULL;
static LPFN_CONNECTEX lpfnConnectEx = NULL;

bool AIOQueue::associate(socket_t socket_) {
  return yield::aio::win32::AIOQueue::associate(socket_to_fd(socket_));
}

bool AIOQueue::enqueue(Event& event) {
  switch (event.get_type_id()) {
  case acceptAIOCB::TYPE_ID: {
    acceptAIOCB& accept_aiocb = static_cast<acceptAIOCB&>(event);

    if (lpfnAcceptEx == NULL) {
      GUID GuidAcceptEx = WSAID_ACCEPTEX;
      DWORD dwBytes;
      WSAIoctl(
        accept_aiocb.get_socket(),
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
    }

    StreamSocket* accepted_socket = accept_aiocb.get_socket().dup();

    if (accepted_socket != NULL) {
      accept_aiocb.set_accepted_socket(*accepted_socket);

      DWORD dwBytesReceived;

      return lpfnAcceptEx(
                accept_aiocb.get_socket(),
                *accepted_socket,
                static_cast<char*>(*accept_aiocb.get_recv_buffer()) +
                  accept_aiocb.get_recv_buffer()->size(),
                accept_aiocb.get_recv_buffer()->capacity()
                  - accept_aiocb.get_recv_buffer()->size()
                  - ((accept_aiocb.get_peername().len() + 16) * 2),
                accept_aiocb.get_peername().len() + 16,
                accept_aiocb.get_peername().len() + 16,
                &dwBytesReceived,
                accept_aiocb
              ) == TRUE
              ||
              WSAGetLastError() == WSA_IO_PENDING;
    } else
      return false;

  }
  break;

  case connectAIOCB::TYPE_ID: {
    connectAIOCB& connect_aiocb = static_cast<connectAIOCB&>(event);

    if (lpfnConnectEx == NULL) {
      DWORD dwBytes;
      GUID GuidConnectEx = WSAID_CONNECTEX;
      WSAIoctl(
        connect_aiocb.get_socket(),
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
      = connect_aiocb.get_peername().filter(
          connect_aiocb.get_socket().get_domain()
        );

    if (peername != NULL) {
      PVOID lpSendBuffer;
      DWORD dwSendDataLength;
      if (connect_aiocb.get_send_buffer() != NULL) {
        lpSendBuffer = *connect_aiocb.get_send_buffer();
        dwSendDataLength = connect_aiocb.get_send_buffer()->size();
      } else {
        lpSendBuffer = NULL;
        dwSendDataLength = 0;
      }

      DWORD dwBytesSent;

      return lpfnConnectEx(
               connect_aiocb.get_socket(),
               *peername,
               peername->len(),
               lpSendBuffer,
               dwSendDataLength,
               &dwBytesSent,
               connect_aiocb
             )
             ||
             WSAGetLastError() == WSA_IO_PENDING;
    } else
      return false;
  }
  break;

  case recvAIOCB::TYPE_ID: {
    recvAIOCB& recv_aiocb = static_cast<recvAIOCB&>(event);

    DWORD dwFlags = static_cast<DWORD>(recv_aiocb.get_flags());
    sockaddr* lpFrom = recv_aiocb.get_peername();
    socklen_t lpFromlen = recv_aiocb.get_peername().len();

    if (recv_aiocb.get_buffer().get_next_buffer() == NULL) {
      WSABUF wsabuf;
      wsabuf.buf = static_cast<char*>(recv_aiocb.get_buffer())
                     + recv_aiocb.get_buffer().size();
      wsabuf.len = recv_aiocb.get_buffer().capacity()
                     - recv_aiocb.get_buffer().size();

      return WSARecvFrom(
                recv_aiocb.get_socket(),
                &wsabuf,
                1,
                NULL,
                &dwFlags,
                lpFrom,
                &lpFromlen,
                recv_aiocb,
                NULL
              ) == 0
              ||
              WSAGetLastError() == WSA_IO_PENDING;
    } else { // Scatter I/O
      vector<WSABUF> wsabufs;
      Buffer* next_buffer = &recv_aiocb.get_buffer();
      do {
        WSABUF wsabuf;
        wsabuf.buf = static_cast<char*>(*next_buffer) + next_buffer->size();
        wsabuf.len = next_buffer->capacity() - next_buffer->size();
        wsabufs.push_back(wsabuf);
        next_buffer = next_buffer->get_next_buffer();
      } while (next_buffer != NULL);

      return WSARecvFrom(
                recv_aiocb.get_socket(),
                &wsabufs[0],
                wsabufs.size(),
                NULL,
                &dwFlags,
                lpFrom,
                &lpFromlen,
                recv_aiocb,
                NULL
              ) == 0
              ||
              WSAGetLastError() == WSA_IO_PENDING;;
    }
  }
  break;

  case sendAIOCB::TYPE_ID: {
    sendAIOCB& send_aiocb = static_cast<sendAIOCB&>(event);

    const sockaddr* lpTo = NULL;
    socklen_t iToLen = 0;
    if (send_aiocb.get_peername() != NULL) {
      const SocketAddress* peername =
        send_aiocb.get_peername()->filter(
          send_aiocb.get_socket().get_domain()
        );
      if (peername != NULL) {
        lpTo = *peername;
        iToLen = peername->len();
      } else
        return false;
    }

    if (send_aiocb.get_buffer().get_next_buffer() == NULL) {
      WSABUF wsabuf;
      wsabuf.buf = send_aiocb.get_buffer();
      wsabuf.len = send_aiocb.get_buffer().size();

      return WSASendTo(
                send_aiocb.get_socket(),
                &wsabuf,
                1,
                NULL,
                send_aiocb.get_flags(),
                lpTo,
                iToLen,
                send_aiocb,
                NULL
              ) == 0
              ||
              WSAGetLastError() == WSA_IO_PENDING;
    } else { // Gather I/O
      vector<WSABUF> wsabufs;
      Buffer* next_buffer = &send_aiocb.get_buffer();
      do {
        WSABUF wsabuf;
        wsabuf.buf = static_cast<char*>(*next_buffer);
        wsabuf.len = next_buffer->size();
        wsabufs.push_back(wsabuf);
        next_buffer = next_buffer->get_next_buffer();
      } while (next_buffer != NULL);

      return WSASendTo(
                send_aiocb.get_socket(),
                &wsabufs[0],
                wsabufs.size(),
                NULL,
                send_aiocb.get_flags(),
                lpTo,
                iToLen,
                send_aiocb,
                NULL
              ) == 0
              ||
              WSAGetLastError() == WSA_IO_PENDING;
    }
  }
  break;

  default:
    return yield::aio::win32::AIOQueue::enqueue(event);
  }
}
}
}
}
}
