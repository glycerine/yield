// yield/thread/reader_writer_lock.hpp

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

#ifndef _YIELD_THREAD_READER_WRITER_LOCK_HPP_
#define _YIELD_THREAD_READER_WRITER_LOCK_HPP_

#include "yield/types.hpp"

#ifdef _WIN32
struct _RTL_CRITICAL_SECTION;
typedef struct _RTL_CRITICAL_SECTION RTL_CRITICAL_SECTION;
typedef RTL_CRITICAL_SECTION CRITICAL_SECTION;
#else
#include <pthread.h>
#endif

namespace yield {
namespace thread {
/**
  Reader-writer lock synchronization primitive.
*/
class ReaderWriterLock {
public:
  ReaderWriterLock();
  ~ReaderWriterLock();

public:
  /**
    Wait indefinitely to acquire a reader (shared) lock.
    @return true if the caller now holds a reader lock
  */
  bool rdlock();

  /**
    Release a reader (shared) lock.
  */
  void rdunlock();

  /**
    Try to acquire a reader (shared) lock without blocking.
    @return true if the caller now holds a reader lock
  */
  bool tryrdlock();

  /**
    Try to acquire a writer (exclusive) lock without blocking.
    @return true if the caller now holds a writer lock
  */
  bool trywrlock();

  /**
    Wait indefinitely to acquire a writer (exclusive) lock.
    @return true if the caller now holds a writer lock
  */
  bool wrlock();

  /**
    Release a writer (exclusive) lock.
  */
  void wrunlock();

private:
#ifdef _WIN32
  uint32_t active_writer_readers;
  CRITICAL_SECTION* cs;
  void* hReadyToRead, *hReadyToWrite;
  int32_t waiting_readers_count, waiting_writers_count;
#else
  pthread_rwlock_t rwlock;
#endif
};
}
}

#endif
