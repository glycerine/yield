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

#include "yield/channel.hpp"

#define O_SYNC     010000
#define O_ASYNC    020000
#define O_DIRECT   040000
#define O_HIDDEN   0100000
#define O_NONBLOCK 0200000

namespace yield {
namespace fs {
namespace win32 {
class Stat;

class File : public Channel {
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

    // Object
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
  File(fd_t fd);
  ~File();

public:
  bool close();
  virtual bool datasync();
  //YO_NEW_REF Lock* getlk(const Lock&);
  ssize_t pread(void*, size_t, uint64_t);
  ssize_t preadv(const iovec*, int, uint64_t);
  ssize_t pwrite(const void*, size_t, uint64_t);
  ssize_t pwritev(const iovec*, int, uint64_t);
  uint64_t seek(int64_t offset, uint8_t whence = SEEK_SET);
  bool setlk(const Lock&);
  bool setlkw(const Lock&);
  YO_NEW_REF Stat* stat();
  virtual bool sync();
  uint64_t tell();
  bool truncate(uint64_t);
  bool unlk(const Lock&);

public:
  // Object
  File& inc_ref() {
    return Object::inc_ref(*this);
  }

public:
  // Channel
  operator fd_t() const {
    return fd;
  }
  ssize_t read(void* buf, size_t buflen);
  ssize_t readv(const iovec* iov, int iovlen);
  virtual bool set_blocking_mode(bool blocking_mode);
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
