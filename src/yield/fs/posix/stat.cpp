// yield/fs/posix/stat.cpp

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

#include "yield/fs/posix/stat.hpp"

namespace yield {
namespace fs {
Stat::Stat(const struct stat& stbuf)
  : atime(stbuf.st_atime),
    ctime(stbuf.st_ctime),
    mtime(stbuf.st_mtime) {
  atime = DateTime(stbuf.st_atime);
  st_blksize = stbuf.st_blksize;
  st_blocks = stbuf.st_blocks;
  ctime = DateTime(stbuf.st_ctime);
  st_dev = stbuf.st_dev;
  st_gid = stbuf.st_gid;
  st_ino = stbuf.st_ino;
  st_mode = stbuf.st_mode;
  mtime = DateTime(stbuf.st_mtime);
  st_nlink = stbuf.st_nlink;
  st_rdev = stbuf.st_rdev;
  st_size = stbuf.st_size;
  st_uid = stbuf.st_uid;
}

uint64_t Stat::get_blksize() const {
  return st_blksize;
}

uint64_t Stat::get_blocks() const {
  return st_blocks;
}

uint64_t Stat::get_dev() const {
  return st_dev;
}

gid_t Stat::get_gid() const {
  return st_gid;
}

uint64_t Stat::get_ino() const {
  return st_ino;
}

mode_t Stat::get_mode() const {
  return st_mode;
}

int16_t Stat::get_nlink() const {
  return st_nlink;
}

uint64_t Stat::get_rdev() const {
  return st_rdev;
}

uint64_t Stat::get_size() const {
  return st_size;
}

uid_t Stat::get_uid() const {
  return st_uid;
}

bool Stat::ISBLK() const {
  return S_ISBLK(get_mode());
}

bool Stat::ISCHR() const {
  return S_ISCHR(get_mode());
}

bool Stat::ISDIR() const {
  return S_ISDIR(get_mode());
}

bool Stat::ISFIFO() const {
  return S_ISFIFO(get_mode());
}

bool Stat::ISLNK() const {
  return S_ISLNK(get_mode());
}

bool Stat::ISREG() const {
  return S_ISREG(get_mode());
}

bool Stat::ISSOCK() const {
  return S_ISSOCK(get_mode());
}

Stat::operator struct stat() const {
  return *this;
}

//Stat::operator struct stat() const {
//  struct stat stbuf;
//  memset(&stbuf, 0, sizeof(stbuf));
//  stbuf.st_dev = static_cast<dev_t>(get_dev());
//  stbuf.st_ino = static_cast<ino_t>(get_ino());
//  stbuf.st_mode = get_mode();
//  stbuf.st_nlink = get_nlink();
//  stbuf.st_uid = get_uid();
//  stbuf.st_gid = get_gid();
//  stbuf.st_rdev = static_cast<dev_t>(get_rdev());
//  stbuf.st_size = static_cast<off_t>(get_size());
//  stbuf.st_atime = static_cast<time_t>(get_atime().as_unix_date_time_s());
//  stbuf.st_mtime = static_cast<time_t>(get_mtime().as_unix_date_time_s());
//  stbuf.st_ctime = static_cast<time_t>(get_ctime().as_unix_date_time_s());
//  stbuf.st_blksize = get_blksize();
//  stbuf.st_blocks = get_blocks();
//  return stbuf;
//}

//Stat& Stat::operator=(const struct stat& stbuf) {
//  atime = DateTime(stbuf.st_atime);
//  st_blksize = stbuf.st_blksize;
//  st_blocks = stbuf.st_blocks;
//  ctime = DateTime(stbuf.st_ctime);
//  st_dev = stbuf.st_dev;
//  st_gid = stbuf.st_gid;
//  st_ino = stbuf.st_ino;
//  st_mode = stbuf.st_mode;
//  mtime = DateTime(stbuf.st_mtime);
//  st_nlink = stbuf.st_nlink;
//  st_rdev = stbuf.st_rdev;
//  st_size = stbuf.st_size;
//  st_uid = stbuf.st_uid;
//  return *this;
//}

bool Stat::operator==(const Stat& other) const {
  return get_atime() == other.get_atime() &&
         get_blksize() == other.get_blksize() &&
         get_blocks() == other.get_blocks() &&
         get_ctime() == other.get_ctime() &&
         get_dev() == other.get_dev() &&
         get_gid() == other.get_gid() &&
         get_ino() == other.get_ino() &&
         get_mode() == other.get_mode() &&
         get_mtime() == other.get_mtime() &&
         get_nlink() == other.get_nlink() &&
         get_rdev() == other.get_rdev() &&
         get_size() == other.get_size() &&
         get_uid() == other.get_uid();
}
}
}
