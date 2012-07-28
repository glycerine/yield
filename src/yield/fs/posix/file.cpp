// yield/fs/posix/file.cpp

// Copyright (c) 2012 Minor Gordon
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

#include "yield/buffers.hpp"
#include "yield/fs/file.hpp"
#include "yield/fs/stat.hpp"

#include <cstdio>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

namespace yield {
namespace fs {
const uint32_t File::TYPE_ID;

File::Lock::Lock(
  uint64_t len,
  uint64_t start,
  bool exclusive,
  pid_t pid,
  int16_t whence
) {
  flock_.l_len = len;
  flock_.l_pid = pid;
  flock_.l_start = start;
  flock_.l_type = exclusive ? F_WRLCK : F_RDLCK;
  flock_.l_whence = whence;
}

File::Lock::Lock(const struct flock& flock_) : flock_(flock_) {
}

uint64_t File::Lock::get_len() const {
  return flock_.l_len;
}

pid_t File::Lock::get_pid() const {
  return flock_.l_pid;
}

uint64_t File::Lock::get_start() const {
  return flock_.l_start;
}

int16_t File::Lock::get_whence() const {
  return flock_.l_whence;
}

bool File::Lock::is_exclusive() const {
  return flock_.l_type == F_WRLCK;
}


File::Map::Map(
  size_t capacity,
  void* data,
  File& file,
  uint64_t file_offset,
  int flags,
  int prot
)
  : Buffer(capacity, data, capacity),
    file(file.inc_ref()),
    file_offset(file_offset),
    flags(flags),
    prot(prot)
{ }

File::Map::~Map() {
  unmap();
  data_ = NULL;
  File::dec_ref(file);
}

bool File::Map::is_read_only() const {
  return (prot & PROT_WRITE) != PROT_WRITE;
}

bool File::Map::is_shared() const {
  return (flags & MAP_SHARED) == MAP_SHARED;
}

bool File::Map::sync() {
  return sync(data_, capacity());
}

bool File::Map::sync(size_t offset, size_t length) {
  return sync(static_cast<char*>(data_) + offset, length);
}

#pragma GCC diagnostic ignored "-Wold-style-cast"
bool File::Map::sync(void* ptr, size_t length) {
  if (data_ != MAP_FAILED) {
#ifdef __sun
    return msync(static_cast<char*>(ptr), length, MS_SYNC) == 0;
#else
    return msync(ptr, length, MS_SYNC) == 0;
#endif
  } else {
    errno = EBADF;
    return false;
  }
}

bool File::Map::unmap() {
  if (data_ != MAP_FAILED) {
    if (munmap(data_, capacity()) == 0) {
      capacity_ = 0;
      data_ = MAP_FAILED;
      return true;
    } else {
      return false;
    }
  } else {
    errno = EBADF;
    return false;
  }
}
#pragma GCC diagnostic warning "-Wold-style-cast"


File::File(fd_t fd) : fd(fd) {
}

File::~File() {
  close();
}

bool File::close() {
  if (fd >= 0) {
    if (::close(fd) == 0) {
      fd = -1;
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

bool File::datasync() {
#ifdef __linux__
  return fdatasync(*this) != -1;
#else
  return sync();
#endif
}

YO_NEW_REF File* File::dup(fd_t fd) {
  fd_t dup_fd = ::dup(fd);
  if (dup_fd != -1) {
    return new File(dup_fd);
  } else {
    return NULL;
  }
}

YO_NEW_REF File* File::dup(FILE* file) {
  return dup(fileno(file));
}

YO_NEW_REF File::Lock* File::getlk(const Lock& lock) {
  struct flock flock_ = lock;
  if (fcntl(*this, F_GETLK, &flock_) == 0) {
    if (flock_.l_type == F_UNLCK) { // No lock blocking lock
      return NULL;
    } else {
      return new Lock(flock_);
    }
  } else {
    return NULL;
  }
}

#pragma GCC diagnostic ignored "-Wold-style-cast"
YO_NEW_REF File::Map*
File::mmap(
  size_t length,
  off_t offset,
  bool read_only,
  bool shared
) {
  if (length == SIZE_MAX) {
    struct stat stbuf;
    if (::fstat(*this, &stbuf) == 0) {
      length = stbuf.st_size;
    } else {
      return NULL;
    }
  }

  int flags = shared ? MAP_SHARED : MAP_PRIVATE;
  int prot = read_only ? PROT_READ : PROT_READ | PROT_WRITE;

  void* data;
  if (length > 0) {
    data = ::mmap(NULL, length, prot, flags, *this, offset);
    if (data == MAP_FAILED) {
      return NULL;
    }
  } else {
    data = MAP_FAILED;
  }

  return new
         File::Map(
           length,
           data,
           *this,
           offset,
           flags,
           prot
         );
}
#pragma GCC diagnostic warning "-Wold-style-cast"

ssize_t File::pread(Buffer& buffer, off_t offset) {
  if (buffer.get_next_buffer() == NULL) {
    ssize_t pread_ret
    = pread(buffer, buffer.capacity() - buffer.size(), offset);
    if (pread_ret > 0) {
      buffer.put(NULL, static_cast<size_t>(pread_ret));
    }
    return pread_ret;
  } else {
    vector<iovec> iov;
    Buffers::as_read_iovecs(buffer, iov);
    ssize_t preadv_ret = preadv(&iov[0], iov.size(), offset);
    if (preadv_ret > 0) {
      Buffers::put(buffer, NULL, preadv_ret);
    }
    return preadv_ret;
  }
}

ssize_t File::pread(void* buf, size_t buflen, off_t offset) {
  return ::pread(*this, buf, buflen, offset);
}

ssize_t File::preadv(const iovec* iov, int iovlen, off_t offset) {
#ifdef __MACH__
  if (iovlen == 1) {
    return pread(iov[0].iov_base, iov[0].iov_len, offset);
  } else {
    errno = EINVAL;
    return -1;
  }
#else
  return ::preadv(*this, iov, iovlen, offset);
#endif
}

ssize_t File::pwrite(const Buffer& buffer, off_t offset) {
  if (buffer.get_next_buffer() == NULL) {
    return pwrite(buffer, buffer.size(), offset);
  } else {
    vector<iovec> iov;
    Buffers::as_write_iovecs(buffer, iov);
    return pwritev(&iov[0], iov.size(), offset);
  }
}

ssize_t File::pwrite(const void* buf, size_t buflen, off_t offset) {
  return ::pwrite(*this, buf, buflen, offset);
}

ssize_t File::pwritev(const iovec* iov, int iovlen, off_t offset) {
#ifdef __MACH__
  string buf;
  for (int iov_i = 0; iov_i < iovlen; ++iov_i) {
    buf.append(static_cast<char*>(iov[iov_i].iov_base), iov[iov_i].iov_len);
  }
  return pwrite(buf.data(), buf.size(), offset);
#else
  return ::pwritev(*this, iov, iovlen, offset);
#endif
}

ssize_t File::read(Buffer& buffer) {
  return Channel::read(buffer);
}

ssize_t File::read(void* buf, size_t buflen) {
  return ::read(*this, buf, buflen);
}

ssize_t File::readv(const iovec* iov, int iovlen) {
  return ::readv(*this, iov, iovlen);
}

off_t File::seek(off_t offset, uint8_t whence) {
  return lseek(*this, offset, whence);
}

bool File::setlk(const Lock& lock) {
  struct flock flock_ = lock;
  return fcntl(*this, F_SETLK, &flock_) == 0;
}

bool File::setlkw(const Lock& lock) {
  struct flock flock_ = lock;
  return fcntl(*this, F_SETLKW, &flock_) == 0;
}

Stat* File::stat() {
  struct stat stbuf;
  if (fstat(*this, &stbuf) == 0) {
    return new Stat(stbuf);
  } else {
    return NULL;
  }
}

bool File::sync() {
  return fsync(*this) == 0;
}

off_t File::tell() {
  return lseek(*this, 0, SEEK_CUR);
}

bool File::truncate(off_t length) {
  return ::ftruncate(*this, length) == 0;
}

bool File::unlk(const Lock& lock) {
  struct flock flock_ = lock;
  flock_.l_type = F_UNLCK;
  return fcntl(*this, F_SETLK, &flock_) == 0;
}

ssize_t File::write(const Buffer& buffer) {
  return Channel::write(buffer);
}

ssize_t File::write(const void* buf, size_t buflen) {
  return ::write(*this, buf, buflen);
}

ssize_t File::writev(const iovec* iov, int iovlen) {
  return ::writev(*this, iov, iovlen);
}
}
}
