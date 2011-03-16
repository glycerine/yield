// yield/sockets/server/stream_socket_server.hpp

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

#ifndef _YIELD_SOCKETS_SERVER_STREAM_SOCKET_SERVER_HPP_
#define _YIELD_SOCKETS_SERVER_STREAM_SOCKET_SERVER_HPP_

#include "yield/sockets/server/socket_server.hpp"
#include "yield/sockets/peer/stream_socket_peer.hpp"

namespace yield {
namespace sockets {
namespace aio {
class acceptAIOCB;
}

namespace server {
class StreamSocketServer : public yield::sockets::peer::StreamSocketPeer<SocketServer> {
protected:
  class Connection : public yield::sockets::peer::StreamSocketPeer<SocketServer>::Connection {
  public:
    Connection(
      StreamSocketServer&,
      YO_NEW_REF SocketAddress& peername,
      YO_NEW_REF StreamSocket& socket_
    );

    virtual void handle(YO_NEW_REF yield::sockets::aio::acceptAIOCB& accept_aiocb) = 0;
  };

protected:
  StreamSocketServer(
    Log* error_log,
    YO_NEW_REF StreamSocket& socket_,
    const SocketAddress& sockname,
    Log* trace_log
  );

  virtual ~StreamSocketServer();

  virtual Connection&
  create_connection(
    SocketAddress& peername,
    StreamSocket& socket_
  ) = 0;

  void enqueue(YO_NEW_REF yield::sockets::aio::acceptAIOCB& accept_aiocb);

  StreamSocket& get_socket() const {
    return socket_;
  }

  // Stage
  virtual void service(YO_NEW_REF Event& event);

private:
  StreamSocket& socket_;
};
}
}
}

#endif
