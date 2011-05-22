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

#ifdef _WIN32
struct _BY_HANDLE_FILE_INFORMATION;
typedef _BY_HANDLE_FILE_INFORMATION BY_HANDLE_FILE_INFORMATION;
struct _WIN32_FILE_ATTRIBUTE_DATA;
typedef struct _WIN32_FILE_ATTRIBUTE_DATA WIN32_FILE_ATTRIBUTE_DATA;
struct _WIN32_FIND_DATAW;
typedef _WIN32_FIND_DATAW WIN32_FIND_DATAW;
typedef WIN32_FIND_DATAW WIN32_FIND_DATA;
#endif

namespace yield {
namespace fs {
class Stat
  : public Object
#ifndef _WIN32
    , private stat
#endif
{
public:
#ifdef _WIN32
  Stat(
    const DateTime& atime,
    uint32_t attributes,
    const DateTime& ctime,
    const DateTime& mtime,
    int16_t nlink,
    uint64_t size
  );

  Stat(const BY_HANDLE_FILE_INFORMATION&);
  Stat(const WIN32_FILE_ATTRIBUTE_DATA&);
  Stat(const WIN32_FIND_DATA&);
#else
  Stat(const struct stat&);
#endif

public:
  const DateTime& get_atime() const {
    return atime;
  }

#ifdef _WIN32
  uint32_t get_attributes() const;
#endif

#ifndef _WIN32
  uint64_t get_blksize() const;
  uint64_t get_blocks() const
#endif

  const DateTime& get_ctime() const {
    return ctime;
  }

#ifndef _WIN32
  uint64_t get_dev() const;
  gid_t get_gid() const;
  uint64_t get_ino() const;
  mode_t get_mode() const
#endif

  const DateTime& get_mtime() const {
    return mtime;
  }

  int16_t get_nlink() const;

#ifndef _WIN32
  uint64_t get_rdev() const
#endif

  uint64_t get_size() const;

public:
#ifndef _WIN32
  bool ISBLK() const;
  bool ISCHR() const;
#endif
  bool ISDEV() const;
  bool ISDIR() const;
#ifndef _WIN32
  bool ISFIFO() const;
  bool ISLNK() const;
#endif
  bool ISREG() const;
#ifndef _WIN32
  bool ISSOCK() const;
#endif

public:
  bool operator==(const Stat&) const;

public:
#ifdef _WIN32
  operator BY_HANDLE_FILE_INFORMATION() const;
  operator WIN32_FILE_ATTRIBUTE_DATA() const;
  operator WIN32_FIND_DATA() const;
#else
  operator struct stat() const;
#endif

public:
#ifdef _WIN32
  Stat& operator=(const BY_HANDLE_FILE_INFORMATION&);
  Stat& operator=(const WIN32_FILE_ATTRIBUTE_DATA&);
  virtual Stat& operator=(const WIN32_FIND_DATA&);
#endif

public:
  // yield::Object
  Stat& inc_ref() {
    return Object::inc_ref(*this);
  }

private:
  void set_size(uint32_t nFileSizeLow, uint32_t nFileSizeHigh);

private:
  DateTime atime;
#ifdef _WIN32
  uint32_t attributes;
#endif
  DateTime ctime;
  DateTime mtime;
#ifdef _WIN32
  int16_t nlink;
  uint64_t size;
#endif
};
}
}

#endif
