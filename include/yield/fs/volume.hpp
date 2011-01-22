// yield/fs/volume.hpp

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


#ifndef _YIELD_FS_VOLUME_HPP_
#define _YIELD_FS_VOLUME_HPP_


#include "yield/object.hpp"
#include "yield/fs/path.hpp"


#ifdef _WIN32
struct statvfs {
  unsigned long f_bsize;    // File system block size.
  unsigned long f_frsize;   // Fundamental file system block size.
  uint64_t    f_blocks;     // Total number of blocks on file system
  // in units of f_frsize.
  uint64_t    f_bfree;      // Total number of free blocks.
  uint64_t    f_bavail;     // Number of free blocks available to
  // non-privileged process.
  uint64_t    f_files;      // Total number of file serial numbers.
  uint64_t    f_ffree;      // Total number of free file serial numbers.
  uint64_t    f_favail;     // Number of file serial numbers available to
  // non-privileged process.
  unsigned long f_fsid;     // File system ID.
  unsigned long f_flag;     // Bit mask of f_flag values.
  unsigned long f_namemax;  // Maximum filename length.
};
#else
struct statvfs;
#endif


namespace yield {
class DateTime;
class Log;


namespace fs {
class Directory;
class ExtendedAttributes;
class File;
class MemoryMappedFile;
class Stat;


class Volume : public Object {
public:
  static mode_t FILE_MODE_DEFAULT;
  static mode_t DIRECTORY_MODE_DEFAULT;
  static int MMAP_FLAGS_DEFAULT; // MAP_SHARED
  const static size_t MMAP_LENGTH_WHOLE_FILE = static_cast<size_t>(-1);
  static int MMAP_PROT_DEFAULT; // PROT_READ|PROT_WRITE
  const static uint32_t OPEN_ATTRIBUTES_DEFAULT = 0;
  static uint32_t OPEN_FLAGS_DEFAULT; // O_RDONLY

public:
  static Volume* create();

  virtual ~Volume() { }

  virtual bool access(const Path&, int amode) = 0;
  bool chmod(const Path&, mode_t);
  bool chown(const Path&, uid_t, gid_t);
  YO_NEW_REF File* creat(const Path&);
  YO_NEW_REF File* creat(const Path&, mode_t);
  bool exists(const Path& path);
  virtual YO_NEW_REF Stat* getattr(const Path&) = 0;
  virtual bool isdir(const Path&);
  virtual bool isfile(const Path&);
  virtual bool link(const Path& old_path, const Path& new_path) = 0;
  bool mkdir(const Path&);
  virtual bool mkdir(const Path&, mode_t) = 0;

  virtual YO_NEW_REF File*
  mkfifo
  (
    const Path&,
    uint32_t flags = OPEN_FLAGS_DEFAULT,
    mode_t mode = FILE_MODE_DEFAULT
  ) = 0;

  bool mktree(const Path&);
  bool mktree(const Path&, mode_t);

  virtual YO_NEW_REF MemoryMappedFile*
  mmap
  (
    YO_NEW_REF File& file,
    void* start = NULL,
    size_t length = MMAP_LENGTH_WHOLE_FILE,
    int prot = MMAP_PROT_DEFAULT,
    int flags = MMAP_FLAGS_DEFAULT,
    uint64_t offset = 0
  ) = 0;

  virtual YO_NEW_REF File*
  open
  (
    const Path& path,
    uint32_t flags = OPEN_FLAGS_DEFAULT,
    mode_t mode = FILE_MODE_DEFAULT,
    uint32_t attributes = OPEN_ATTRIBUTES_DEFAULT
  ) = 0;

  virtual YO_NEW_REF Directory* opendir(const Path&) = 0;
  virtual YO_NEW_REF ExtendedAttributes* openxattrs(const Path&) = 0;
  virtual bool readlink(const Path&, OUT Path&) = 0;
  virtual bool realpath(const Path&, OUT Path&) = 0;
  virtual bool rename(const Path& from_path, const Path& to_path) = 0;
  virtual bool rmdir(const Path& path) = 0;
  bool rmtree(const Path&);
  virtual bool setattr(const Path&, const Stat& stbuf) = 0;
  YO_NEW_REF Stat* stat(const Path& p) {
    return getattr(p);
  }
  virtual bool statvfs(const Path&, struct statvfs&) = 0;
  virtual bool symlink(const Path& old_path, const Path& new_path) = 0;
  bool touch(const Path&);
  bool touch(const Path&, mode_t);
  virtual bool truncate(const Path&, uint64_t new_size) = 0;
  virtual bool unlink(const Path& path) = 0;

  bool
  utime
  (
    const Path&,
    const DateTime& atime,
    const DateTime& mtime
  );

  bool
  utime
  (
    const Path&,
    const DateTime& atime,
    const DateTime& mtime,
    const DateTime& ctime
  );

  virtual bool volname(const Path&, OUT Path&) = 0;

  // Object
  Volume& inc_ref() {
    return Object::inc_ref(*this);
  }

private:
  class chmodStat;
  class chownStat;
  class utimeStat;
};
}
}


#endif
