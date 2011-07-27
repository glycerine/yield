// yield/sockets/socket_address.hpp

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

#ifndef _YIELD_SOCKETS_SOCKET_ADDRESS_HPP_
#define _YIELD_SOCKETS_SOCKET_ADDRESS_HPP_

#include "yield/exception.hpp"
#include "yield/object.hpp"
#include "yield/uri/uri.hpp"

#ifndef _WIN32
#include <arpa/inet.h> // for socklen_t
#endif
#include <ostream>
#include <stdlib.h> // for atoi
#include <sstream> // for std::ostringstream
#ifndef _WIN32
#include <sys/socket.h> // for sockaddr_storage
#endif

struct addrinfo;
struct sockaddr;
struct sockaddr_in;
struct sockaddr_in6;
struct in_addr;
struct in6_addr;

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4521) // Multiple copy constructors
#endif

namespace yield {
namespace sockets {
/**
  The address of a socket, such as an IP-port pair.
  Roughly equivalent to a sockaddr_storage.
*/
class SocketAddress : public Object {
public:
  /**
    getnameinfo flag: return the unqualified host name for an address instead
      of its fully-qualified domain name (FQDN).
  */
  const static int GETNAMEINFO_FLAG_NOFQDN;

  /**
    getnameinfo flag: always return a host name for an address, never a numeric
      representation of the address.
  */
  const static int GETNAMEINFO_FLAG_NAMEREQD;

  /**
    getnameinfo flag: return a numeric representation of an address rather than
      a host name or fully-qualified domain name.
  */
  const static int GETNAMEINFO_FLAG_NUMERICHOST;

  /**
    getnameinfo flag: return a numeric representation of a service e.g., a
      a port rather than a service name.
  */
  const static int GETNAMEINFO_FLAG_NUMERICSERV;

public:
  /**
    Special socket addresses: the IPv4 address to bind to all available
      addresses.
  */
  const static SocketAddress IN_ANY;

  /**
    Special socket addresses: the IP broadcast address.
  */
  const static SocketAddress IN_BROADCAST;

  /**
    Special socket addresses: the IP loopback address.
  */
  const static SocketAddress IN_LOOPBACK;

public:
  /**
    Construct an empty SocketAddress, suitable for using as an out parameter
      to Socket methods such as getsockname or getpeername.
  */
  SocketAddress() {
    memset(&addr, 0, sizeof(addr));
    next_socket_address = NULL;
  }

  /**
    Construct a SocketAddress from a struct addrinfo linked list, usually
      returned by getaddrinfo.
    @param addrinfo_ a linked list of <code>struct addrinfo</code>s
  */
  SocketAddress(const addrinfo& addrinfo_) {
    next_socket_address = NULL;
    assign(addrinfo_);
  }

  /**
    Construct a SocketAddress from an IPv4 struct sockaddr_in.
    @param sockaddr_in_ an IPv4 struct sockaddr_in
  */
  SocketAddress(const sockaddr_in& sockaddr_in_) {
    assign(sockaddr_in_);
    next_socket_address = NULL;
  }

  /**
    Construct a SocketAddress from an IPv6 struct sockaddr_in6.
    @param sockaddr_in6_ an IPv6 struct sockaddr_in6_
  */
  SocketAddress(const sockaddr_in6& sockaddr_in6_) {
    assign(sockaddr_in6_);
    next_socket_address = NULL;
  }

  /**
    Construct a SocketAddress from a generic struct sockaddr, first
      downcasting it to the specified family.
    @param sockaddr_ a generic struct sockaddr
    @param family the address family (e.g., AF_INET) of the struct sockaddr
  */
  SocketAddress(const sockaddr& sockaddr_, int family) {
    assign(sockaddr_, family);
    next_socket_address = NULL;
  }

  /**
    Construct a SocketAddress from an IPv4 IP and port.
    @param in_addr_ an IPv4 IP
    @param port port
  */
  SocketAddress(const in_addr& in_addr_, uint16_t port) {
    assign(in_addr_, port);
    next_socket_address = NULL;
  }

  /**
    Construct a SocketAddress from an IPv4 IP and port.
    @param in_addr_ an IPv4 IP
    @param port port
  */
  SocketAddress(uint32_t in_addr_, uint16_t port) {
    assign(in_addr_, port);
    next_socket_address = NULL;
  }

  /**
    Construct a SocketAddress from an IPv6 IP and port.
    @param in6_addr_ an IPv6 IP
    @param port port
  */
  SocketAddress(const in6_addr& in6_addr_, uint16_t port) {
    assign(in6_addr_, port);
    next_socket_address = NULL;
  }

  /**
    Construct a SocketAddress from IN_ANY and the given port.
    @param port port
  */
  SocketAddress(uint16_t port) {
    init(NULL, port);
  }

