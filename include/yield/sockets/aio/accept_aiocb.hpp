// yield/sockets/aio/accept_aiocb.hpp

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

#ifndef _YIELD_SOCKETS_AIO_ACCEPT_AIOCB_HPP_
#define _YIELD_SOCKETS_AIO_ACCEPT_AIOCB_HPP_

#include "yield/sockets/aio/aiocb.hpp"

namespace yield {
class Buffer;

namespace sockets {
class Socket;
class SocketAddress;
class StreamSocket;

namespace aio {
/**
  AIO control block for accept operations on sockets.
*/
class acceptAIOCB : public AIOCB {
public:
  const static uint32_t TYPE_ID = 3895043741UL;

public:
  /**
    Construct an acceptAIOCB with an optional buffer for receiving data
      after a new connection is accepted.
    @param socket_ listen socket to accept on
    @param context optional context object
    @param recv_buffer optional buffer for receiving data after a new
      connection is accepted
  */
  acceptAIOCB(
    StreamSocket& socket_,
    Object* context = NULL,
    YO_NEW_REF Buffer* recv_buffer = NULL
  );

  ~acceptAIOCB();

public:
  /**
    Get the accepted socket, once the accept operation is complete.
    @return the accepted socket
  */
  StreamSocket* get_accepted_socket() {
    return accepted_socket;
  }

  /**
    Get the address of the peer of the accepted connection, once the accept
      operation is complete.
    @return the new peer's address
  */
  SocketAddress* get_peername() {
    return peername;
  }

  /**
    Get the buffer with data received from the new peer.
    The buffer is updated after the accept operation if it is specified.
    @return the buffer with data received from the new peer, or NULL
      if no buffer was passed to the constructor
  */
  Buffer* get_recv_buffer() {
    return recv_buffer;
  }

  /**
    Get the listen socket in this accept operation.
  */
  StreamSocket& get_socket();

public:
  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }

  const char* get_type_name() const {
    return "yield::sockets::aio::acceptAIOCB";
  }

protected:
#ifdef _WIN32
  friend class AIOQueue;
#endif
  friend class NBIOQueue;

  void set_accepted_socket(YO_NEW_REF StreamSocket& accepted_socket);
  void set_peername(YO_NEW_REF SocketAddress* peername);
  void set_recv_buffer(YO_NEW_REF Buffer* recv_buffer);

private:
  StreamSocket* accepted_socket;
  SocketAddress* peername;
  Buffer* recv_buffer;
};

/**
  Print a string representation of an acceptAIOCB to a std::ostream.
  @param os std::ostream to print to
  @param accept_aiocb acceptAIOCB to print
  @return os
*/
std::ostream& operator<<(std::ostream& os, acceptAIOCB& accept_aiocb);
}
}
}

#endif
