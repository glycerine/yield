// yield/fs/posix/stat.hpp

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


#ifndef _YIELD_FS_POSIX_STAT_HPP_
#define _YIELD_FS_POSIX_STAT_HPP_


#include "yield/fs/stat.hpp"

#include <sys/stat.h>


namespace yield {
namespace fs {
namespace posix {
class Stat : public yield::fs::Stat, private stat {
public:
  Stat();
  Stat(const struct stat&);

  operator struct stat() const;
  Stat& operator=(const struct stat&);

  // Stat
  const DateTime& get_atime() const {
    return atime;
  }
  uint64_t get_blksize() const {
    return st_blksize;
  }
  uint64_t get_blocks() const {
    return st_blocks;
  }
  const DateTime& get_ctime() const {
    return ctime;
  }
  uint64_t get_dev() const {
    return st_dev;
  }
  gid_t get_gid() const {
    return st_gid;
  }
  uint64_t get_ino() const {
    return st_ino;
  }
  mode_t get_mode() const {
    return st_mode;
  }
  const DateTime& get_mtime() const {
    return mtime;
  }
  int16_t get_nlink() const {
    return st_nlink;
  }
  uint64_t get_rdev() const {
    return st_rdev;
  }
  uint64_t get_size() const {
    return st_size;
  }
  Type get_type() const;
  uid_t get_uid() const {
    return st_uid;
  }

private:
  DateTime atime, ctime, mtime;
};
}
}
}


#endif
