// yield/net/sockets/socket_pair.hpp

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


#ifndef _YIELD_NET_SOCKETS_SOCKET_PAIR_HPP_
#define _YIELD_NET_SOCKETS_SOCKET_PAIR_HPP_


#include "yield/channel_pair.hpp"
#include "yield/net/sockets/socket.hpp"


namespace yield {
namespace net {
namespace sockets {
class SocketPair : public ChannelPair {
public:
  static int DOMAIN_DEFAULT; // AF_UNIX
  const static int PROTOCOL_DEFAULT = Socket::PROTOCOL_DEFAULT;
  static int TYPE_DEFAULT; // SOCK_STREAM

public:
  SocketPair
  (
    int domain = DOMAIN_DEFAULT,
    int type = TYPE_DEFAULT,
    int protocol = Socket::PROTOCOL_DEFAULT
  );

  ~SocketPair();

  static YO_NEW_REF SocketPair*
  create
  (
    int domain = DOMAIN_DEFAULT,
    int type = TYPE_DEFAULT,
    int protocol = Socket::PROTOCOL_DEFAULT
  );

  Socket& first() const {
    return *sockets[0];
  }
  Socket& second() const {
    return *sockets[1];
  }

  // ChannelPair
  Channel& get_read_channel() {
    return first();
  }
  Channel& get_write_channel() {
    return second();
  }

private:
  SocketPair(YO_NEW_REF Socket**);

  static bool socketpair(int domain, int type, int protocol, Socket**);

private:
  Socket* sockets[2];
};
}
}
}


#endif
