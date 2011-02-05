// yield/fs/win32/file.cpp

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

#include "yield/page.hpp"
#include "yield/fs/win32/file.hpp"
#include "yield/fs/win32/stat.hpp"

#include <Windows.h>

namespace yield {
namespace fs {
namespace win32 {
File::File(fd_t fd)
  : fd(fd)
{ }

File::~File() {
  close();
}

bool File::close() {
  if (fd != INVALID_HANDLE_VALUE) {
    if (CloseHandle(fd)) {
      fd = INVALID_HANDLE_VALUE;
      return true;
    } else {
      fd = INVALID_HANDLE_VALUE;
      return false;
    }
  } else
    return false;
}

bool File::datasync() {
  return FlushFileBuffers(*this) != 0;
}

YO_NEW_REF Stat* File::getattr() {
  BY_HANDLE_FILE_INFORMATION by_handle_file_information;
  if (GetFileInformationByHandle(*this, &by_handle_file_information) != 0)
    return new Stat(by_handle_file_information);
  else
    return NULL;
}

//YO_NEW_REF File::Lock* File::getlk(const Lock&) {
//  SetLastError(ERROR_NOT_SUPPORTED);
//  return NULL;
//}

ssize_t File::pread(void* buf, size_t buflen, uint64_t offset) {
  OVERLAPPED overlapped;
  ZeroMemory(&overlapped, sizeof(overlapped));

  overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

  if (overlapped.hEvent != INVALID_HANDLE_VALUE) {
    overlapped.Offset = static_cast<DWORD>(offset);
    overlapped.OffsetHigh = static_cast<DWORD>(offset >> 32);

    DWORD dwNumberOfBytesRead;
    if (ReadFile(*this, buf, buflen, &dwNumberOfBytesRead, &overlapped)) {
      CloseHandle(overlapped.hEvent);
      return static_cast<ssize_t>(dwNumberOfBytesRead);
    } else if (GetLastError() == ERROR_IO_PENDING) {
      DWORD dwNumberOfBytesTransferred;
      if
      (
        GetOverlappedResult
        (
          *this,
          &overlapped,
          &dwNumberOfBytesTransferred,
          TRUE
        )
      ) {
        CloseHandle(overlapped.hEvent);
        return static_cast<ssize_t>(dwNumberOfBytesTransferred);
      }
    }

    CloseHandle(overlapped.hEvent);
  }

  return -1;
}

ssize_t File::preadv(const iovec* iov, int iovlen, uint64_t offset) {
  for (int iov_i = 0; iov_i < iovlen; iov_i++) {
    if (!Page::is_page_aligned(iov[iov_i])) {
      if (iovlen == 1)
        return pread(iov[0].iov_base, iov[0].iov_len, offset);
      else {
        SetLastError(ERROR_INVALID_PARAMETER);
        return -1;
      }
    }
  }

  OVERLAPPED overlapped;
  ZeroMemory(&overlapped, sizeof(overlapped));

  overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

  if (overlapped.hEvent != INVALID_HANDLE_VALUE) {
    overlapped.Offset = static_cast<DWORD>(offset);
    overlapped.OffsetHigh = static_cast<DWORD>(offset >> 32);

    FILE_SEGMENT_ELEMENT* aSegmentArray = new FILE_SEGMENT_ELEMENT[iovlen];
    DWORD dwNumberOfBytesToRead = 0;

    for (int iov_i = 0; iov_i < iovlen; iov_i++) {
      aSegmentArray[iov_i].Buffer = iov[iov_i].iov_base;
      dwNumberOfBytesToRead += iov[iov_i].iov_len;
    }

    if
    (
      ReadFileScatter
      (
        *this,
        aSegmentArray,
        dwNumberOfBytesToRead,
        NULL,
        &overlapped
      )
      ||
      GetLastError() == ERROR_IO_PENDING
    ) {
      // You always have to call GetOverlappedResult with ReadFileScatter
      DWORD dwNumberOfBytesTransferred;
      if
      (
        GetOverlappedResult
        (
          *this,
          &overlapped,
          &dwNumberOfBytesTransferred,
          TRUE
        )
      ) {
        delete [] aSegmentArray;
        CloseHandle(overlapped.hEvent);
        return static_cast<ssize_t>(dwNumberOfBytesTransferred);
      }
    }

    delete [] aSegmentArray;
  }

  CloseHandle(overlapped.hEvent);

  return -1;
}

ssize_t File::pwrite(const void* buf, size_t buflen, uint64_t offset) {
  OVERLAPPED overlapped;
  ZeroMemory(&overlapped, sizeof(overlapped));

  overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

  if (overlapped.hEvent != INVALID_HANDLE_VALUE) {
    overlapped.Offset = static_cast<DWORD>(offset);
    overlapped.OffsetHigh = static_cast<DWORD>(offset >> 32);

    DWORD dwNumberOfBytesWritten;
    if (WriteFile(*this, buf, buflen, &dwNumberOfBytesWritten, &overlapped)) {
      CloseHandle(overlapped.hEvent);
      return static_cast<ssize_t>(dwNumberOfBytesWritten);
    } else if (GetLastError() == ERROR_IO_PENDING) {
      DWORD dwNumberOfBytesTransferred;
      if
      (
        GetOverlappedResult
        (
          *this,
          &overlapped,
          &dwNumberOfBytesTransferred,
          TRUE
        )
      ) {
        CloseHandle(overlapped.hEvent);
        return static_cast<ssize_t>(dwNumberOfBytesTransferred);
      }
    }

    CloseHandle(overlapped.hEvent);
  }

  return -1;
}

ssize_t File::pwritev(const iovec* iov, int iovlen, uint64_t offset) {
  for (int iov_i = 0; iov_i < iovlen; iov_i++) {
    if (!Page::is_page_aligned(iov[iov_i])) {
      if (iovlen == 1)
        return pwrite(iov[0].iov_base, iov[0].iov_len, offset);
      else {
        string buffer;
        for (int iov_i = 0; iov_i < iovlen; iov_i++) {
          buffer.append
          (
            static_cast<const char*>(iov[iov_i].iov_base),
            iov[iov_i].iov_len
          );
        }

        return pwrite(buffer.c_str(), buffer.size(), offset);
      }
    }
  }

  OVERLAPPED overlapped;
  ZeroMemory(&overlapped, sizeof(overlapped));

  overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

  if (overlapped.hEvent != INVALID_HANDLE_VALUE) {
    overlapped.Offset = static_cast<DWORD>(offset);
    overlapped.OffsetHigh = static_cast<DWORD>(offset >> 32);

    FILE_SEGMENT_ELEMENT* aSegmentArray = new FILE_SEGMENT_ELEMENT[iovlen];
    DWORD dwNumberOfBytesToWrite = 0;

    for (int iov_i = 0; iov_i < iovlen; iov_i++) {
      aSegmentArray[iov_i].Buffer = iov[iov_i].iov_base;
      dwNumberOfBytesToWrite += iov[iov_i].iov_len;
    }

    if
    (
      WriteFileGather
      (
        *this,
        aSegmentArray,
        dwNumberOfBytesToWrite,
        NULL,
        &overlapped
      )
    ) {
      CloseHandle(overlapped.hEvent);
      delete [] aSegmentArray;
      return static_cast<ssize_t>(dwNumberOfBytesToWrite);
    } else if (GetLastError() == ERROR_IO_PENDING) {
      DWORD dwNumberOfBytesTransferred;
      if
      (
        GetOverlappedResult
        (
          *this,
          &overlapped,
          &dwNumberOfBytesTransferred,
          TRUE
        )
      ) {
        CloseHandle(overlapped.hEvent);
        delete [] aSegmentArray;
        return static_cast<ssize_t>(dwNumberOfBytesTransferred);
      }
    }

    delete [] aSegmentArray;
  }

  CloseHandle(overlapped.hEvent);

  return -1;
}

ssize_t File::read(void* buf, size_t buflen) {
  return pread(buf, buflen, tell());
}

ssize_t File::readv(const iovec* iov, int iovlen) {
  return preadv(iov, iovlen, tell());
}

uint64_t File::seek(int64_t offset, uint8_t whence) {
  LARGE_INTEGER liOffset;
  liOffset.QuadPart = offset;

  liOffset.LowPart
  = SetFilePointer(*this, liOffset.LowPart, &liOffset.HighPart, whence);

  if (liOffset.LowPart != INVALID_SET_FILE_POINTER)
    return liOffset.QuadPart;
  else
    return static_cast<uint64_t>(-1);
}

bool File::set_blocking_mode(bool) {
  SetLastError(ERROR_NOT_SUPPORTED);
  return false;
}

bool File::setlk(const Lock& lock) {
  return setlk(lock, false);
}

bool File::setlkw(const Lock& lock) {
  return setlk(lock, true);
}

bool File::setlk(const Lock& lock, bool wait) {
  DWORD dwFlags = 0;
  if (lock.is_exclusive()) dwFlags |= LOCKFILE_EXCLUSIVE_LOCK;
  if (!wait) dwFlags |= LOCKFILE_FAIL_IMMEDIATELY;

  OVERLAPPED overlapped;
  ZeroMemory(&overlapped, sizeof(overlapped));

  overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

  if (overlapped.hEvent != INVALID_HANDLE_VALUE) {
    overlapped.Offset = static_cast<DWORD>(lock.get_start());
    overlapped.OffsetHigh = static_cast<DWORD>(lock.get_start() >> 32);

    if
    (
      LockFileEx
      (
        *this,
        dwFlags,
        0,
        static_cast<DWORD>(lock.get_len()),
        static_cast<DWORD>(lock.get_len() >> 32),
        &overlapped
      )
    ) {
      DWORD dwNumberOfBytesTransferred;

      return GetOverlappedResult
             (
               *this,
               &overlapped,
               &dwNumberOfBytesTransferred,
               TRUE
             )  == TRUE;
    }
  }

  return false;
}

bool File::sync() {
  return FlushFileBuffers(*this) != 0;
}

uint64_t File::tell() {
  ULARGE_INTEGER uliFilePointer;
  LONG lFilePointerHigh = 0;
  uliFilePointer.LowPart
  = SetFilePointer(*this, 0, &lFilePointerHigh, FILE_CURRENT);
  uliFilePointer.HighPart = lFilePointerHigh;
  return static_cast<uint64_t>(uliFilePointer.QuadPart);
}

bool File::truncate(uint64_t new_size) {
  if (seek(new_size, SEEK_SET) == new_size)
    return SetEndOfFile(*this) != 0;
  else
    return false;
}

bool File::unlk(const Lock& lock) {
  return UnlockFile
         (
           *this,
           static_cast<DWORD>(lock.get_start()),
           static_cast<DWORD>(lock.get_start() >> 32),
           static_cast<DWORD>(lock.get_len()),
           static_cast<DWORD>(lock.get_len() >> 32)
         ) == TRUE;
}

ssize_t File::write(const void* buf, size_t buflen) {
  return pwrite(buf, buflen, tell());
}

ssize_t File::writev(const iovec* iov, int iovlen) {
  return pwritev(iov, iovlen, tell());
}
}
}
}
