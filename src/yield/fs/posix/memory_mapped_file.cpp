// yield/fs/posix/memory_mapped_file.cpp

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


#include "file_system.hpp"
#include "memory_mapped_file.hpp"
#include "yield/assert.hpp"
#include "yield/exception.hpp"

#include <errno.h>
#include <sys/mman.h>


namespace yield {
namespace fs {
namespace posix {
MemoryMappedFile::MemoryMappedFile
(
  size_t capacity,
  void* data,
  YO_NEW_REF File& file,
  int flags,
  uint64_t offset,
  int prot
)
  : yield::fs::MemoryMappedFile(capacity, file, flags, offset, prot),
    data_(data)
{ }

MemoryMappedFile::~MemoryMappedFile() {
  close();
}

void MemoryMappedFile::reserve(size_t capacity) {
  if (data_ != MAP_FAILED) {
    if (!yield::fs::MemoryMappedFile::sync())
      throw Exception();

    if (!unmap())
      throw Exception();
  }

  debug_assert_eq(capacity_, 0);
  debug_assert_eq(data_, MAP_FAILED);

  if (get_file().truncate(capacity)) {
    data_
    = FileSystem::mmap
      (
        NULL,
        capacity,
        get_prot(),
        get_flags(),
        static_cast<File&>(get_file()),
        get_offset()
      );

    if (data_ != MAP_FAILED)
      capacity_ = capacity;
    else
      throw Exception();
  } else
    throw Exception();
}

bool MemoryMappedFile::sync(void* ptr, size_t length) {
  if (data_ != MAP_FAILED) {
#ifdef __sun
    return msync(static_cast<char*>(ptr), length, MS_SYNC) == 0;
#else
    return msync(ptr, length, MS_SYNC) == 0;
#endif
  } else {
    errno = EBADF;
    return false;
  }
}

bool MemoryMappedFile::unmap() {
  if (data_ != MAP_FAILED) {
    if (munmap(data_, capacity()) == 0) {
      capacity_ = 0;
      data_ = MAP_FAILED;
      return true;
    } else
      return false;
  } else {
    errno = EBADF;
    return false;
  }
}
}
}
}
