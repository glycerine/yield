// yield/sockets/posix/socket_address.cpp

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
#include "yield/exception.hpp"
#include "yield/sockets/socket_address.hpp"

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>

namespace yield {
namespace sockets {
#pragma GCC diagnostic ignored "-Wold-style-cast"
const int SocketAddress::GETNAMEINFO_FLAG_NOFQDN = NI_NOFQDN;
const int SocketAddress::GETNAMEINFO_FLAG_NAMEREQD = NI_NAMEREQD;
const int SocketAddress::GETNAMEINFO_FLAG_NUMERICHOST = NI_NUMERICHOST;
const int SocketAddress::GETNAMEINFO_FLAG_NUMERICSERV = NI_NUMERICSERV;

const SocketAddress
SocketAddress::IN_ANY(
  static_cast<uint32_t>(INADDR_ANY),
  in6addr_any,
  0
);

const SocketAddress
SocketAddress::IN_BROADCAST(
  static_cast<uint32_t>(INADDR_BROADCAST),
  0
);

#ifdef IN_LOOPBACK
#undef IN_LOOPBACK
#endif
const SocketAddress
SocketAddress::IN_LOOPBACK(
  static_cast<uint32_t>(INADDR_LOOPBACK),
  in6addr_loopback,
  0
);

SocketAddress::SocketAddress(const SocketAddress& other, uint16_t port) {
  memcpy_s(&addr, sizeof(addr), &other.addr, sizeof(other.addr));

  switch (get_family()) {
  case AF_INET: {
    reinterpret_cast<sockaddr_in*>(&addr)->sin_port = htons(port);
  }
  break;

  case AF_INET6: {
    reinterpret_cast<sockaddr_in6*>(&addr)->sin6_port = htons(port);
  }
  break;

  default:
    debug_break();
  }

  if (other.next_socket_address != NULL) {
    next_socket_address = new SocketAddress(*other.next_socket_address, port);
  } else {
    next_socket_address = NULL;
  }
}

void SocketAddress::assign(const addrinfo& addrinfo_) {
  memcpy_s(
    &addr,
    sizeof(addr),
    addrinfo_.ai_addr,
    addrinfo_.ai_addrlen
  );

  addr.ss_family = static_cast<uint16_t>(addrinfo_.ai_family);

  SocketAddress::dec_ref(next_socket_address);
  if (addrinfo_.ai_next != NULL) {
    next_socket_address = new SocketAddress(*addrinfo_.ai_next);
  } else {
    next_socket_address = NULL;
  }
}

void SocketAddress::assign(uint32_t ip, uint16_t port) {
  in_addr in_addr_;
  memset(&in_addr_, 0, sizeof(in_addr_));
  in_addr_.s_addr = htonl(ip);
  assign(in_addr_, port);
}

void SocketAddress::assign(const in_addr& in_addr_, uint16_t port) {
  sockaddr_in sockaddr_in_;
  memset(&sockaddr_in_, 0, sizeof(sockaddr_in_));
  sockaddr_in_.sin_family = AF_INET;
  sockaddr_in_.sin_addr = in_addr_;
  sockaddr_in_.sin_port = port;
  assign(sockaddr_in_);
}

void SocketAddress::assign(const in6_addr& in6_addr_, uint16_t port) {
  sockaddr_in6 sockaddr_in6_;
  memset(&sockaddr_in6_, 0, sizeof(sockaddr_in6_));
  sockaddr_in6_.sin6_family = AF_INET6;
  sockaddr_in6_.sin6_addr = in6_addr_;
  sockaddr_in6_.sin6_port = port;
  assign(sockaddr_in6_);
}

void SocketAddress::assign(const sockaddr_in& sockaddr_in_) {
  assign(reinterpret_cast<const sockaddr&>(sockaddr_in_), AF_INET);
}

void SocketAddress::assign(const sockaddr_in6& sockaddr_in6_) {
  assign(reinterpret_cast<const sockaddr&>(sockaddr_in6_), AF_INET6);
}

const SocketAddress* SocketAddress::filter(int family) const {
  const SocketAddress* next_socket_address = this;

  do {
    if (next_socket_address->get_family() == family) {
      return next_socket_address;
    } else {
      next_socket_address = next_socket_address->next_socket_address;
    }
  } while (next_socket_address != NULL);

  errno = EAFNOSUPPORT;

  return NULL;
}

SocketAddress*
SocketAddress::getaddrinfo(
  const char* nodename,
  const char* servname
) {
  addrinfo* addrinfo_ = _getaddrinfo(nodename, servname);
  if (addrinfo_ != NULL) {
    SocketAddress* socket_address = new SocketAddress(*addrinfo_);
    freeaddrinfo(addrinfo_);
    return socket_address;
  } else {
    return NULL;
  }
}

addrinfo*
SocketAddress::_getaddrinfo(
  const char* nodename,
  const char* servname
) {
#ifdef __sun
  if (nodename == NULL) {
    nodename = "0.0.0.0";
  }
  servname = NULL;
#endif

  addrinfo addrinfo_hints;
  memset(&addrinfo_hints, 0, sizeof(addrinfo_hints));
  addrinfo_hints.ai_family = AF_UNSPEC;
  if (nodename == NULL) {
    addrinfo_hints.ai_flags |= AI_PASSIVE;  // To get INADDR_ANYs
  }

  addrinfo* addrinfo_;

  int getaddrinfo_ret
  = ::getaddrinfo(nodename, servname, &addrinfo_hints, &addrinfo_);

  if (getaddrinfo_ret == 0) {
    return addrinfo_;
  } else {
    return NULL;
  }
}

bool
SocketAddress::getnameinfo(
  char* nodename,
  size_t nodenamelen,
  char* servname,
  size_t servnamelen,
  int flags
) const {
  const SocketAddress* next_socket_address = this;
  do {
    if (
      ::getnameinfo(
        *this,
        len(),
        nodename,
        nodenamelen,
        servname,
        servnamelen,
        flags
      ) == 0
    ) {
      return true;
    }

    next_socket_address = next_socket_address->next_socket_address;
  } while (next_socket_address != NULL);

  return false;
}

void
SocketAddress::init(
  const char* nodename,
  const char* servname
)
throw(Exception) {
  addrinfo* addrinfo_ = _getaddrinfo(nodename, servname);
  if (addrinfo_ != NULL) {
    next_socket_address = NULL;
    assign(*addrinfo_);
    freeaddrinfo(addrinfo_);
  } else {
    throw Exception();
  }
}

socklen_t SocketAddress::len(int family) {
  switch (family) {
  case 0:
    return sizeof(sockaddr_storage);
  case AF_UNIX:
    return sizeof(sockaddr_un);
  case AF_INET:
    return sizeof(sockaddr_in);
  case AF_INET6:
    return sizeof(sockaddr_in6);
  default:
    debug_break();
    return 0;
  }
}
#pragma GCC diagnostic warning "-Wold-style-cast"
}
}
