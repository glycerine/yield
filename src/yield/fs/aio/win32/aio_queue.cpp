// yield/fs/aio/win32/aio_queue.cpp

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

#include "yield/fs/aio/fdatasync_aiocb.hpp"
#include "yield/fs/aio/fsync_aiocb.hpp"
#include "yield/fs/aio/pread_aiocb.hpp"
#include "yield/fs/aio/pwrite_aiocb.hpp"
#include "yield/fs/aio/setlk_aiocb.hpp"
#include "yield/fs/aio/unlk_aiocb.hpp"
#include "yield/fs/aio/win32/aio_queue.hpp"

#include <Windows.h>

namespace yield {
namespace fs {
namespace aio {
namespace win32 {
bool AIOQueue::enqueue(Event& event) {
  switch (event.get_type_id()) {
  case fdatasyncAIOCB::TYPE_ID: {
  case fsyncAIOCB::TYPE_ID: {
    AIOCB& aiocb = static_cast<AIOCB&>(event);
    if (aiocb.get_file().sync()) {
      aiocb.set_error(0);
      return yield::aio::win32::AIOQueue::enqueue(event);
    } else {
      aiocb.set_error(GetLastError());
      return yield::aio::win32::AIOQueue::enqueue(event);
    }
  }
  break;

  case preadAIOCB::TYPE_ID: {
    preadAIOCB& pread_aiocb = static_cast<preadAIOCB&>(event);
    if (pread_aiocb.get_buffer().get_next_buffer() == NULL) {
      return ReadFile(
               pread_aiocb.get_file(),
               pread_aiocb.get_buffer(),
               pread_aiocb.get_buffer().size(),
               NULL,
               pread_aiocb
             ) == TRUE
             ||
             GetLastError() == ERROR_IO_PENDING;
    } else {
      vector<FILE_SEGMENT_ELEMENT> aSegmentArray;
      size_t nNumberOfBytesToRead = 0;
      Buffer* next_buffer = &pread_aiocb.get_buffer();
      do {
        FILE_SEGMENT_ELEMENT file_segment_element;
        file_segment_element.Buffer = *next_buffer;
        aSegmentArray.push_back(file_segment_element);
        nNumberOfBytesToRead += next_buffer->size();
        next_buffer = next_buffer->get_next_buffer();
      } while (next_buffer != NULL);

      return ReadFileScatter(
               pread_aiocb.get_file(),
               &aSegmentArray[0],
               nNumberOfBytesToRead,
               NULL,
               pread_aiocb
             ) == TRUE
             ||
             GetLastError() == ERROR_IO_PENDING;
    }
  }
  break;

  case pwriteAIOCB::TYPE_ID: {
    pwriteAIOCB& pwrite_aiocb = static_cast<pwriteAIOCB&>(event);
    if (pwrite_aiocb.get_buffer().get_next_buffer() == NULL) {
      return WriteFile(
                pwrite_aiocb.get_file(),
                pwrite_aiocb.get_buffer(),
                pwrite_aiocb.get_buffer().size(),
                NULL,
                pwrite_aiocb
              ) == TRUE
              ||
              GetLastError() == ERROR_IO_PENDING;
    } else {
      vector<FILE_SEGMENT_ELEMENT> aSegmentArray;
      DWORD nNumberOfBytesToWrite = 0;
      Buffer* next_buffer = &pwrite_aiocb.get_buffer();
      do {
        FILE_SEGMENT_ELEMENT file_segment_element;
        file_segment_element.Buffer = *next_buffer;
        aSegmentArray.push_back(file_segment_element);
        nNumberOfBytesToWrite += next_buffer->size();
        next_buffer = next_buffer->get_next_buffer();
      } while (next_buffer != NULL);

      return WriteFileGather(
                pwrite_aiocb.get_file(),
                &aSegmentArray[0],
                nNumberOfBytesToWrite,
                NULL,
                pwrite_aiocb
              ) == TRUE
              ||
              GetLastError() == ERROR_IO_PENDING;
    }
  }
  break;

  case setlkAIOCB::TYPE_ID: {
    setlkAIOCB& setlk_aiocb = static_cast<setlkAIOCB&>(event);
    return LockFileEx(
             setlk_aiocb.get_file(),
             setlk_aiocb.get_flock().is_exclusive()
              ? LOCKFILE_EXCLUSIVE_LOCK :
              0,
             0,
             static_cast<DWORD>(setlk_aiocb.get_flock().get_len()),
             static_cast<DWORD>(setlk_aiocb.get_flock().get_len() >> 32),
             setlk_aiocb
           ) == TRUE
           ||
           GetLastError() == ERROR_IO_PENDING;
  }
  break;

  case unlkAIOCB::TYPE_ID: {
    unlkAIOCB& unlk_aiocb = static_cast<unlkAIOCB&>(event);
    return UnlockFileEx(
             unlk_aiocb.get_file(),
             0,
             static_cast<DWORD>(unlk_aiocb.get_flock().get_len()),
             static_cast<DWORD>(unlk_aiocb.get_flock().get_len() >> 32),
             unlk_aiocb
           ) == TRUE
           ||
           GetLastError() == ERROR_IO_PENDING;
  }
  break;

  default:
    return yield::aio::win32::AIOQueue::enqueue(event);
  }
  }
}
}
}
}
}