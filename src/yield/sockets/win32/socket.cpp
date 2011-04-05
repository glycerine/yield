// yield/sockets/win32/socket.cpp

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
#include "yield/sockets/socket.hpp"
#include "yield/sockets/win32/winsock.hpp"

namespace yield {
namespace sockets {
bool Socket::bind(const SocketAddress& _name) {
  const SocketAddress* name = _name.filter(get_domain());
  if (name != NULL)
    return ::bind(*this, *name, name->len()) != -1;
  else
    return false;
}

bool Socket::close() {
  return ::closesocket(*this) != -1;
}

bool Socket::connect(const SocketAddress& _peername) {
  const SocketAddress* peername = _peername.filter(get_domain());
  if (peername != NULL) {
    if (::connect(*this, *peername, peername->len()) != -1)
      return true;
    else
      return WSAGetLastError() == WSAEISCONN;
  } else
    return false;
}

socket_t Socket::create(int domain, int type, int protocol) {
  socket_t socket_ = ::socket(domain, type, protocol);

  if (socket_ == INVALID_SOCKET && WSAGetLastError() == WSANOTINITIALISED) {
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;
    WSAStartup(wVersionRequested, &wsaData);

    socket_ = ::socket(domain, type, protocol);
  }

  if (socket_ != INVALID_SOCKET) {
    if (domain == AF_INET6) {
      DWORD ipv6only = 0; // Allow dual-mode sockets
      ::setsockopt
      (
        socket_,
        IPPROTO_IPV6,
        IPV6_V6ONLY,
        (char*)&ipv6only,
        sizeof(ipv6only)
      );
    }
  } else
    return INVALID_SOCKET;

  return socket_;
}

string Socket::getfqdn() {
  DWORD dwFQDNLength = 0;
  GetComputerNameExA(ComputerNameDnsHostname, NULL, &dwFQDNLength);
  if (dwFQDNLength > 0) {
    char* fqdn_temp = new char[dwFQDNLength];
    if
    (
      GetComputerNameExA
      (
        ComputerNameDnsFullyQualified,
        fqdn_temp,
        &dwFQDNLength
      )
    ) {
      string fqdn(fqdn_temp, dwFQDNLength);
      delete [] fqdn_temp;
      return fqdn;
    } else
      delete [] fqdn_temp;
  }

  return string();
}

string Socket::gethostname() {
  DWORD dwHostNameLength = 0;
  GetComputerNameExA(ComputerNameDnsHostname, NULL, &dwHostNameLength);
  if (dwHostNameLength > 0) {
    char* hostname_temp = new char[dwHostNameLength];
    if
    (
      GetComputerNameExA
      (
        ComputerNameDnsHostname,
        hostname_temp,
        &dwHostNameLength
      )
    ) {
      string hostname(hostname_temp, dwHostNameLength);
      delete [] hostname_temp;
      return hostname;
    } else
      delete [] hostname_temp;
  }

  return string();
}

bool Socket::getpeername(OUT SocketAddress& peername) const {
  socklen_t peernamelen = peername.len();
  if
  (
    ::getpeername
    (
      *this,
      peername,
      &peernamelen
    ) != -1
  ) {
    debug_assert_eq(peername.get_family(), get_domain());
    return true;
  } else
    return false;
}

bool Socket::getsockname(OUT SocketAddress& sockname) const {
  socklen_t socknamelen = sockname.len();

  if
  (
    ::getsockname
    (
      *this,
      sockname,
      &socknamelen
    ) != -1
  ) {
    debug_assert_eq(sockname.get_family(), get_domain());
    return true;
  } else
    return false;
}

ssize_t Socket::recv(void* buf, size_t buflen, const MessageFlags& flags) {
  return ::recv
         (
           *this,
           static_cast<char*>(buf),
           static_cast<int>(buflen),
           flags
         ); // No real advantage to WSARecv on Win for one buffer
}

ssize_t
Socket::recvfrom
(
  void* buf,
  size_t buflen,
  const MessageFlags& flags,
  SocketAddress& peername
) {
  socklen_t namelen = peername.len();
  return ::recvfrom
         (
           *this,
           static_cast<char*>(buf),
           buflen,
           flags,
           reinterpret_cast<sockaddr*>(&peername),
           &namelen
         );
}

ssize_t
Socket::recvmsg
(
  const iovec* iov,
  int iovlen,
  const MessageFlags& flags,
  SocketAddress* peername
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

  ssize_t recv_ret;


  if (peername != NULL) {
    socklen_t peernamelen = peername->len();

    recv_ret
    = WSARecvFrom
      (
        *this,
#ifdef _WIN64
        & wsabufs[0],
#else
        reinterpret_cast<WSABUF*>(const_cast<iovec*>(iov)),
#endif
        iovlen,
        &dwNumberOfBytesRecvd,
        &dwFlags,
        *peername,
        &peernamelen,
        NULL,
        NULL
      );
  } else {
    recv_ret
    = WSARecv
      (
        *this,
#ifdef _WIN64
        & wsabufs[0],
#else
        reinterpret_cast<WSABUF*>(const_cast<iovec*>(iov)),
#endif
        iovlen,
        &dwNumberOfBytesRecvd,
        &dwFlags,
        NULL,
        NULL
      );
  }

  if (recv_ret > 0)
    return static_cast<ssize_t>(dwNumberOfBytesRecvd);
  else
    return recv_ret;
}

ssize_t
Socket::send
(
  const void* buf,
  size_t buflen,
  const MessageFlags& flags
) {
  DWORD dwNumberOfBytesSent;
  WSABUF wsabuf;
  wsabuf.len = static_cast<ULONG>(buflen);
  wsabuf.buf = const_cast<char*>(static_cast<const char*>(buf));

  ssize_t send_ret
  = WSASend
    (
      *this,
      &wsabuf,
      1,
      &dwNumberOfBytesSent,
      static_cast<DWORD>(flags),
      NULL,
      NULL
    );

  if (send_ret >= 0)
    return static_cast<ssize_t>(dwNumberOfBytesSent);
  else
    return send_ret;
}

ssize_t
Socket::sendmsg
(
  const iovec* iov,
  int iovlen,
  const MessageFlags& flags,
  const SocketAddress* peername
) {
  DWORD dwNumberOfBytesSent;
#ifdef _WIN64
  vector<iovec64> wsabufs(iovlen);
  for (uint32_t iov_i = 0; iov_i < iovlen; iov_i++) {
    wsabufs[iov_i].buf = static_cast<char*>(iov[iov_i].iov_base);
    wsabufs[iov_i].len = static_cast<ULONG>(iov[iov_i].iov_len);
  }
#endif
  ssize_t send_ret;

  if (peername != NULL) {
    peername = peername->filter(get_domain());
    if (peername != NULL) {
      send_ret
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
          *peername,
          peername->len(),
          NULL,
          NULL
        );
    } else
      return -1;
  } else {
    send_ret
    = WSASend(
        *this,
#ifdef _WIN64
        & wsabufs[0],
#else
        reinterpret_cast<WSABUF*>(const_cast<iovec*>(iov)),
#endif
        iovlen,
        &dwNumberOfBytesSent,
        static_cast<DWORD>(flags),
        NULL,
        NULL
      );
  }

