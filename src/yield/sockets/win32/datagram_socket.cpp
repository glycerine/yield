// yield/sockets/win32/datagram_socket.cpp

// Copyright (c) 2012 Minor Gordon
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

#include "winsock.hpp"
#include "yield/sockets/datagram_socket.hpp"

namespace yield {
namespace sockets {
const int DatagramSocket::TYPE = SOCK_DGRAM;

ssize_t
DatagramSocket::recvfrom(
  void* buf,
  size_t buflen,
  const MessageFlags& flags,
  SocketAddress& peername
) {
  socklen_t peernamelen = peername.len();
  return ::recvfrom(
           *this,
           static_cast<char*>(buf),
           buflen,
           flags,
           static_cast<sockaddr*>(peername),
           &peernamelen
         );
}

ssize_t
DatagramSocket::recvmsg(
  const iovec* iov,
  int iovlen,
  const MessageFlags& flags,
  SocketAddress& peername
) {
  DWORD dwFlags = static_cast<DWORD>(flags);
  DWORD dwNumberOfBytesRecvd;
#ifdef _WIN64
  vector<WSABUF> wsabufs(iovlen);
  for (uint32_t iov_i = 0; iov_i < iovlen; iov_i++) {
    wsabufs[iov_i].buf = static_cast<char*>(iov[iov_i].iov_base);
    wsabufs[iov_i].len = static_cast<ULONG>(iov[iov_i].iov_len);
  }
#endif

  socklen_t peernamelen = peername.len();
  ssize_t recvfrom_ret
  = WSARecvFrom(
      *this,
#ifdef _WIN64
      & wsabufs[0],
#else
      reinterpret_cast<WSABUF*>(const_cast<iovec*>(iov)),
#endif
      iovlen,
      &dwNumberOfBytesRecvd,
      &dwFlags,
      peername,
      &peernamelen,
      NULL,
      NULL
    );

  if (recvfrom_ret == 0) {
    return static_cast<ssize_t>(dwNumberOfBytesRecvd);
  } else {
    return recvfrom_ret;
  }
}

ssize_t
DatagramSocket::sendmsg(
  const iovec* iov,
  int iovlen,
  const MessageFlags& flags,
  const SocketAddress& peername
) {
  DWORD dwNumberOfBytesSent;
#ifdef _WIN64
  vector<iovec64> wsabufs(iovlen);
  for (uint32_t iov_i = 0; iov_i < iovlen; iov_i++) {
    wsabufs[iov_i].buf = static_cast<char*>(iov[iov_i].iov_base);
    wsabufs[iov_i].len = static_cast<ULONG>(iov[iov_i].iov_len);
  }
#endif

  const SocketAddress* peername_ = peername.filter(get_domain());
  if (peername_ != NULL) {
    ssize_t sendto_ret
    = WSASendTo(
        *this,
#ifdef _WIN64
        reinterpret_cast<LPWSABUF>(&wsabufs[0]),
#else
        reinterpret_cast<LPWSABUF>(const_cast<iovec*>(iov)),
#endif
        iovlen,
        &dwNumberOfBytesSent,
        static_cast<DWORD>(flags),
        *peername_,
        peername_->len(),
        NULL,
        NULL
      );

    if (sendto_ret >= 0) {
      return static_cast<ssize_t>(dwNumberOfBytesSent);
    } else {
      return sendto_ret;
    }
  } else {
    return -1;
  }
}

ssize_t
DatagramSocket::sendto(
  const void* buf,
  size_t buflen,
  const MessageFlags& flags,
  const SocketAddress& peername
) {
  const SocketAddress* peername_ = peername.filter(get_domain());
  if (peername_ != NULL) {
    return ::sendto(
             *this,
             static_cast<const char*>(buf),
             buflen,
             flags,
             *peername_,
             peername_->len()
           );
  } else {
    return -1;
  }
}
}
}
