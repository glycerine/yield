// yield/sockets/win32/stream_socket_pair.cpp

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
#include "yield/sockets/stream_socket_pair.hpp"

namespace yield {
namespace sockets {
StreamSocketPair::StreamSocketPair() {
  StreamSocket listen_stream_socket(AF_INET);
  if (listen_stream_socket.bind(SocketAddress::IN_LOOPBACK)) {
    if (listen_stream_socket.listen()) {
      try {
        sockets[0] = new StreamSocket(AF_INET);
        if (sockets[0]->connect(*listen_stream_socket.getsockname())) {
          sockets[1] = listen_stream_socket.accept();
          if (sockets[1] == NULL) {
            throw Exception();
          }
        } else {
          throw Exception();
        }
      } catch (Exception&) {
        StreamSocket::dec_ref(sockets[0]);
        throw;
      }
    } else {
      throw Exception();
    }
  } else {
    throw Exception();
  }
}
}
}
