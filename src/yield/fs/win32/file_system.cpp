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


#include "directory.hpp"
#include "file.hpp"
#include "memory_mapped_file.hpp"
#include "named_pipe.hpp"
#include "stat.hpp"
#include "file_system.hpp"
#include "yield/assert.hpp"
#include "yield/fs/path.hpp"

#include <fcntl.h> // For O_*
#include <Windows.h>


namespace yield {
namespace fs {
namespace win32 {
bool FileSystem::access(const Path&, int) {
  return true;
}

yield::fs::Stat* FileSystem::getattr(const Path& path) {
  WIN32_FILE_ATTRIBUTE_DATA stbuf;
  if (GetFileAttributesEx(path.c_str(), GetFileExInfoStandard, &stbuf))
    return new Stat(stbuf);
  else
    return NULL;
}

bool FileSystem::isdir(const Path& path) {
  DWORD dwAttributes = GetFileAttributes(path.c_str());
  return dwAttributes != INVALID_FILE_ATTRIBUTES
         &&
         (dwAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool FileSystem::isfile(const Path& path) {
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
  return CreateHardLink
         (
           new_path.c_str(),
           old_path.c_str(),
           NULL
         ) == TRUE;
}

bool FileSystem::mkdir(const Path& path, mode_t mode) {
  return CreateDirectory(path.c_str(), NULL) == TRUE;
}

yield::fs::File*
FileSystem::mkfifo
(
  const Path& path,
  uint32_t flags,
  mode_t
) {
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
    = CreateNamedPipe
      (
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

void*
FileSystem::mmap
(
  size_t length,
  int prot,
  int flags,
  fd_t fd,
  uint64_t offset,
  void*& out_hFileMapping
) {
  DWORD flProtect;
  // The order of checks is important (= priority)
  if ((prot & PROT_NONE) == PROT_NONE)
    flProtect = PAGE_NOACCESS;
  else if ((prot & PROT_EXEC) == PROT_EXEC) {
    if ((prot & PROT_WRITE) == PROT_WRITE)
      flProtect = PAGE_EXECUTE_READWRITE;
    else
      flProtect = PAGE_EXECUTE_READ;
  } else if ((prot & PROT_WRITE) == PROT_WRITE)
    flProtect = PAGE_READWRITE;
  else if ((prot & PROT_READ) == PROT_READ)
    flProtect = PAGE_READONLY;
  else
    flProtect = PAGE_READWRITE;

  ULARGE_INTEGER uliSize;
  uliSize.QuadPart = length;

  out_hFileMapping
  = CreateFileMapping
    (
      fd,
      NULL,
      flProtect,
      uliSize.HighPart,
      uliSize.LowPart,
      NULL
    );

  if (out_hFileMapping != NULL) {   // not INVALID_HANDLE_VALUE
    DWORD dwDesiredAccess;
    if ((flags & MAP_PRIVATE) == MAP_SHARED)
      dwDesiredAccess = FILE_MAP_COPY;
    else {
      if ((prot & PROT_WRITE) == PROT_WRITE)
        dwDesiredAccess = FILE_MAP_WRITE;
      else
        dwDesiredAccess = FILE_MAP_READ;
    }

    if ((prot & PROT_EXEC) == PROT_EXEC)
      dwDesiredAccess |= FILE_MAP_EXECUTE;

    ULARGE_INTEGER uliOffset;
    uliOffset.QuadPart = offset;

    void* lpMapAddress
    = MapViewOfFile
      (
        out_hFileMapping,
        dwDesiredAccess,
        uliOffset.HighPart,
        uliOffset.LowPart,
        length
      );

    if (lpMapAddress != NULL)
      return lpMapAddress;
    else
      CloseHandle(out_hFileMapping);
  }

  return reinterpret_cast<void*>(-1);
}

yield::fs::MemoryMappedFile*
FileSystem::mmap
(
  yield::fs::File& file,
  void* addr,
  size_t length,
  int prot,
  int flags,
  uint64_t offset
) {
  if (addr == NULL) {
    if (length == MMAP_LENGTH_WHOLE_FILE) {
      ULARGE_INTEGER uliFileSize;
      uliFileSize.LowPart
      = GetFileSize(static_cast<File&>(file), &uliFileSize.HighPart);
      if (uliFileSize.LowPart != INVALID_FILE_SIZE)
        length = static_cast<size_t>(uliFileSize.QuadPart);
      else
        length = 0;
    }

    HANDLE hFileMapping;
    LPVOID lpMapAddress;

    if (length > 0) {   // Can't CreateFileMapping on an empty file
      // just return an "empty" MemoryMappedFile (lpMapAddress=NULL)
      lpMapAddress    // that can be resized
      = mmap
        (
          length,
          prot,
          flags,
          static_cast<File&>(file),
          offset,
          hFileMapping
        );

      if (lpMapAddress == reinterpret_cast<LPVOID>(-1))
        return NULL;
    } else {
      hFileMapping = NULL;
      lpMapAddress = reinterpret_cast<LPVOID>(-1);
    }

    return new MemoryMappedFile
           (
             length,
             lpMapAddress,
             static_cast<File&>(file),
             flags,
             hFileMapping,
             offset,
             prot
           );
  } else {
    SetLastError(ERROR_INVALID_PARAMETER);
    return NULL;
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

  //  if ( ( flags & O_SPARSE ) == O_SPARSE )
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
  = CreateFile
    (
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

yield::fs::Directory* FileSystem::opendir(const Path& path) {
  HANDLE hDirectory
  = CreateFile
    (
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

YO_NEW_REF ExtendedAttributes* FileSystem::openxattrs(const Path&) {
  SetLastError(ERROR_NOT_SUPPORTED);
  return NULL;
}

bool FileSystem::readlink(const Path&, Path&) {
  SetLastError(ERROR_NOT_SUPPORTED);
  return false;
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
  return MoveFileEx
         (
           from_path.c_str(),
           to_path.c_str(),
           MOVEFILE_REPLACE_EXISTING
         ) == TRUE;
}

bool FileSystem::rmdir(const Path& path) {
  return RemoveDirectory(path.c_str()) == TRUE;
}

bool FileSystem::setattr(const Path& path, const yield::fs::Stat& stbuf) {
  if
  (
    stbuf.has_blksize()
    ||
    stbuf.has_blocks()
    ||
    stbuf.has_dev()
    ||
    stbuf.has_gid()
    ||
    stbuf.has_ino()
    ||
    stbuf.has_mode()
    ||
    stbuf.has_nlink()
    ||
    stbuf.has_rdev()
    ||
    stbuf.has_size()
    ||
    stbuf.has_uid()
  ) {
    SetLastError(ERROR_NOT_SUPPORTED);
    return false;
  }

  bool have_setattr = false;

  if (stbuf.has_atime() || stbuf.has_ctime() || stbuf.has_mtime()) {
    File* file
    = static_cast<File*>
      (
        open
        (
          path,
          O_WRONLY,
          FILE_MODE_DEFAULT,
          OPEN_ATTRIBUTES_DEFAULT
        )
      );

    if (file != NULL) {
      FILETIME ftCreationTime = stbuf.get_ctime(),
               ftLastAccessTime = stbuf.get_atime(),
               ftLastWriteTime = stbuf.get_mtime();

      if
      (
        SetFileTime
        (
          *file,
          stbuf.has_ctime() ? &ftCreationTime : NULL,
          stbuf.has_atime() ? &ftLastAccessTime : NULL,
          stbuf.has_mtime() ? &ftLastWriteTime : NULL
        )
      ) {
        have_setattr = true;
        File::dec_ref(*file);
      } else {
        debug_assert_false(have_setattr);
        File::dec_ref(*file);
      }
    } else {
      debug_assert_false(have_setattr);
      return false;
    }
  }

  if (stbuf.has_attributes()) {
    if
    (
      SetFileAttributes
      (
        path.c_str(),
        stbuf.get_attributes()
      )
    )
      have_setattr = true;
    else {
      debug_assert_false(have_setattr);
      return false;
    }
  }

  return true;
}

bool FileSystem::statvfs(const Path& path, struct statvfs& stbuf) {
  ULARGE_INTEGER uFreeBytesAvailableToCaller,
                 uTotalNumberOfBytes,
                 uTotalNumberOfFreeBytes;

  if
  (
    GetDiskFreeSpaceEx
    (
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

bool FileSystem::symlink(const Path&, const Path&) {
  SetLastError(ERROR_NOT_SUPPORTED);
  return false;
}

bool FileSystem::unlink(const Path& path) {
  return DeleteFileW(path.c_str()) == TRUE;
}

bool FileSystem::truncate(const Path& path, uint64_t new_size) {
  yield::fs::File* file
  = open(path, O_CREAT | O_WRONLY, FILE_MODE_DEFAULT, 0);
  if (file != NULL) {
    file->truncate(new_size);
    File::dec_ref(*file);
    return true;
  } else
    return false;
}
}
}
}
