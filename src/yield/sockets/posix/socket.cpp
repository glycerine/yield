// yield/sockets/posix/socket.cpp

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

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>

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
  return ::close(*this) != -1;
}

bool Socket::connect(const SocketAddress& _peername) {
  const SocketAddress* peername = _peername.filter(get_domain());
  if (peername != NULL) {
    if (::connect(*this, *peername, peername->len()) != -1)
      return true;
    else
      return errno == EISCONN;
  } else
    return false;
}

socket_t Socket::create(int domain, int type, int protocol) {
  return ::socket(domain, type, protocol);
}

string Socket::getfqdn() {
  char fqdn[256];
  ::gethostname(fqdn, 256);
  char* first_dot = strstr(fqdn, ".");
  if (first_dot != NULL) *first_dot = 0;

  // getnameinfo does not return aliases, which means we get "localhost"
  // on Linux if that's the first
  // entry for 127.0.0.1 in /etc/hosts

#ifndef __sun
  char domainname[256];
  // getdomainname is not a public call on Solaris, apparently
  if (getdomainname(domainname, 256) == 0 &&
      domainname[0] != 0 &&
      strcmp(domainname, "(none)") != 0 &&
      strcmp(domainname, fqdn) != 0 &&
      strstr(domainname, "localdomain") == NULL)
    strcat(fqdn, domainname);
  else {
#endif
    // Try gethostbyaddr, like Python
    uint32_t local_host_addr = inet_addr("127.0.0.1");
    hostent* hostents
    = gethostbyaddr
      (
        reinterpret_cast<char*>(&local_host_addr),
        sizeof(uint32_t),
        AF_INET
      );

    if (hostents != NULL) {
      if
      (
        strchr(hostents->h_name, '.') != NULL &&
        strstr(hostents->h_name, "localhost") == NULL
      ) {
        strncpy(fqdn, hostents->h_name, 256);
      } else {
        for (uint8_t i = 0; hostents->h_aliases[i] != NULL; i++) {
          if
          (
            strchr(hostents->h_aliases[i], '.') != NULL &&
            strstr(hostents->h_name, "localhost") == NULL
          ) {
            strncpy(fqdn, hostents->h_aliases[i], 256);
            break;
          }
        }
      }
    }
#ifndef __sun
  }
#endif
  return fqdn;
}

string Socket::gethostname() {
  char hostname[256];
  ::gethostname(hostname, 256);
  return hostname;
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
  return ::recv(*this, buf, buflen, flags);
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
  msghdr msghdr_;
  memset(&msghdr_, 0, sizeof(msghdr_));

  msghdr_.msg_iov = const_cast<iovec*>(iov);
  msghdr_.msg_iovlen = iovlen;

  if (peername != NULL) {
    msghdr_.msg_name = *peername;
    msghdr_.msg_namelen = sizeof(*peername);
  }

  return ::recvmsg(*this, &msghdr_, flags);
}

ssize_t
Socket::send
(
  const void* buf,
  size_t buflen,
  const MessageFlags& flags
) {
  return ::send(*this, buf, buflen, flags);
}

ssize_t
Socket::sendmsg
(
  const iovec* iov,
  int iovlen,
  const MessageFlags& flags,
  const SocketAddress* _peername
) {
  const SocketAddress* peername = _peername->filter(get_domain());
  if (peername != NULL) {
    msghdr msghdr_;
    memset(&msghdr_, 0, sizeof(msghdr_));

    msghdr_.msg_iov = const_cast<iovec*>(iov);
    msghdr_.msg_iovlen = iovlen;

    if (peername != NULL) {
      const sockaddr* peername_sockaddr = *peername;
      msghdr_.msg_name = const_cast<sockaddr*>(peername_sockaddr);
      msghdr_.msg_namelen = peername->len();
    }

    return ::sendmsg(*this, &msghdr_, flags);
  } else
    return -1;
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
  int current_fcntl_flags = fcntl(*this, F_GETFL, 0);
  if (blocking_mode) {
    if ((current_fcntl_flags & O_NONBLOCK) == O_NONBLOCK)
      return fcntl(*this, F_SETFL, current_fcntl_flags ^ O_NONBLOCK) != -1;
    else
      return true;
  } else
    return fcntl(*this, F_SETFL, current_fcntl_flags | O_NONBLOCK) != -1;
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
  if (shut_rd && shut_wr) how = SHUT_RDWR;
  else if (shut_rd) how = SHUT_RD;
  else if (shut_wr) how = SHUT_WR;
  else return false;

  return ::shutdown(*this, how) != -1;
}

bool Socket::want_recv() const {
  return errno == EWOULDBLOCK;
}

bool Socket::want_send() const {
  return errno == EWOULDBLOCK;
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

#ifdef __linux
  platform_message_flags |= MSG_NOSIGNAL;
#endif
}
}
}
