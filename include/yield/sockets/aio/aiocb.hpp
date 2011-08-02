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
#ifdef _WIN32
class AIOQueue;
#endif
class NBIOQueue;

/**
  Asynchronous Input/Output Control Block (AIOCB) for sockets.
*/
class AIOCB : public Event {
public:
  virtual ~AIOCB();

public:
#ifdef _WIN32
  /**
    Win32 only: cast an LPOVERLAPPED returned by
      <code>GetQueuedCompletionStatus[Ex]</code> to an AIOCB.
    @param lpOverlapped the LPOVERLAPPED returned by
      <code>GetQueuedCompletionStatus[Ex]</code>
    @return the AIOCB that wraps lpOverlapped
  */
  static AIOCB& cast(::OVERLAPPED& lpOverlapped);
#endif

public:
  /**
    Get the context object.
    @return the context object, or NULL if it was not set in the constructor.
  */
  Object* get_context() {
    return context;
  }

  /**
    Get the error code of the AIO operation associated with this control block,
      once the operation is complete.
    Callers should first check that get_return() < 0.
    Similar to POSIX aio_error.
    @return the error code produced by the operation associated with this
      control block
  */
  uint32_t get_error() const {
    return error;
  }

  /**
    Get the return value of the AIO operation associated with this control block,
      once the operation is complete.
    Similar to POSIX aio_return.
    @return the return value produced by the operation associated with this
      control block
  */
  ssize_t get_return() const {
    return return_;
  }

  /**
    Get the socket associated with this control block.
  */
  Socket& get_socket() {
    return socket_;
  }

public:
#ifdef _WIN32
  /**
    Win32 only: cast to get a pointer to the underlying OVERLAPPED structure.
    @return a pointer to the underlying OVERLAPPED structure
  */
  operator ::OVERLAPPED* ();
#endif

public:
  // yield::Object
  virtual uint32_t get_type_id() const = 0;
  virtual const char* get_type_name() const = 0;

  AIOCB& inc_ref() {
    return Object::inc_ref(*this);
  }

protected:
  AIOCB(Socket& socket_, Object* context = NULL);
  AIOCB(Socket& socket_, off_t offset, Object* context = NULL);

protected:
#ifdef _WIN32
  friend class AIOQueue;
#endif
  friend class NBIOQueue;

  void set_error(uint32_t error) {
    this->error = error;
  }

  void set_return(ssize_t return_) {
    this->return_ = return_;
  }

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

  Object* context;
  uint32_t error;
  ssize_t return_;
  Socket& socket_;
};
}
}
}

#endif
