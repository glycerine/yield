// yield/fs/aio/posix/aio_queue.cpp

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
#include "yield/fs/aio/fdatasync_aiocb.hpp"
#include "yield/fs/aio/fsync_aiocb.hpp"
#include "yield/fs/aio/pread_aiocb.hpp"
#include "yield/fs/aio/pwrite_aiocb.hpp"
#include "yield/fs/aio/setlk_aiocb.hpp"
#include "yield/fs/aio/unlk_aiocb.hpp"
#include "yield/fs/aio/posix/aio_queue.hpp"
#include "yield/thread/synchronized_event_queue.hpp"

#include <errno.h>

namespace yield {
namespace fs {
namespace aio {
namespace posix {
using yield::fs::aio::AIOCB;
using yield::thread::SynchronizedEventQueue;

AIOQueue::AIOQueue() {
  completed_event_queue = new SynchronizedEventQueue;
}

AIOQueue::~AIOQueue() {
  SynchronizedEventQueue::dec_ref(*completed_event_queue);
}

YO_NEW_REF Event& AIOQueue::dequeue() {
  return completed_event_queue->dequeue();
}

bool AIOQueue::enqueue(YO_NEW_REF Event& event) {
  switch (event.get_type_id()) {
  case fdatasyncAIOCB::TYPE_ID: {
    fdatasyncAIOCB& fdatasync_aiocb = static_cast<fdatasyncAIOCB&>(event);
    fdatasync_aiocb.set_completion_handler(*completed_event_queue);
    return aio_fsync(O_DSYNC, fdatasync_aiocb) == 0;
  }
  break;

  case fsyncAIOCB::TYPE_ID: {
    fsyncAIOCB& fsync_aiocb = static_cast<fsyncAIOCB&>(event);
    fsync_aiocb.set_completion_handler(*completed_event_queue);
    return aio_fsync(O_SYNC, fsync_aiocb) == 0;
  }
  break;

  case preadAIOCB::TYPE_ID: {
    preadAIOCB& pread_aiocb = static_cast<preadAIOCB&>(event);
    if (pread_aiocb.get_buffer().get_next_buffer() == NULL) {
      pread_aiocb.set_completion_handler(*completed_event_queue);
      aiocb* aiocb_ = pread_aiocb;
      aiocb_->aio_nbytes
        = pread_aiocb.get_buffer().capacity() - pread_aiocb.get_buffer().size();
      return aio_read(aiocb_) == 0;
    } else {
      pread_aiocb.set_error(ENOTSUP);
      return completed_event_queue->enqueue(pread_aiocb);
    }
  }
  break;

  case pwriteAIOCB::TYPE_ID: {
    pwriteAIOCB& pwrite_aiocb = static_cast<pwriteAIOCB&>(event);
    if (pwrite_aiocb.get_buffer().get_next_buffer() == NULL) {
      pwrite_aiocb.set_completion_handler(*completed_event_queue);
      aiocb* aiocb_ = pwrite_aiocb;
      aiocb_->aio_nbytes = pwrite_aiocb.get_buffer().size();
      return aio_write(aiocb_) == 0;
    } else {
      pwrite_aiocb.set_error(ENOTSUP);
      return completed_event_queue->enqueue(pwrite_aiocb);
    }
  }
  break;

  case setlkAIOCB::TYPE_ID:
  case unlkAIOCB::TYPE_ID: DebugBreak(); return false;

  default:
    return completed_event_queue->enqueue(event);
  }
}

YO_NEW_REF Event* AIOQueue::trydequeue() {
  return completed_event_queue->trydequeue();
}
}
}
}
}
