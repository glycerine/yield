// yield/fs/posix/file_system.cpp

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

#include "directory.hpp"
#include "file.hpp"
#include "memory_mapped_file.hpp"
#include "stat.hpp"
#include "file_system.hpp"
#include "yield/assert.hpp"

#include <errno.h>
#include <fcntl.h>
#include <limits.h> // for PATH_MAX
#include <stdio.h>
#include <stdlib.h> // for realpath
#include <sys/mman.h>
#include <sys/statvfs.h>
#include <sys/time.h>


namespace yield {
namespace fs {
namespace posix {
bool FileSystem::access(const Path& path, int amode) {
  return ::access(path.c_str(), amode) >= 0;
}

yield::fs::Stat* FileSystem::getattr(const Path& path) {
  struct stat stbuf;
  if (::stat(path.c_str(), &stbuf) != -1)
    return new Stat(stbuf);
  else
    return NULL;
}

bool FileSystem::link(const Path& old_path, const Path& new_path) {
  return ::link(old_path.c_str(), new_path.c_str()) != -1;
}

bool FileSystem::mkdir(const Path& path, mode_t mode) {
  return ::mkdir(path.c_str(), mode) != -1;
}

yield::fs::File*
FileSystem::mkfifo
(
  const Path& path,
  uint32_t flags,
  mode_t mode
) {
  if (::mkfifo(path.c_str(), mode) != -1)
    return open(path, flags | O_NONBLOCK, mode, 0);
  else
    return NULL;
}

void*
FileSystem::mmap
(
  void* start,
  size_t length,
  int prot,
  int flags,
  fd_t fd,
  uint64_t offset
) {
  return ::mmap(start, length, prot, flags, fd, offset);
}

yield::fs::MemoryMappedFile*
FileSystem::mmap
(
  yield::fs::File& file,
  void* start,
  size_t length,
  int prot,
  int flags,
  uint64_t offset
) {
  if (length == MMAP_LENGTH_WHOLE_FILE) {
    struct stat stbuf;
    if (::fstat(file, &stbuf) != -1)
      length = static_cast<size_t>(stbuf.st_size);
    else
      return NULL;
  }

  if (length > 0) {
    start = mmap(start, length, prot, flags, file, offset);
    if (start != MAP_FAILED) {
      return new MemoryMappedFile
             (
               length,
               start,
               static_cast<File&>(file),
               flags,
               offset,
               prot
             );
    } else
      return NULL;
  } else {
    return new MemoryMappedFile
           (
             length,
             MAP_FAILED,
             static_cast<File&>(file),
             flags,
             offset,
             prot
           );
  }
}

yield::fs::File*
FileSystem::open
(
  const Path& path,
  uint32_t flags,
  mode_t mode,
  uint32_t attributes
) {
  fd_t fd = ::open(path.c_str(), flags, mode);
  if (fd != -1)
    return new File(fd);
  else
    return NULL;
}

yield::fs::Directory* FileSystem::opendir(const Path& path) {
  DIR* dirp = ::opendir(path.c_str());
  if (dirp != NULL)
    return new Directory(dirp, path);
  else
    return NULL;
}

YO_NEW_REF ExtendedAttributes* FileSystem::openxattrs(const Path&) {
  errno = ENOTSUP;
  return NULL;
}

bool FileSystem::readlink(const Path& path, OUT Path& target_path) {
  char target_path_[PATH_MAX];
  ssize_t target_path_len
  = ::readlink(path.c_str(), target_path_, PATH_MAX);
  if (target_path_len > 0) {
    target_path.assign(target_path_, target_path_len);
    return true;
  } else
    return false;
}

bool FileSystem::realpath(const Path& path, OUT Path& realpath) {
  char realpath_[PATH_MAX];
  if (::realpath(path.c_str(), realpath_) != NULL) {
    realpath.assign(realpath_);
    return true;
  } else
    return false;
}

bool FileSystem::rename(const Path& from_path, const Path& to_path) {
  return ::rename(from_path.c_str(), to_path.c_str()) != -1;
}

bool FileSystem::rmdir(const Path& path) {
  return ::rmdir(path.c_str()) != -1;
}

bool FileSystem::setattr(const Path& path, const yield::fs::Stat& stbuf) {
  if
  (
    stbuf.has_attributes()
    ||
    stbuf.has_blksize()
    ||
    stbuf.has_blocks()
    ||
    stbuf.has_dev()
    ||
    stbuf.has_ino()
    ||
    stbuf.has_nlink()
    ||
    stbuf.has_rdev()
    ||
    stbuf.has_size()
  ) {
    errno = ENOTSUP;
    return false;
  }

  bool have_setattr = false;

  if (stbuf.has_atime() || stbuf.has_mtime()) {
    timeval tv[2];
    tv[0] = stbuf.get_atime();
    tv[1] = stbuf.get_mtime();
    if (::utimes(path.c_str(), tv) == 0)
      have_setattr = true;
    else {
      debug_assert_false(have_setattr);
      return false;
    }
  }

  if (stbuf.has_mode()) {
    if (::chmod(path.c_str(), stbuf.get_mode()) == 0)
      have_setattr = true;
    else {
      debug_assert_false(have_setattr);
      return false;
    }
  }

  if (stbuf.has_uid()) {
    if (stbuf.has_gid()) {   // Change both
      if (::chown(path.c_str(), stbuf.get_uid(), stbuf.get_gid()) == 0)
        have_setattr = true;
      else {
        debug_assert_false(have_setattr);
        return false;
      }
    } else { // Only change the uid
      if (::chown(path.c_str(), stbuf.get_uid(), -1) == 0)
        have_setattr = true;
      else {
        debug_assert_false(have_setattr);
        return false;
      }
    }
  } else if (stbuf.has_gid()) {   // Only change the gid
    if (::chown(path.c_str(), -1, stbuf.get_gid()) == 0)
      have_setattr = true;
    else {
      debug_assert_false(have_setattr);
      return false;
    }
  }

  return true;
}

bool FileSystem::statvfs(const Path& path, struct statvfs& stbuf) {
  return ::statvfs(path.c_str(), &stbuf) == 0;
}

bool FileSystem::symlink(const Path& old_path, const Path& new_path) {
  return ::symlink(old_path.c_str(), new_path.c_str()) != -1;
}

bool FileSystem::truncate(const Path& path, uint64_t new_size) {
  return ::truncate(path.c_str(), new_size) >= 0;
}

bool FileSystem::unlink(const Path& path) {
  return ::unlink(path.c_str()) != -1;
}
}
}
}
