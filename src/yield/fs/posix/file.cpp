// yield/fs/posix/file.cpp

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

#if defined(__FreeBSD__)
#include "../freebsd/extended_attributes.hpp"
#elif defined(__linux__)
#include "../linux/extended_attributes.hpp"
#elif defined(__MACH__)
#include "../darwin/extended_attributes.hpp"
#endif
#include "yield/buffers.hpp"
#include "yield/fs/posix/file.hpp"
#include "yield/fs/posix/stat.hpp"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

namespace yield {
namespace fs {
namespace posix {
File::Map::Map(
  size_t capacity,
  void* data,
  File& file,
  uint64_t file_offset,
  int flags,
  int prot
)
  : Buffer(capacity, data),
    file(file.inc_ref()),
    file_offset(file_offset),
    flags(flags),
    prot(prot)
{ }

File::Map::~Map() {
  unmap();
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
    } else
      return false;
  } else {
    errno = EBADF;
    return false;
  }
}


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
    } else
      return false;
  } else
    return false;
}

bool File::datasync() {
  return sync();
}

File::Lock* File::getlk(const Lock& lock) {
  flock flock_ = lock;
  if (fcntl(*this, F_GETLK, &flock_) == 0) {
    if (flock_.l_type == F_UNLCK)   // No lock blocking lock
      return NULL;
    else
      return new Lock(flock_);
  } else
    return NULL;
}

YO_NEW_REF File::Map*
File::mmap(
  size_t length,
  uint64_t offset,
  bool read_only,
  bool shared
) {
  if (length == SIZE_MAX) {
    struct stat stbuf;
    if (::fstat(*this, &stbuf) == 0)
      length = stbuf.st_size;
    else
      return NULL;
  }

  int flags = shared ? MAP_SHARED : MAP_PRIVATE;
  int prot = read_only ? PROT_READ : PROT_READ | PROT_WRITE;

  void* data;
  if (length > 0) {
    data = ::mmap(NULL, length, prot, flags, *this, offset);
    if (data == MAP_FAILED)
      return NULL;
  } else
    data = MAP_FAILED;

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

YO_NEW_REF ExtendedAttributes* File::openxattrs() {
  errno = ENOTSUP;
  return NULL;
}

ssize_t File::pread(Buffer& buffer, uint64_t offset) {
  if (buffer.get_next_buffer() == NULL) {
    ssize_t pread_ret
      = pread(buffer, buffer.capacity() - buffer.size(), offset);
    if (pread_ret > 0)
      buffer.put(NULL, static_cast<size_t>(pread_ret));
    return pread_ret;
  } else {
    vector<iovec> iov;
    Buffers::as_read_iovecs(buffer, iov);
    ssize_t preadv_ret = preadv(&iov[0], iov.size(), offset);
    if (preadv_ret > 0)
      Buffers::put(buffer, NULL, preadv_ret);
    return preadv_ret;
  }
}

ssize_t File::pread(void* buf, size_t buflen, uint64_t offset) {
  return ::pread(*this, buf, buflen, offset);
}

ssize_t File::preadv(const iovec* iov, int iovlen, uint64_t offset) {
  return ::preadv(*this, iov, iovlen, offset);
}

ssize_t File::pwrite(const Buffer& buffer, uint64_t offset) {
  if (buffer.get_next_buffer() == NULL)
    return pwrite(buffer, buffer.size(), offset);
  else {
    vector<iovec> iov;
    Buffers::as_write_iovecs(buffer, iov);
    return pwritev(&iov[0], iov.size(), offset);
  }
}

ssize_t File::pwrite(const void* buf, size_t buflen, uint64_t offset) {
  return ::pwrite(*this, buf, buflen, offset);
}

ssize_t File::pwritev(const iovec* iov, int iovlen, uint64_t offset) {
  return ::pwritev(*this, iov, iovlen, offset);
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

uint64_t File::seek(int64_t offset, uint8_t whence) {
  return lseek(*this, offset, whence);
}

bool File::setlk(const Lock& lock) {
  flock flock_ = lock;
  return fcntl(*this, F_SETLK, &flock_) == 0;
}

bool File::setlkw(const Lock& lock) {
  flock flock_ = lock;
  return fcntl(*this, F_SETLKW, &flock_) == 0;
}

Stat* File::stat() {
  struct stat stbuf;
  if (fstat(*this, &stbuf) == 0)
    return new Stat(stbuf);
  else
    return NULL;
}

bool File::sync() {
  return fsync(*this) == 0;
}

uint64_t File::tell() {
  return lseek(*this, 0, SEEK_CUR);
}

bool File::truncate(uint64_t new_size) {
  return ::ftruncate(*this, new_size) == 0;
}

bool File::unlk(const Lock& lock) {
  flock flock_ = lock;
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
}