  /**
    Construct a SocketAddress from the given node name and service name,
      first resolving both (node name to address, service name to port).
    @param nodename node (host) name to resolve
    @param servname service name to resolve
  */
  SocketAddress(const char* nodename, const char* servname) throw(Exception) {
    init(nodename, servname);
  }

  /**
    Construct a SocketAddress from the given node name and port,
      first resolving the node name to an address.
    @param nodename node (host) name to resolve
    @param port port
  */
  SocketAddress(const char* nodename, uint16_t port) throw(Exception) {
    init(nodename, port);
  }

  /**
    Copy constructor with minimal allocations.
    @param other SocketAddress to copy
  */
  SocketAddress(SocketAddress& other) {
    memcpy_s(&addr, sizeof(addr), &other.addr, sizeof(other.addr));
    next_socket_address = Object::inc_ref(other.next_socket_address);
  }

  /**
    Copy constructor that may do extra allocations.
    @param other SocketAddress to copy
  */
  SocketAddress(const SocketAddress& other) {
    memcpy_s(&addr, sizeof(addr), &other.addr, sizeof(other.addr));
    if (other.next_socket_address != NULL) {
      next_socket_address = new SocketAddress(*other.next_socket_address);
    } else {
      next_socket_address = NULL;
    }
  }

  /**
    Copy constructor that overwrites port in the copy.
    @param other SocketAddress to copy
    @param port port of the new copy
  */
  SocketAddress(const SocketAddress& other, uint16_t port);

  /**
    Destructor.
  */
  ~SocketAddress() {
    dec_ref(next_socket_address);
  }

public:
  /**
    Replace the contents of this SocketAddress with those of a struct addrinfo
      linked list, usually returned by getaddrinfo.
    @param addrinfo_ a linked list of <code>struct addrinfo</code>s
  */
  void assign(const addrinfo& addrinfo_);

  /**
    Replace the contents of this SocketAddress with those of an IPv4
      struct sockaddr_in.
    @param sockaddr_in_ an IPv4 struct sockaddr_in
  */
  void assign(const sockaddr_in& sockaddr_in_);

  /**
    Replace the contents of this SocketAddress with those of an an IPv6
      struct sockaddr_in6.
    @param sockaddr_in6_ an IPv6 struct sockaddr_in6_
  */
  void assign(const sockaddr_in6& sockaddr_in6_);

  /**
    Replace the contents of this SocketAddress with those of a generic
      struct sockaddr, first downcasting it to the specified family.
    @param sockaddr_ a generic struct sockaddr
    @param family the address family (e.g., AF_INET) of the struct sockaddr
  */
  void assign(const sockaddr& sockaddr_, int family) {
    memcpy_s(&this->addr, sizeof(this->addr), &sockaddr_, len(family));
    this->addr.ss_family = static_cast<uint16_t>(family);
  }

  /**
    Replace the contents of this SocketAddress with the given IPv4 IP and port.
    @param in_addr_ an IPv4 IP
    @param port port
  */
  void assign(const in_addr& in_addr_, uint16_t port);

  /**
    Replace the contents of this SocketAddress with the given IPv4 IP and port.
    @param in_addr_ an IPv4 IP
    @param port port
  */
  void assign(uint32_t in_addr_, uint16_t port);

  /**
    Replace the contents of this SocketAddress with the given IPv6 IP and port.
    @param in6_addr_ an IPv4 IP
    @param port port
  */
  void assign(const in6_addr& in6_addr_, uint16_t port);

public:
  /**
    Filter the SocketAddress to only cover one address family (e.g, AF_INET).
    @param family the address family of the desired SocketAddress
    @return a SocketAddress with the desired address family on success,
      NULL+errno on failure
  */
  const SocketAddress* filter(int family) const;

public:
  /**
    Translate a node and service name pair into a SocketAddress.
    @param nodename node (host) name to resolve into an address
    @param servname service name to resolve into a port
    @return the resolved SocketAddress on success, NULL+errno on failure
  */
  static YO_NEW_REF SocketAddress*
  getaddrinfo(
    const char* nodename,
    const char* servname
  );

