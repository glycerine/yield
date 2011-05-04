// yield/sockets/socket_pair.cpp

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

#include "yield/exception.hpp"
#include "yield/sockets/datagram_socket.hpp"
#include "yield/sockets/socket_address.hpp"
#include "yield/sockets/socket_pair.hpp"
#include "yield/sockets/stream_socket.hpp"
#include "yield/sockets/tcp_socket.hpp"

#ifndef _WIN32
#include <sys/socket.h>
#endif

namespace yield {
namespace sockets {
#ifdef _WIN32
const int SocketPair::DOMAIN_DEFAULT = Socket::DOMAIN_DEFAULT;
#else
const int SocketPair::DOMAIN_DEFAULT = AF_UNIX;
#endif
const int SocketPair::TYPE_DEFAULT = StreamSocket::TYPE;


SocketPair::SocketPair(int domain, int type, int protocol) {
  if (!socketpair(domain, type, protocol, sockets))
    throw Exception();
}

SocketPair::~SocketPair() {
  Socket::dec_ref(*sockets[0]);
  Socket::dec_ref(*sockets[1]);
}

bool
SocketPair::socketpair(
  int domain,
  int type,
  int protocol,
  Socket** sockets
) {
#ifndef _WIN32
  socket_t sv[2];
  if (::socketpair(domain, type, protocol, sv) != -1) {
    sockets[0] = new Socket(domain, type, protocol, sv[0]);
    sockets[1] = new Socket(domain, type, protocol, sv[1]);
    return true;
  } else if (domain == AF_UNIX)
    return false;
#endif

  sockets[0] = sockets[1] = NULL;

  if (type == StreamSocket::TYPE) {
    StreamSocket* listen_stream_socket
    = StreamSocket::create(domain, protocol);

    if (
      listen_stream_socket != NULL
      &&
      listen_stream_socket->bind(SocketAddress::IN_LOOPBACK)
      &&
      listen_stream_socket->listen()
    ) {
      SocketAddress sockname;
      if (listen_stream_socket->getsockname(sockname)) {
        sockets[0] = StreamSocket::create(domain, protocol);

        if (sockets[0] != NULL) {
          if (sockets[0]->connect(sockname)) {
            sockets[1] = listen_stream_socket->accept();
            if (sockets[1] != NULL) {
              StreamSocket::dec_ref(*listen_stream_socket);
              return true;
            }
          } else
            StreamSocket::dec_ref(*sockets[0]);
        }
      }
    } else
      StreamSocket::dec_ref(listen_stream_socket);
  } else if (type == DatagramSocket::TYPE) {
    SocketAddress socknames[2];

    for (uint8_t i = 0; i < 2; i++) {
      if (
        (sockets[i] = DatagramSocket::create(domain)) != NULL
        &&
        sockets[i]->bind(SocketAddress::IN_LOOPBACK)
        &&
        sockets[i]->getsockname(socknames[i])
      )
        continue;
      else {
        Socket::dec_ref(sockets[0]);
        Socket::dec_ref(sockets[1]);
        return false;
      }
    }

    if (sockets[0]->connect(socknames[1]) && sockets[1]->connect(socknames[0]))
      return true;
    else {
      for (uint8_t i = 0; i < 2; i++) {
        Socket::dec_ref(sockets[i]);
        SocketAddress::dec_ref(socknames[i]);
      }
    }
  }

  return false;
}
}
}
