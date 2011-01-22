// yield/net/sockets/socket_address.hpp

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


#ifndef _YIELD_NET_SOCKETS_SOCKET_ADDRESS_HPP_
#define _YIELD_NET_SOCKETS_SOCKET_ADDRESS_HPP_


#include "yield/exception.hpp"
#include "yield/object.hpp"
#include "yield/net/uri.hpp"

#include <ostream>
#include <sstream> // for std::ostringstream
#ifndef _WIN32
#include <arpa/inet.h> // for socklen_t
#endif


struct addrinfo;
struct sockaddr;
struct sockaddr_in;
struct sockaddr_in6;
struct in_addr;
struct in6_addr;


#ifdef _WIN32
#pragma warning( push )
#pragma warning( disable: 4521 ) // Multiple copy constructors
#endif


namespace yield {
namespace net {
namespace sockets {
class SocketAddress : public Object {
public:
  const static SocketAddress IN_ANY;
  const static SocketAddress IN_BROADCAST;
  const static SocketAddress IN_LOOPBACK;

public:
  SocketAddress() {
    memset(&addr, 0, sizeof(addr));
    next_socket_address = NULL;
  }

  SocketAddress(const addrinfo& addrinfo_) {
    next_socket_address = NULL;
    assign(addrinfo_);
  }

  SocketAddress(const sockaddr_in& sockaddr_in_) {
    assign(sockaddr_in_);
    next_socket_address = NULL;
  }

  SocketAddress(const sockaddr_in6& sockaddr_in6_) {
    assign(sockaddr_in6_);
    next_socket_address = NULL;
  }

  SocketAddress(const sockaddr& sockaddr_, int family) {
    assign(sockaddr_, family);
    next_socket_address = NULL;
  }

  SocketAddress(const in_addr& in_addr_, uint16_t port) {
    assign(in_addr_, port);
    next_socket_address = NULL;
  }

  SocketAddress(uint32_t in_addr_, uint16_t port) {
    assign(in_addr_, port);
    next_socket_address = NULL;
  }

  SocketAddress(const in6_addr& in6_addr_, uint16_t port) {
    assign(in6_addr_, port);
    next_socket_address = NULL;
  }

  SocketAddress(uint16_t port) {
    init(NULL, port);
  }

  SocketAddress
  (
    const char* nodename,
    const char* servname
  ) throw(Exception) {
    init(nodename, servname);
  }

  SocketAddress(const char* nodename, uint16_t port) throw(Exception) {
    init(nodename, port);
  }

  SocketAddress(const URI& uri) throw(Exception) {
    init(uri.get_host().c_str(), uri.get_port());
  }

  SocketAddress(SocketAddress& other) {
    memcpy_s(&addr, sizeof(addr), &other.addr, sizeof(other.addr));
    next_socket_address = Object::inc_ref(other.next_socket_address);
  }

  SocketAddress(const SocketAddress& other) {
    memcpy_s(&addr, sizeof(addr), &other.addr, sizeof(other.addr));
    if (other.next_socket_address != NULL)
      next_socket_address = new SocketAddress(*other.next_socket_address);
    else
      next_socket_address = NULL;
  }

  SocketAddress(const SocketAddress&, uint16_t port);

  ~SocketAddress() {
    dec_ref(next_socket_address);
  }

  void assign(const addrinfo&);
  void assign(const sockaddr_in&);
  void assign(const sockaddr_in6&);

  void assign(const sockaddr& sockaddr_, int family) {
    memcpy_s(&this->addr, sizeof(this->addr), &sockaddr_, len(family));
    this->addr.ss_family = static_cast<uint16_t>(family);
  }

  void assign(uint32_t in_addr_, uint16_t port);
  void assign(const in_addr&, uint16_t port);
  void assign(const in6_addr&, uint16_t port);

  const SocketAddress* filter(int family) const;

  static YO_NEW_REF SocketAddress*
  getaddrinfo
  (
    const char* nodename,
    const char* servname
  );

  static YO_NEW_REF SocketAddress*
  getaddrinfo
  (
    const char* nodename,
    uint16_t port
  ) {
    std::ostringstream servname;
    servname << port;
    return getaddrinfo(nodename, servname.str().c_str());
  }

  int get_family() const {
    return addr.ss_family;
  }

  bool getnameinfo(OUT string& nodename, bool numeric) const {
    char nameinfo[256];
    if (this->getnameinfo(nameinfo, 256, numeric)) {
      nodename.assign(nameinfo);
      return true;
    } else
      return false;
  }

  bool getnameinfo(OUT char* nodename, size_t, bool numeric) const;

  socklen_t len() const {
    return len(get_family());
  }

  operator string() const {
    std::ostringstream repr;
    repr << *this;
    return repr.str();
  }

  operator sockaddr* () {
    return reinterpret_cast<sockaddr*>(&addr);
  }

  operator const sockaddr* () const {
    return reinterpret_cast<const sockaddr*>(&addr);
  }

  bool operator==(const SocketAddress& other) const {
    if (memcmp(&addr, &other.addr, sizeof(addr)) == 0) {
      if (next_socket_address == NULL)
        return true;
      else if (other.next_socket_address != NULL)
        return *next_socket_address == *other.next_socket_address;
    }

    return false;
  }

  bool operator!=(const SocketAddress& other) const {
    return !operator==(other);
  }

  // Object
  SocketAddress& inc_ref() {
    return Object::inc_ref(*this);
  }

private:
  SocketAddress
  (
    uint32_t in_addr_,
    const struct in6_addr& in6_addr_,
    uint16_t port
  ) {
    assign(in_addr_, port);
    next_socket_address = new SocketAddress(in6_addr_, port);
  }

  static addrinfo* _getaddrinfo(const char*, const char*);

  void init(const char* nodename, uint16_t port) throw(Exception) {
    std::ostringstream servname;
    servname << port;
    init(nodename, servname.str().c_str());
  }

  void
  init
  (
    const char* nodename,
    const char* servname
  ) throw(Exception);

  socklen_t len(int family) const;

private:
  struct {
    union {
      uint16_t ss_family;
      uint64_t u_pad[128 / sizeof(uint64_t)];
    };
  } addr;

  SocketAddress* next_socket_address;
};

static inline
std::ostream&
operator<<
(
  std::ostream& os,
  const SocketAddress& sockaddr_
) {
  string nodename;
  if (sockaddr_.getnameinfo(nodename, true))
    os << nodename;
  else os << "(unknown)";
  return os;
}
}
}
}


#ifdef _WIN32
#pragma warning( pop )
#endif
//


#endif
