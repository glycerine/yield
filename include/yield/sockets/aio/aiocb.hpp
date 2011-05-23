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

#include "yield/event.hpp"

#ifdef _WIN32
struct _OVERLAPPED;
typedef struct _OVERLAPPED OVERLAPPED;
#endif

namespace yield {
namespace sockets {
class Socket;

namespace aio {
class AIOCB : public Event {
public:
  virtual ~AIOCB();

public:
#ifdef _WIN32
  static AIOCB& cast(::OVERLAPPED& lpOverlapped);
#endif

public:
  uint32_t get_error() const {
    return error;
  }

  ssize_t get_return() const {
    return return_;
  }

  Socket& get_socket() {
    return socket_;
  }

public:
#ifdef _WIN32
  operator ::OVERLAPPED*();
#endif

public:
  void set_error(uint32_t error) {
    this->error = error;
  }

  void set_return(ssize_t return_) {
    this->return_ = return_;
  }

public:
  // yield::Object
  virtual uint32_t get_type_id() const = 0;
  virtual const char* get_type_name() const = 0;

  AIOCB& inc_ref() {
    return Object::inc_ref(*this);
  }

protected:
  AIOCB(Socket& socket_);
  AIOCB(Socket& socket_, off_t offset);

private:
#ifdef _WIN32
  struct {
    unsigned long* Internal;
    unsigned long* InternalHigh;
#pragma warning(push)
#pragma warning(disable: 4201)
    union {
      struct {
        unsigned long Offset;
        unsigned long OffsetHigh;
      };
      void* Pointer;
    };
#pragma warning(pop)
    void* hEvent;
  } overlapped;
  AIOCB* this_;
#endif

  uint32_t error;
  ssize_t return_;
  Socket& socket_;
};
}
}
}

#endif
