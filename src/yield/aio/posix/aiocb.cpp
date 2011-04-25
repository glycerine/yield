// yield/aio/posix/aiocb.cpp

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
//#include "yield/event_handler.hpp"
#include "yield/aio/posix/aiocb.hpp"

namespace yield {
namespace aio {
namespace posix {
AIOCB::AIOCB(fd_t fd, off_t offset) {
  memset(&aiocb_, 0, sizeof(aiocb_));
  aiocb_.aio_fildes = fd;
  aiocb_.aio_offset = offset;
  //aiocb_.aio_sigevent.sigev_notify = SIGEV_THREAD;
  //aiocb_.aio_sigevent.sigev_notify_attributes = NULL;
  //aiocb_.aio_sigevent.sigev_notify_function = notify_function;
  //aiocb_.aio_sigevent.sigev_value.sival_ptr = this;

  completion_handler = NULL;
  error = 0;
  return_ = -1;
}

//AIOCB::~AIOCB() {
//  EventHandler::dec_ref(completion_handler);
//}

//bool AIOCB::cancel() {
//  return aio_cancel(aiocb_.aio_fildes, *this) == AIO_CANCELED;
//}

//void AIOCB::notify_function(sigval_t sigval) {
//  AIOCB& aiocb = *static_cast<AIOCB*>(sigval.sival_ptr);
//  aiocb.set_error(aio_error(aiocb));
//  aiocb.set_return(aio_return(aiocb));
//  aiocb.get_completion_handler()->handle(aiocb);
//}

//void AIOCB::set_completion_handler(EventHandler& completion_handler) {
//  EventHandler::dec_ref(this->completion_handler);
//  this->completion_handler = &completion_handler.inc_ref();
//}
}
}
}
