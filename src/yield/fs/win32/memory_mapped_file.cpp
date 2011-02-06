// yield/fs/win32/memory_mapped_file.cpp

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
#include "yield/exception.hpp"
#include "yield/fs/win32/file.hpp"
#include "yield/fs/win32/file_system.hpp"
#include "yield/fs/win32/memory_mapped_file.hpp"

#include <Windows.h>

namespace yield {
namespace fs {
namespace win32 {
MemoryMappedFile::MemoryMappedFile
(
  size_t capacity,
  void* data,
  File& file,
  int flags,
  HANDLE hFileMapping,
  uint64_t offset,
  int prot
)
  : Buffer(capacity),
    data_(data),
    file(file),
    flags(flags),
    hFileMapping(hFileMapping),
    offset(offset),
    prot(prot) {
  if (data_ == reinterpret_cast<void*>(-1)) {
    debug_assert_eq(hFileMapping, NULL);
  } else {
    debug_assert_ne(data_, NULL);
    debug_assert_ne(hFileMapping, NULL);
    debug_assert_ne(hFileMapping, INVALID_HANDLE_VALUE);
  }
}

MemoryMappedFile::~MemoryMappedFile() {
  close();
  File::dec_ref(file);
}

bool MemoryMappedFile::close() {
  return unmap() && get_file().close();
}

void MemoryMappedFile::reserve(size_t capacity) {
  if (data_ != reinterpret_cast<void*>(-1)) {
    if (!sync())
      throw Exception();

    if (!unmap())
      throw Exception();
  }

  debug_assert_eq(data_, reinterpret_cast<void*>(-1));
  debug_assert_eq(capacity_, 0);
  debug_assert_eq(hFileMapping, NULL);

  if (get_file().truncate(capacity)) {
    data_
    = FileSystem::mmap
      (
        capacity,
        get_prot(),
        get_flags(),
        static_cast<File&>(get_file()),
        get_offset(),
        hFileMapping
      );

    if (data_ != reinterpret_cast<void*>(-1))
      capacity_ = capacity;
    else
      throw Exception();
  } else
    throw Exception();
}

bool MemoryMappedFile::sync() {
  return sync(data(), capacity());
}

bool MemoryMappedFile::sync(size_t offset, size_t length) {
  return sync(static_cast<char*>(data()) + offset, length);
}

bool MemoryMappedFile::sync(void* ptr, size_t length) {
  if (data_ != reinterpret_cast<void*>(-1))
    return FlushViewOfFile(ptr, length) == TRUE;
  else {
    SetLastError(ERROR_INVALID_PARAMETER);
    return false;
  }
}

bool MemoryMappedFile::unmap() {
  if (data_ != reinterpret_cast<void*>(-1)) {
    debug_assert_ne(hFileMapping, NULL);

    if
    (
      UnmapViewOfFile(data_)
      &&
      CloseHandle(hFileMapping)
    ) {
      capacity_ = 0;
      data_ = reinterpret_cast<void*>(-1);
      hFileMapping = NULL;
      return true;
    } else {
      DebugBreak();
      return false;
    }
  } else {
    SetLastError(ERROR_INVALID_PARAMETER);
    return false;
  }
}
}
}
}