  if (send_ret >= 0)
    return static_cast<ssize_t>(dwNumberOfBytesSent);
  else
    return send_ret;
}

ssize_t
Socket::sendto
(
  const void* buf,
  size_t buflen,
  const MessageFlags& flags,
  const SocketAddress& _peername
) {
  const SocketAddress* peername = _peername.filter(get_domain());
  if (peername != NULL) {
    return ::sendto
           (
             *this,
             static_cast<const char*>(buf),
             buflen,
             flags,
             *peername,
             peername->len()
           );
  } else
    return -1;
}

bool Socket::set_blocking_mode(bool blocking_mode) {
  unsigned long val = blocking_mode ? 0UL : 1UL;
  return ::ioctlsocket(*this, FIONBIO, &val) != SOCKET_ERROR;
}

bool Socket::setsockopt(Option option, bool onoff) {
  switch (option) {
  case OPTION_SO_KEEPALIVE: {
    int optval = onoff ? 1 : 0;
    return ::setsockopt
           (
             *this,
             SOL_SOCKET,
             SO_KEEPALIVE,
             reinterpret_cast<char*>(&optval),
             static_cast<int>(sizeof(optval))
           ) == 0;
  }
  break;

  case OPTION_SO_LINGER: {
    linger optval;
    optval.l_onoff = onoff ? 1 : 0;
    optval.l_linger = 0;
    return ::setsockopt
           (
             *this,
             SOL_SOCKET,
             SO_LINGER,
             reinterpret_cast<char*>(&optval),
             static_cast<int>(sizeof(optval))
           ) == 0;
  }
  break;

  default:
    return false;
    break;
  }
}

bool Socket::shutdown(bool shut_rd, bool shut_wr) {
  int how;
  if (shut_rd && shut_wr) how = SD_BOTH;
  else if (shut_rd) how = SD_RECEIVE;
  else if (shut_wr) how = SD_SEND;
  else return false;

  return ::shutdown(*this, how) == 0;
}

bool Socket::want_recv() const {
  return WSAGetLastError() == WSAEWOULDBLOCK;
}

bool Socket::want_send() const {
  return WSAGetLastError() == WSAEWOULDBLOCK;
}


Socket::MessageFlags::MessageFlags(int message_flags) {
  platform_message_flags = 0;

  if ((message_flags & DONTROUTE) == DONTROUTE) {
    platform_message_flags |= MSG_DONTROUTE;
    message_flags ^= DONTROUTE;
  }

  if ((message_flags & OOB) == OOB) {
    platform_message_flags |= MSG_OOB;
    message_flags ^= OOB;
  }

  if ((message_flags & PEEK) == PEEK) {
    platform_message_flags |= MSG_PEEK;
    message_flags ^= PEEK;
  }

  platform_message_flags |= message_flags;
}
}
}
