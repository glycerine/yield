// yield/fs/posix/file.hpp

// Copyright (c) 2010 Minor Gordon
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


#include "yield/fs/file.hpp"


struct flock;


namespace yield {
namespace fs {
namespace posix {
class File : public yield::fs::File {
public:
  class Lock : public yield::fs::File::Lock {
  public:
    Lock(const flock& flock_);
    Lock(const yield::fs::File::Lock&);

    operator flock() const;
  };

public:
  File(fd_t fd);
  virtual ~File();

  // Channel
  bool close();
  operator fd_t() const {
    return fd;
  }
  ssize_t read(void* buf, size_t buflen);
  ssize_t readv(const iovec* iov, int iovlen);
  bool set_blocking_mode(bool blocking_mode);
  ssize_t write(const void* buf, size_t buflen);
  ssize_t writev(const iovec* iov, int iovlen);

  // yield::fs::File
  YO_NEW_REF yield::fs::Stat* getattr();
  YO_NEW_REF yield::fs::File::Lock* getlk(const yield::fs::File::Lock&);
  virtual YO_NEW_REF ExtendedAttributes* openxattrs();
  ssize_t pread(void*, size_t, uint64_t);
  ssize_t preadv(const iovec*, int, uint64_t);
  ssize_t pwrite(const void*, size_t, uint64_t);
  ssize_t pwritev(const iovec*, int, uint64_t);
  uint64_t seek(int64_t offset, uint8_t whence);
  bool setlk(const yield::fs::File::Lock&);
  bool setlkw(const yield::fs::File::Lock&);
  bool sync();
  uint64_t tell();
  bool truncate(uint64_t new_size);
  bool unlk(const yield::fs::File::Lock&);

private:
  fd_t fd;
};
}
}
}


#endif
