// yield/fs/win32/file.hpp

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

#ifndef _YIELD_FS_WIN32_FILE_HPP_
#define _YIELD_FS_WIN32_FILE_HPP_

#include "yield/buffer.hpp"
#include "yield/channel.hpp"

#define O_SYNC     010000
#define O_ASYNC    020000
#define O_DIRECT   040000
#define O_HIDDEN   0100000
#define O_NONBLOCK 0200000

namespace yield {
namespace fs {
namespace win32 {
class Map;
class Stat;

class File : public Channel {
public:
  const static uint32_t TYPE_ID = 1965528734UL;

public:
  class Lock : public Object {
  public:
    Lock(
      uint64_t start,
      uint64_t len,
      bool exclusive = true,
      int16_t whence = SEEK_SET
    )
      : exclusive(exclusive),
        len(len),
        start(start),
        whence(whence)
    { }

    uint64_t get_len() const {
      return len;
    }

    uint64_t get_start() const {
      return start;
    }
    int16_t get_whence() const {
      return whence;
    }
    bool is_exclusive() const {
      return exclusive;
    }

    // yield::Object
    File::Lock& inc_ref() {
      return Object::inc_ref(*this);
    }

  private:
    bool exclusive;
    uint64_t len;
    uint64_t start;
    int16_t whence;
  };

public:
  class Map : public Buffer {
  public:
    ~Map();

  public:
    File& get_file() {
      return file;
    }

    uint64_t get_file_offset() const {
      return file_offset;
    }

    bool is_read_only() const {
      return read_only;
    }

    bool is_shared() const {
      return shared;
    }

  public:
    bool sync();
    bool sync(size_t offset, size_t length);
    bool sync(void* ptr, size_t length);

    bool unmap();

  private:
    friend class File;

    Map(
      size_t capacity,
      void* data,
      File& file,
      fd_t file_mapping,
      uint64_t file_offset,
      unsigned int flags,
      unsigned int prot,
      bool read_only,
      bool shared
    );

  private:
    File& file;
    fd_t file_mapping;
    uint64_t file_offset;
    unsigned int flags;
    unsigned int prot;
    bool read_only;
    bool shared;
  };

public:
  File(fd_t fd);
  ~File();

public:
  bool datasync();

public:
  YO_NEW_REF File* dup() {
    return dup(fd);
  }

  static YO_NEW_REF File* dup(fd_t fd);
  static YO_NEW_REF File* dup(FILE* file);

public:
  YO_NEW_REF Map*
  mmap(
    size_t length = SIZE_MAX,
    off_t offset = 0,
    bool read_only = false,
    bool shared = true
  );

public:
  operator fd_t() const {
    return fd;
  }

public:
  ssize_t pread(Buffer& buffer, off_t offset);
  ssize_t pread(void* buf, size_t buflen, off_t offset);
  ssize_t preadv(const iovec* iov, int iovlen, off_t offset);

public:
  ssize_t pwrite(const Buffer& buffer, off_t offset);
  ssize_t pwrite(const void* buf, size_t buflen, off_t offset);
  ssize_t pwritev(const iovec* iov, int iovlen, off_t offset);

public:
  off_t seek(off_t offset, uint8_t whence = SEEK_SET);

public:
  bool setlk(const Lock&);
  bool setlkw(const Lock&);

public:
  YO_NEW_REF Stat* stat();

public:
  bool sync();

public:
  off_t tell();

public:
  bool truncate(off_t length);

public:
  bool unlk(const Lock&);

public:
  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }

  const char* get_type_name() const {
    return "yield::fs::win32::File";
  }

  File& inc_ref() {
    return Object::inc_ref(*this);
  }

public:
  // yield::Channel
  bool close();
  ssize_t read(Buffer& buffer);
  ssize_t read(void* buf, size_t buflen);
  ssize_t readv(const iovec* iov, int iovlen);
  ssize_t write(const Buffer& buffer);
  ssize_t write(const void* buf, size_t buflen);
  ssize_t writev(const iovec* iov, int iovlen);

private:
  bool setlk(const Lock&, bool wait);

private:
  fd_t fd;
};
}
}
}

#endif
