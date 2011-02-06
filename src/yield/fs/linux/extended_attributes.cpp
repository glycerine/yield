// yield/fs/linux/extended_attributes.cpp

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

#include "extended_attributes.hpp"
#include "yield/assert.hpp"

#include <sys/xattr.h>

namespace yield {
namespace fs {
namespace linux {
ExtendedAttributes(fd_t fd)
  : yield::fs::posix::ExtendedAttributes(fd)
{ }

ExtendedAttributes(const Path& path)
  : yield::fs::posix::ExtendedAttributes(path)
{ }

ssize_t
ExtendedAttributes::get(
  fd_t fd,
  const char* name,
  void* value,
  size_t size
) {
  return fgetxattr(fd, name, value, size);
}

ssize_t
ExtendedAttributes::get(
  const Path& path,
  const char* name,
  void* value,
  size_t size
) {
  return getxattr(path.c_str(), name, value, size);
}

ssize_t ExtendedAttributes::list(fd_t fd, char* names, size_t names_len) {
  return flistxattr(fd, names, names_len);
}

ssize_t
ExtendedAttributes::list(
  const Path& path,
  char* names,
  size_t names_len
) {
  return listxattr(path.c_str(), names, names_len);
}

bool ExtendedAttributes::remove(fd_t fd, const char* name) {
  return fremovexattr(fd, name) == 0;
}

bool ExtendedAttributes::remove(const Path& path, const char* name) {
    return removexattr(path.c_str(), name) == 0;
}

bool
ExtendedAttributes::set
(
  fd_t fd,
  const char* name,
  const void* value,
  size_t size,
  int flags
) {
  return fsetxattr(fd, name, value, size, flags) == 0;
}

bool
ExtendedAttributes::set
(
  const Path& path,
  const char* name,
  const void* value,
  size_t size,
  int flags
) {
  return setxattr(path.c_str(), name, value, size, flags) == 0;
}
}
}
}