  /**
    Translate a node name into a SocketAddress.
    @param nodename node (host) name to resolve into an address
    @param port port of the new SocketAddress
    @return the resolved SocketAddress on success, NULL+errno on failure
  */
  static YO_NEW_REF SocketAddress*
  getaddrinfo(
    const char* nodename,
    uint16_t port
  ) {
    std::ostringstream servname;
    servname << port;
    return getaddrinfo(nodename, servname.str().c_str());
  }

public:
  /**
    Get the family of this SocketAddress e.g., AF_INET.
    This is usually the same as a Socket's domain.
  */
  int get_family() const {
    return addr.ss_family;
  }

public:
  /**
    Translate this SocketAddress into a node (host) and service name pair,
      performing a reverse lookup if necessary.
    @param[out] nodename the translated node (host) name
    @param[out] servname the translated service name or port,
      or NULL if this is not required
    @param flags flags controlling the output of the translation process;
      see the GETNAMEINFO_FLAG constants
  */
  bool getnameinfo(
    string& nodename,
    uint16_t* servname = NULL,
    int flags = GETNAMEINFO_FLAG_NUMERICHOST | GETNAMEINFO_FLAG_NUMERICSERV
  ) const {
    char nodename_[256], servname_[16];
    if (
      getnameinfo(
        nodename_,
        256,
        servname_,
        16,
        flags | GETNAMEINFO_FLAG_NUMERICSERV
      )
    ) {
      nodename.assign(nodename_);
      if (servname != NULL) {
        *servname = static_cast<uint16_t>(atoi(servname_));
      }
      return true;
    } else {
      return false;
    }
  }

  /**
    Translate this SocketAddress into a node (host) and service name pair,
      performing a reverse lookup if necessary.
    @param[out] nodename the translated node (host) name
    @param nodenamelen length of nodename, in bytes
    @param[out] servname the translated service name or port,
      or NULL if this is not required
    @param servnamelen length of servname, in bytes
    @param flags flags controlling the output of the translation process;
      see the GETNAMEINFO_FLAG constants
  */
  bool
  getnameinfo(
    char* nodename,
    size_t nodenamelen,
    char* servname,
    size_t servnamelen,
    int flags = GETNAMEINFO_FLAG_NUMERICHOST | GETNAMEINFO_FLAG_NUMERICSERV
  ) const;

public:
  /**
    Get the length in bytes of the underlying platform-specific socket address.
    @return the length in bytes of the underlying platform-specific socket address
  */
  socklen_t len() const {
    return len(get_family());
  }

  /**
    Get the length in bytes of the platform-specific socket address
      corresponding to the given family.
    @param family a socket address family such as AF_INET
    @return the length in bytes of the platform-specific socket address
      corresponding to the given family
  */
  static socklen_t len(int family);

public:
  /**
    Cast to a human-readable string representation.
    @return a human-rceadable string representation
  */
  operator string() const {
    std::ostringstream repr;
    repr << *this;
    return repr.str();
  }

  /**
    Get the underlying platform-specific socket address.
    @return the underlying platform-specific socket address
  */
  operator sockaddr* () {
    return reinterpret_cast<sockaddr*>(&addr);
  }

  /**
    Get the underlying platform-specific socket address.
    @return the underlying platform-specific socket address
  */
  operator const sockaddr* () const {
    return reinterpret_cast<const sockaddr*>(&addr);
  }

  /**
    Compare two SocketAddress objects for equality.
    @param other SocketAddress object to compare this one to
    @return true if the two SocketAddress objects are equal
  */
  bool operator==(const SocketAddress& other) const {
    if (memcmp(&addr, &other.addr, sizeof(addr)) == 0) {
      if (next_socket_address == NULL) {
        return true;
      } else if (other.next_socket_address != NULL) {
        return *next_socket_address == *other.next_socket_address;
      }
    }

    return false;
  }

  /**
    Compare two SocketAddress objects for inequality.
    @param other SocketAddress object to compare this one to
    @return true if the two SocketAddress objects are not equal
  */
  bool operator!=(const SocketAddress& other) const {
    return !operator==(other);
  }

public:
  // yield::Object
  SocketAddress& inc_ref() {
    return Object::inc_ref(*this);
  }

private:
  SocketAddress(
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
  init(
    const char* nodename,
    const char* servname
  ) throw(Exception);

private:
#ifdef _WIN32
  struct {
    union {
      uint16_t ss_family;
      uint64_t u_pad[128 / sizeof(uint64_t)];
    };
  } addr;
#else
  sockaddr_storage addr;
#endif

  SocketAddress* next_socket_address;
};

static inline
std::ostream&
operator<<(
  std::ostream& os,
  const SocketAddress& sockaddr_
) {
  if (sockaddr_.get_family() != 0) {
    string nodename;
    uint16_t servname;
    if (sockaddr_.getnameinfo(nodename, &servname)) {
      os << nodename << ":" << servname;
    }
  } else {
    os << "(unknown)";
  }
  return os;
}

static inline
std::ostream&
operator<<(
  std::ostream& os,
  SocketAddress* socket_address
) {
  if (socket_address != NULL) {
    return operator<<(os, *socket_address);
  } else {
    os << "NULL";
    return os;
  }
}
}
}

#ifdef _WIN32
#pragma warning(pop)
#endif
//

#endif
