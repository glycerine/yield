// yield/fs/posix/file.hpp

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

#ifndef _YIELD_FS_POSIX_FILE_HPP_
#define _YIELD_FS_POSIX_FILE_HPP_

#include "yield/buffer.hpp"
#include "yield/channel.hpp"

#include <fcntl.h>

namespace yield {
namespace fs {
namespace posix {
class ExtendedAttributes;
class Map;
class Stat;

class File : public Channel {
public:
  class Lock : public Object {
  public:
    Lock(
      uint64_t len,
      uint64_t start,
      bool exclusive = true,
      pid_t pid = -1,
      int16_t whence = SEEK_SET
    ) {
      flock_.l_len = len;
      flock_.l_pid = pid;
      flock_.l_start = start;
      flock_.l_type = exclusive ? F_WRLCK : F_RDLCK;
      flock_.l_whence = whence;
    }

    Lock(const flock& flock_) : flock_(flock_) {
    }

  public:
    uint64_t get_len() const {
      return flock_.l_len;
    }

    pid_t get_pid() const {
      return flock_.l_pid;
    }

    uint64_t get_start() const {
      return flock_.l_start;
    }

    int16_t get_whence() const {
      return flock_.l_whence;
    }

    bool is_exclusive() const {
      return flock_.l_type == F_WRLCK;
    }

  public:
    operator flock() const {
      return flock_;
    }

  public:
    // Object
    Lock& inc_ref() {
      return Object::inc_ref(*this);
    }

  private:
    flock flock_;
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

    bool is_read_only() const;
    bool is_shared() const;

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
      uint64_t file_offset,
      int flags,
      int prot
    );

  private:
    File& file;
    uint64_t file_offset;
    int flags;
    int prot;
  };

public:
  File(fd_t fd);
  virtual ~File();

public:
  virtual bool datasync();

public:
  YO_NEW_REF Lock* getlk(const File::Lock&);

public:
  YO_NEW_REF Map*
  mmap(
    size_t length = SIZE_MAX,
    uint64_t offset = 0,
    bool read_only = false,
    bool shared = true
  );

public:
  virtual YO_NEW_REF ExtendedAttributes* openxattrs();

public:
  operator fd_t() const {
    return fd;
  }

public:
  ssize_t pread(Buffer& buffer, uint64_t offset);
  ssize_t pread(void* buf, size_t len, uint64_t offset);
  ssize_t preadv(const iovec* iov, int iovlen, uint64_t offset);

public:
  ssize_t pwrite(const Buffer& buffer, uint64_t offset);
  ssize_t pwrite(const void* buf, size_t len, uint64_t offset);
  ssize_t pwritev(const iovec* iov, int iovlen, uint64_t);

public:
  uint64_t seek(int64_t offset, uint8_t whence = SEEK_SET);

public:
  bool setlk(const Lock&);
  bool setlkw(const Lock&);

public:
  YO_NEW_REF Stat* stat();

public:
  bool sync();

public:
  uint64_t tell();

public:
  bool truncate(uint64_t new_size);

public:
  bool unlk(const Lock&);

public:
  // yield::Object
  File& inc_ref() {
    return Object::inc_ref(*this);
  }

public:
  // yield::Channel
  bool close();
  ssize_t read(void* buf, size_t buflen);
  ssize_t readv(const iovec* iov, int iovlen);
  ssize_t write(const void* buf, size_t buflen);

private:
  fd_t fd;
};
}
}
}

#endif
