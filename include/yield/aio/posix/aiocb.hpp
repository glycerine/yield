// yield/aio/posix/aiocb.hpp

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

#ifndef _YIELD_AIO_POSIX_AIOCB_HPP_
#define _YIELD_AIO_POSIX_AIOCB_HPP_

#include "yield/event.hpp"

#include <aio.h>

namespace yield {
namespace aio {
namespace posix {
class AIOQueue;

class AIOCB : public Event {
public:
  virtual ~AIOCB();

public:
  uint32_t get_error() const {
    return error;
  }

  uint64_t get_offset() const {
    return aiocb_.aio_offset;
  }

  ssize_t get_return() const {
    return return_;
  }

public:
  operator aiocb* () {
    return &aiocb_;
  }

public:
  void set_error(uint32_t error) {
    this->error = error;
  }

  virtual void set_return(ssize_t return_) {
    this->return_ = return_;
  }

public:
  // yield::Object
  AIOCB& inc_ref() {
    return Object::inc_ref(*this);
  }

protected:
  AIOCB(fd_t, uint64_t offset);

protected:
  static void notify_function(sigval_t sigval);

protected:
  void set_aio_queue(AIOQueue& aio_queue);

private:
  aiocb aiocb_;
  AIOQueue* aio_queue;
  uint32_t error;
  ssize_t return_;
};
}
}
}

#endif
