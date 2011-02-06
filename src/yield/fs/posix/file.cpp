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

#include "yield/fs/posix/file.hpp"
#include "yield/fs/posix/stat.hpp"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>


namespace yield {
namespace fs {
namespace posix {
File::File(fd_t fd)
  : fd(fd)
{ }

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

Stat* File::getattr() {
  struct stat stbuf;
  if (fstat(*this, &stbuf) == 0)
    return new Stat(stbuf);
  else
    return NULL;
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

YO_NEW_REF ExtendedAttributes* File::openxattrs() {
  errno = ENOTSUP;
  return NULL;
}

ssize_t File::pread(void* buf, size_t buflen, uint64_t offset) {
  return ::pread(*this, buf, buflen, offset);
}

ssize_t File::preadv(const iovec* iov, int iovlen, uint64_t offset) {
  return ::preadv(*this, iov, iovlen, offset);
}

ssize_t File::pwrite(const void* buf, size_t buflen, uint64_t offset) {
  return ::pwrite(*this, buf, buflen, offset);
}

ssize_t File::pwritev(const iovec* iov, int iovlen, uint64_t offset) {
  return ::pwritev(*this, iov, iovlen, offset);
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

bool File::set_blocking_mode(bool blocking_mode) {
  int current_fcntl_flags = fcntl(*this, F_GETFL, 0);
  if (blocking_mode) {
    if ((current_fcntl_flags & O_NONBLOCK) == O_NONBLOCK)
      return fcntl(*this, F_SETFL, current_fcntl_flags ^ O_NONBLOCK) == 0;
    else
      return true;
  } else
    return fcntl(*this, F_SETFL, current_fcntl_flags | O_NONBLOCK) == 0;
}

bool File::setlk(const Lock& lock) {
  flock flock_ = lock;
  return fcntl(*this, F_SETLK, &flock_) == 0;
}

bool File::setlkw(const Lock& lock) {
  flock flock_ = lock;
  return fcntl(*this, F_SETLKW, &flock_) == 0;
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

ssize_t File::write(const void* buf, size_t buflen) {
  return ::write(*this, buf, buflen);
}

ssize_t File::writev(const iovec* iov, int iovlen) {
  return ::writev(*this, iov, iovlen);
}
}
}
}
