// yield/fs/stat.hpp

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


#ifndef _YIELD_FS_STAT_HPP_
#define _YIELD_FS_STAT_HPP_


#include "yield/date_time.hpp"
#include "yield/object.hpp"

#include <ostream>


namespace yield {
namespace fs {
class Stat : public Object {
public:
  typedef uint8_t Type;
  const static Type TYPE_ALL = static_cast<Type>(~0);
  const static Type TYPE_CHR = 1;
  const static Type TYPE_BLK = 2;
  const static Type TYPE_DIR = 4;
  const static Type TYPE_FIFO = 8;
  const static Type TYPE_LNK = 16;
  const static Type TYPE_REG = 32;
  const static Type TYPE_SOCK = 64;

  const static DateTime INVALID_ATIME;
  const static uint32_t INVALID_ATTRIBUTES = static_cast<uint32_t>(~0);
  const static uint64_t INVALID_BLKSIZE = static_cast<uint64_t>(~0);
  const static uint64_t INVALID_BLOCKS = static_cast<uint64_t>(~0);
  const static DateTime INVALID_CTIME;
  const static uint64_t INVALID_DEV = static_cast<uint64_t>(~0);
  const static gid_t INVALID_GID = static_cast<gid_t>(-1);
  const static uint64_t INVALID_INO = static_cast<uint64_t>(~0);
  const static mode_t INVALID_MODE = static_cast<mode_t>(-1);
  const static DateTime INVALID_MTIME;
  const static int16_t INVALID_NLINK = 0;
  const static uint64_t INVALID_RDEV = static_cast<uint64_t>(~0);
  const static uint64_t INVALID_SIZE = static_cast<uint64_t>(~0);
  const static Type INVALID_TYPE = 0;
  const static uid_t INVALID_UID = static_cast<uid_t>(-1);

public:
  virtual const DateTime& get_atime() const {
    return INVALID_ATIME;
  }
  virtual uint32_t get_attributes() const {
    return INVALID_ATTRIBUTES;
  }
  virtual uint64_t get_blksize() const {
    return INVALID_BLKSIZE;
  }
  virtual uint64_t get_blocks() const {
    return INVALID_BLOCKS;
  }
  virtual const DateTime& get_ctime() const {
    return INVALID_CTIME;
  }
  virtual uint64_t get_dev() const {
    return INVALID_DEV;
  }
  virtual gid_t get_gid() const {
    return INVALID_GID;
  }
  virtual uint64_t get_ino() const {
    return INVALID_INO;
  }
  virtual mode_t get_mode() const {
    return INVALID_MODE;
  }
  virtual const DateTime& get_mtime() const {
    return INVALID_MTIME;
  }
  virtual int16_t get_nlink() const {
    return INVALID_NLINK;
  }
  virtual uint64_t get_rdev() const {
    return INVALID_RDEV;
  }
  virtual uint64_t get_size() const {
    return INVALID_SIZE;
  }
  virtual Type get_type() const {
    return INVALID_TYPE;
  }
  virtual uid_t get_uid() const {
    return INVALID_UID;
  }

  bool has_atime() const {
    return get_atime() != INVALID_ATIME;
  }
  bool has_attributes() const;
  bool has_blksize() const {
    return get_blksize() != INVALID_BLKSIZE;
  }
  bool has_blocks() const {
    return get_blocks() != INVALID_BLOCKS;
  }
  bool has_ctime() const {
    return get_ctime() != INVALID_CTIME;
  }
  bool has_dev() const {
    return get_dev() != INVALID_DEV;
  }
  bool has_gid() const {
    return get_gid() != INVALID_GID;
  }
  bool has_ino() const {
    return get_ino() != INVALID_INO;
  }
  bool has_mode() const {
    return get_mode() != INVALID_MODE;
  }
  bool has_mtime() const {
    return get_mtime() != INVALID_MTIME;
  }
  bool has_nlink() const {
    return get_nlink() != INVALID_NLINK;
  }
  bool has_rdev() const {
    return get_rdev() != INVALID_RDEV;
  }
  bool has_size() const {
    return get_size() != INVALID_SIZE;
  }
  bool has_uid() const {
    return get_uid() != INVALID_UID;
  }

  bool ISBLK() const {
    return get_type() == TYPE_BLK;
  }
  bool ISCHR() const {
    return get_type() == TYPE_CHR;
  }
  bool ISDIR() const {
    return get_type() == TYPE_DIR;
  }
  bool ISFIFO() const {
    return get_type() == TYPE_FIFO;
  }
  bool ISLNK() const {
    return get_type() == TYPE_LNK;
  }
  bool ISREG() const {
    return get_type() == TYPE_REG;
  }
  bool ISSOCK() const {
    return get_type() == TYPE_SOCK;
  }

  virtual bool operator==(const Stat& other) const;
};
}
}


#endif
