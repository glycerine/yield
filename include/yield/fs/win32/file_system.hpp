// yield/fs/win32/file_system.hpp

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

#ifndef _YIELD_FS_WIN32_FILE_SYSTEM_HPP_
#define _YIELD_FS_WIN32_FILE_SYSTEM_HPP_

#include "yield/object.hpp"

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


namespace yield {
class DateTime;

namespace fs {
class Path;

namespace win32 {
class Directory;
class File;
class MemoryMappedFile;
class Stat;

class FileSystem {
public:
  static int MMAP_FLAGS_DEFAULT; // MAP_SHARED
  const static size_t MMAP_LENGTH_WHOLE_FILE = static_cast<size_t>(-1);
  static int MMAP_PROT_DEFAULT; // PROT_READ|PROT_WRITE

public:
  YO_NEW_REF File* creat(const Path&);

  bool exists(const Path&);
  bool isdir(const Path&);
  bool isfile(const Path&);

  bool link(const Path& old_path, const Path& new_path);

  bool mkdir(const Path&);

  YO_NEW_REF File* mkfifo(const Path&, uint32_t flags = 0);

  bool mktree(const Path&);

  YO_NEW_REF File*
  open(
    const Path&,
    uint32_t flags = 0,
    uint32_t attributes = 0
  );

  YO_NEW_REF Directory* opendir(const Path&);

  bool realpath(const Path&, OUT Path&);

  bool rename(const Path& from_path, const Path& to_path);

  bool rmdir(const Path&);
  bool rmtree(const Path&);

  YO_NEW_REF Stat* stat(const Path&);
  bool statvfs(const Path&, struct statvfs&);

  bool touch(const Path&);

  bool truncate(const Path&, uint64_t);

  bool unlink(const Path&);

  bool utime(const Path&, const DateTime& atime, const DateTime& mtime);
  bool utime(const Path&, const DateTime& atime, const DateTime& mtime, const DateTime& ctime);
};
}
}
}

#endif
