// yield/sockets/aio/aiocb.hpp

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

#ifndef _YIELD_SOCKETS_AIO_AIOCB_HPP_
#define _YIELD_SOCKETS_AIO_AIOCB_HPP_

#include "yield/aio/aiocb.hpp"
#include "yield/sockets/socket.hpp"

namespace yield {
class Buffer;

namespace sockets {
class Socket;

namespace aio {
class AIOCB : public yield::aio::AIOCB {
public:
  virtual ~AIOCB() {
    AIOCB::dec_ref(next_aiocb);
  }

public:
  AIOCB* get_next_aiocb() {
    return next_aiocb;
  }

  Socket& get_socket() {
    return static_cast<Socket&>(get_channel());
  }

public:
  void set_next_aiocb(AIOCB* next_aiocb) {
    AIOCB::dec_ref(this->next_aiocb);
    this->next_aiocb = next_aiocb;
  }

public:
  // yield::Object
  AIOCB& inc_ref() {
    return Object::inc_ref(*this);
  }

protected:
  AIOCB(Socket& socket_)
    : yield::aio::AIOCB(socket_, 0) {
    next_aiocb = NULL; 
  }

private:
  AIOCB* next_aiocb;
};
}
}
}

#endif
