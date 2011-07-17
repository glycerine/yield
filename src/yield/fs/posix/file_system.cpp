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

#include "yield/auto_object.hpp"
#include "yield/assert.hpp"
#include "yield/fs/directory.hpp"
#include "yield/fs/file.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/fs/stat.hpp"

#include <errno.h>
#include <fcntl.h>
#include <limits.h> // for PATH_MAX
#include <stdio.h>
#include <stdlib.h> // for realpath
#include <sys/statvfs.h>
#include <sys/time.h>

namespace yield {
namespace fs {
mode_t FileSystem::FILE_MODE_DEFAULT = S_IREAD | S_IWRITE;
mode_t FileSystem::DIRECTORY_MODE_DEFAULT = S_IREAD | S_IWRITE | S_IEXEC;

bool FileSystem::access(const Path& path, int amode) {
  return ::access(path.c_str(), amode) == 0;
}

bool FileSystem::chmod(const Path& path, mode_t mode) {
  return ::chmod(path.c_str(), mode) == 0;
}

bool FileSystem::chown(const Path& path, uid_t uid) {
  return ::chown(path.c_str(), uid, -1) == 0;
}

bool FileSystem::chown(const Path& path, uid_t uid, gid_t gid) {
  return ::chown(path.c_str(), uid, gid) == 0;
}

YO_NEW_REF File* FileSystem::creat(const Path& path) {
  return creat(path, FILE_MODE_DEFAULT);
}

YO_NEW_REF File* FileSystem::creat(const Path& path, mode_t mode) {
  return open(path, O_CREAT | O_WRONLY | O_TRUNC, mode);
}

bool FileSystem::exists(const Path& path) {
  return access(path, F_OK);
}

bool FileSystem::isdir(const Path& path) {
  struct stat stbuf;
  return ::stat(path.c_str(), &stbuf) != -1 && S_ISDIR(stbuf.st_mode);
}

bool FileSystem::isreg(const Path& path) {
  struct stat stbuf;
  return ::stat(path.c_str(), &stbuf) != -1 && S_ISREG(stbuf.st_mode);
}

bool FileSystem::link(const Path& old_path, const Path& new_path) {
  return ::link(old_path.c_str(), new_path.c_str()) == 0;
}

YO_NEW_REF Stat* FileSystem::lstat(const Path& path) {
  struct stat stbuf;
  if (::lstat(path.c_str(), &stbuf) == 0)
    return new Stat(stbuf);
  else
    return NULL;
}

bool FileSystem::mkdir(const Path& path) {
  return mkdir(path, DIRECTORY_MODE_DEFAULT);
}

bool FileSystem::mkdir(const Path& path, mode_t mode) {
  return ::mkdir(path.c_str(), mode) == 0;
}

YO_NEW_REF File*
FileSystem::mkfifo(
  const Path& path,
  uint32_t flags,
  mode_t mode
) {
  if (::mkfifo(path.c_str(), mode) == 0)
    return open(path, flags | O_NONBLOCK, mode);
  else
    return NULL;
}

bool FileSystem::mktree(const Path& path) {
  return mktree(path, DIRECTORY_MODE_DEFAULT);
}

bool FileSystem::mktree(const Path& path, mode_t mode) {
  bool ret = true;

  std::pair<Path, Path> path_parts = path.split();
  if (!path_parts.first.empty())
    ret &= mktree(path_parts.first, mode);

  if (!exists(path) && !mkdir(path, mode))
    return false;

  return ret;
}

File*
FileSystem::open(
  const Path& path,
  uint32_t flags,
  mode_t mode
) {
  fd_t fd = ::open(path.c_str(), flags, mode);
  if (fd >= 0)
    return new File(fd);
  else
    return NULL;
}

Directory* FileSystem::opendir(const Path& path) {
  DIR* dirp = ::opendir(path.c_str());
  if (dirp != NULL)
    return new Directory(dirp, path);
  else
    return NULL;
}

bool FileSystem::readlink(const Path& path, Path& target_path) {
  char target_path_[PATH_MAX];
  ssize_t target_path_len = ::readlink(path.c_str(), target_path_, PATH_MAX);
  if (target_path_len > 0) {
    target_path.assign(target_path_, target_path_len);
    return true;
  } else
    return false;
}

bool FileSystem::realpath(const Path& path, Path& realpath) {
  char realpath_[PATH_MAX];
  if (::realpath(path.c_str(), realpath_) != NULL) {
    realpath.assign(realpath_);
    return true;
  } else
    return false;
}

bool FileSystem::rename(const Path& from_path, const Path& to_path) {
  return ::rename(from_path.c_str(), to_path.c_str()) == 0;
}

bool FileSystem::rmdir(const Path& path) {
  return ::rmdir(path.c_str()) == 0;
}

bool FileSystem::rmtree(const Path& path) {
  Directory* test_dir = opendir(path);
  if (test_dir != NULL) {
    auto_Object<Directory> dir(test_dir);
    Directory::Entry* test_dentry = dir->read();
    if (test_dentry != NULL) {
      auto_Object<Directory::Entry> dentry(*test_dentry);

      do {
        if (dentry->is_special())
          continue;

        Path dentry_path(path / dentry->get_name());

        if (dentry->ISDIR()) {
          if (rmtree(dentry_path))
            continue;
          else
            return false;
        } else if (unlink(dentry_path))
          continue;
        else
          return false;
      } while (dir->read(*dentry));

      return rmdir(path);
    }
  }

  return false;
}

Stat* FileSystem::stat(const Path& path) {
  struct stat stbuf;
  if (::stat(path.c_str(), &stbuf) == 0)
    return new Stat(stbuf);
  else
    return NULL;
}

bool FileSystem::statvfs(const Path& path, struct statvfs& stbuf) {
  return ::statvfs(path.c_str(), &stbuf) == 0;
}

bool FileSystem::symlink(const Path& old_path, const Path& new_path) {
  return ::symlink(old_path.c_str(), new_path.c_str()) == 0;
}

bool FileSystem::touch(const Path& path) {
  return touch(path, FILE_MODE_DEFAULT);
}

bool FileSystem::touch(const Path& path, mode_t mode) {
  File* file = creat(path, mode);
  if (file != NULL) {
    File::dec_ref(*file);
    return true;
  } else
    return false;
}

bool FileSystem::unlink(const Path& path) {
  return ::unlink(path.c_str()) == 0;
}

bool FileSystem::utime(const Path& path, const DateTime& atime, const DateTime& mtime) {
  timeval tv[2];
  tv[0] = atime;
  tv[1] = mtime;
  return ::utimes(path.c_str(), tv) == 0;
}
}
}
