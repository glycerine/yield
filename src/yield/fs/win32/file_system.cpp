// yield/fs/win32/file_system.cpp

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

#include "named_pipe.hpp"
#include "yield/auto_object.hpp"
#include "yield/assert.hpp"
#include "yield/fs/path.hpp"
#include "yield/fs/directory.hpp"
#include "yield/fs/file.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/fs/stat.hpp"

#include <fcntl.h> // For O_*
#include <Windows.h>

namespace yield {
namespace fs {
YO_NEW_REF File* FileSystem::creat(const Path& path) {
  return open(path, O_CREAT | O_WRONLY | O_TRUNC);
}

bool FileSystem::exists(const Path& path) {
  return GetFileAttributes(path.c_str()) != INVALID_FILE_ATTRIBUTES;
}

bool FileSystem::isdir(const Path& path) {
  DWORD dwAttributes = GetFileAttributes(path.c_str());
  return dwAttributes != INVALID_FILE_ATTRIBUTES
         &&
         (dwAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool FileSystem::isreg(const Path& path) {
  DWORD dwAttributes = GetFileAttributes(path.c_str());
  return dwAttributes != INVALID_FILE_ATTRIBUTES
         &&
         (
           dwAttributes == FILE_ATTRIBUTE_NORMAL
           ||
           dwAttributes == FILE_ATTRIBUTE_ARCHIVE
         );
}

bool FileSystem::link(const Path& old_path, const Path& new_path) {
  return CreateHardLink(new_path.c_str(), old_path.c_str(), NULL) == TRUE;
}

bool FileSystem::mkdir(const Path& path) {
  return CreateDirectory(path.c_str(), NULL) == TRUE;
}

File* FileSystem::mkfifo(const Path& path, uint32_t flags) {
  if (path.find_first_of(L"\\\\.\\pipe") != Path::npos) {
    DWORD dwOpenMode = 0;
    if ((flags & O_ASYNC) == O_ASYNC)
      dwOpenMode |= FILE_FLAG_OVERLAPPED;
    if ((flags & O_RDWR) == O_RDWR)
      dwOpenMode |= PIPE_ACCESS_DUPLEX;
    else if ((flags & O_WRONLY) == O_WRONLY)
      dwOpenMode |= PIPE_ACCESS_OUTBOUND;
    else
      dwOpenMode |= PIPE_ACCESS_INBOUND;

    DWORD dwPipeMode = PIPE_TYPE_BYTE | PIPE_READMODE_BYTE;
    if ((flags & O_NONBLOCK) == O_NONBLOCK)
      dwPipeMode |= PIPE_NOWAIT;
    else
      dwPipeMode |= PIPE_WAIT;

    HANDLE hNamedPipe
    = CreateNamedPipe(
        path.c_str(),
        dwOpenMode,
        dwPipeMode,
        PIPE_UNLIMITED_INSTANCES,
        4096,
        4096,
        0,
        NULL
      );

    if (hNamedPipe != INVALID_HANDLE_VALUE)
      return new NamedPipe(hNamedPipe);
    else
      return NULL;
  } else {
    SetLastError(ERROR_INVALID_PARAMETER);
    return NULL;
  }
}

bool FileSystem::mktree(const Path& path) {
  bool ret = true;

  std::pair<Path, Path> path_parts = path.split();
  if (!path_parts.first.empty())
    ret &= mktree(path_parts.first);

  if (!exists(path) && !mkdir(path))
    return false;

  return ret;
}

File*
FileSystem::open(
  const Path& path,
  uint32_t flags,
  uint32_t attributes
) {
  DWORD dwDesiredAccess = 0,
        dwCreationDisposition = 0,
        dwFlagsAndAttributes = attributes | FILE_FLAG_SEQUENTIAL_SCAN;

  if ((flags & O_APPEND) == O_APPEND)
    dwDesiredAccess |= FILE_APPEND_DATA;
  else if ((flags & O_RDWR) == O_RDWR)
    dwDesiredAccess |= GENERIC_READ | GENERIC_WRITE;
  else if ((flags & O_WRONLY) == O_WRONLY)
    dwDesiredAccess |= GENERIC_WRITE;
  else
    dwDesiredAccess |= GENERIC_READ;

  if ((flags & O_CREAT) == O_CREAT) {
    if ((flags & O_TRUNC) == O_TRUNC)
      dwCreationDisposition = CREATE_ALWAYS;
    else
      dwCreationDisposition = OPEN_ALWAYS;
  } else
    dwCreationDisposition = OPEN_EXISTING;

  //  if ((flags & O_SPARSE ) == O_SPARSE)
  //    dwFlagsAndAttributes |= FILE_ATTRIBUTE_SPARSE_FILE;

  if ((flags & O_SYNC) == O_SYNC)
    dwFlagsAndAttributes |= FILE_FLAG_WRITE_THROUGH;

  if ((flags & O_DIRECT) == O_DIRECT)
    dwFlagsAndAttributes |= FILE_FLAG_NO_BUFFERING;

  if ((flags & O_ASYNC) == O_ASYNC)
    dwFlagsAndAttributes |= FILE_FLAG_OVERLAPPED;

  if ((flags & O_HIDDEN) == O_HIDDEN)
    dwFlagsAndAttributes = FILE_ATTRIBUTE_HIDDEN;

  HANDLE fd
  = CreateFile(
      path.c_str(),
      dwDesiredAccess,
      FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
      NULL,
      dwCreationDisposition,
      dwFlagsAndAttributes,
      NULL
    );

  if (fd != INVALID_HANDLE_VALUE) {
    if ((flags & O_TRUNC) == O_TRUNC && (flags & O_CREAT) != O_CREAT) {
      SetFilePointer(fd, 0, NULL, FILE_BEGIN);
      SetEndOfFile(fd);
    }

    return new File(fd);
  }

  return NULL;
}

Directory* FileSystem::opendir(const Path& path) {
  HANDLE hDirectory
  = CreateFile(
      path.c_str(),
      FILE_LIST_DIRECTORY,
      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
      NULL,
      OPEN_EXISTING,
      FILE_FLAG_OVERLAPPED | FILE_FLAG_BACKUP_SEMANTICS,
      NULL
    );

  if (hDirectory != INVALID_HANDLE_VALUE)
    return new Directory(hDirectory);
  else
    return NULL;
}

bool FileSystem::realpath(const Path& path, OUT Path& realpath) {
  wchar_t full_path_name[MAX_PATH];

  DWORD full_path_name_len
  = GetFullPathName(path.c_str(), MAX_PATH, full_path_name, NULL);

  if (full_path_name_len > 0) {
    realpath.assign(full_path_name, full_path_name_len);
    return true;
  } else
    return false;
}

bool FileSystem::rename(const Path& from_path, const Path& to_path) {
  return MoveFileEx(
           from_path.c_str(),
           to_path.c_str(),
           MOVEFILE_REPLACE_EXISTING
         ) == TRUE;
}

bool FileSystem::rmdir(const Path& path) {
  return RemoveDirectory(path.c_str()) == TRUE;
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
  WIN32_FILE_ATTRIBUTE_DATA stbuf;
  if (GetFileAttributesEx(path.c_str(), GetFileExInfoStandard, &stbuf))
    return new Stat(stbuf);
  else
    return NULL;
}

bool FileSystem::statvfs(const Path& path, struct statvfs& stbuf) {
  ULARGE_INTEGER uFreeBytesAvailableToCaller,
                 uTotalNumberOfBytes,
                 uTotalNumberOfFreeBytes;

  if (
    GetDiskFreeSpaceEx(
      path.c_str(),
      &uFreeBytesAvailableToCaller,
      &uTotalNumberOfBytes,
      &uTotalNumberOfFreeBytes
    )
  ) {
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);
    stbuf.f_bsize = system_info.dwPageSize;
    stbuf.f_frsize = stbuf.f_bsize;
    stbuf.f_blocks = uTotalNumberOfBytes.QuadPart / stbuf.f_bsize;
    stbuf.f_bfree = uTotalNumberOfFreeBytes.QuadPart / stbuf.f_bsize;
    stbuf.f_bavail = uFreeBytesAvailableToCaller.QuadPart / stbuf.f_bsize;
    stbuf.f_namemax = MAX_PATH;
    return true;
  } else
    return false;
}

bool FileSystem::touch(const Path& path) {
  File* file = creat(path);
  if (file != NULL) {
    File::dec_ref(*file);
    return true;
  } else
    return false;
}

bool FileSystem::unlink(const Path& path) {
  return DeleteFileW(path.c_str()) == TRUE;
}

bool
FileSystem::utime(
  const Path& path,
  const DateTime& atime,
  const DateTime& mtime
) {
  FILETIME ftLastAccessTime = atime;
  FILETIME ftLastWriteTime = mtime;
  return utime(path, NULL, &ftLastAccessTime, &ftLastWriteTime);
}

bool
FileSystem::utime(
  const Path& path,
  const DateTime& atime,
  const DateTime& mtime,
  const DateTime& ctime
) {
  FILETIME ftCreationTime = ctime;
  FILETIME ftLastAccessTime = atime;
  FILETIME ftLastWriteTime = mtime;
  return utime(path, &ftCreationTime, &ftLastAccessTime, &ftLastWriteTime);
}

bool FileSystem::utime(
  const Path& path,
  const FILETIME* ftCreationTime,
  const FILETIME* ftLastAccessTime,
  const FILETIME* ftLastWriteTime
) {
  if (isdir(path)) {
    HANDLE hDir
      = CreateFile(
          path.c_str(),
          GENERIC_WRITE,
          FILE_SHARE_READ|FILE_SHARE_DELETE,
          NULL,
          OPEN_EXISTING,
          FILE_FLAG_BACKUP_SEMANTICS,
          NULL
        );

    if (hDir != INVALID_HANDLE_VALUE) {
      if (SetFileTime(hDir, ftCreationTime, ftLastAccessTime, ftLastWriteTime)) {
        CloseHandle(hDir);
        return true;
      } else
        CloseHandle(hDir);
    }
  } else {
    File* file = open(path, O_WRONLY);
    if (file != NULL) {
      if (SetFileTime(*file, ftCreationTime, ftLastAccessTime, ftLastWriteTime)) {
        File::dec_ref(*file);
        return true;
      } else
        File::dec_ref(*file);
    }
  }

  return false;
}
}
}
