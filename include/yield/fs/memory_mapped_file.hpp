// yield/fs/memory_mapped_file.hpp

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

#ifndef _YIELD_FS_MEMORY_MAPPED_FILE_HPP_
#define _YIELD_FS_MEMORY_MAPPED_FILE_HPP_

#include "yield/buffer.hpp"


#ifdef _WIN32
#define MAP_FIXED   1
#define MAP_SHARED  2
#define MAP_PRIVATE 4
#define PROT_EXEC  0x10 // PAGE_EXECUTE
#define PROT_READ  0x02 // PAGE_READONLY
#define PROT_WRITE 0x04 // PAGE_READWRITE
#define PROT_NONE  0x01 // PAGE_NOACCESS
#endif


namespace yield {
namespace fs {
class File;


class MemoryMappedFile : public Buffer {
public:
  virtual ~MemoryMappedFile();

  bool close();
  File& get_file() const {
    return file;
  }
  int get_flags() const {
    return flags;
  }
  uint64_t get_offset() const {
    return offset;
  }
  int get_prot() const {
    return prot;
  }
  bool sync();
  bool sync(size_t offset, size_t length);
  virtual bool sync(void* ptr, size_t length) = 0;
  virtual bool unmap() = 0;

protected:
  MemoryMappedFile
  (
    size_t capacity,
    YO_NEW_REF File& file,
    int flags,
    uint64_t offset,
    int prot
  );

private:
  File& file;
  int flags;
  uint64_t offset;
  int prot;
};
}
}

#endif
