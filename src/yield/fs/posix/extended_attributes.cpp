// yield/fs/posix/extended_attributes.cpp

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

#include "yield/assert.hpp"
#include "yield/fs/posix/extended_attributes.hpp"

namespace yield {
namespace fs {
namespace posix {
ExtendedAttributes::ExtendedAttributes(fd_t fd)
  : fd(fd)
{ }

ExtendedAttributes::ExtendedAttributes(const Path& path)
  : fd(-1), path(path)
{ }

ExtendedAttributes::~ExtendedAttributes() {
  if (fd != -1)
    ::close(fd);
}

bool ExtendedAttributes::get(const char* name, string& out_value) {
  ssize_t estimated_value_len = get(name, NULL, 0);
  if (estimated_value_len != -1) {
    char* value = new char[estimated_value_len];
    ssize_t value_len = get(name, value, estimated_value_len);
    debug_assert_eq(value_len, estimated_value_len);
    out_value.assign(value, value_len);
    delete [] value;
    return true;
  } else
    return false;
}

ssize_t ExtendedAttributes::get(const char* name, void* value, size_t size) {
  if (fd != -1)
    return get(fd, name, value, size);
  else
    return get(path, name, value, size);
}

bool ExtendedAttributes::list(vector<string>& out_names) {
  size_t names_len;
  if (fd != -1)
    names_len = list(fd, NULL, 0);
  else
    names_len = list(path, NULL, 0);

  if (names_len > 0) {
    char* names = new char[names_len];

    if (fd != -1)
      list(fd, names, names_len);
    else
      list(path.c_str(), names, names_len);

    char* name = names;
    do {
      size_t name_len = strlen(name);
      out_names.push_back(string(name, name_len));
      name += name_len;
    } while (static_cast<size_t>(name - names) < names_len);
    delete [] names;
  }
  return true;
}

bool ExtendedAttributes::remove(const char* name) {
  if (fd != -1)
    return remove(fd, name);
  else
    return remove(path, name);
}

bool ExtendedAttributes::set(const char* name, const char* value, int flags) {
  return set(name, value, strlen(value), flags);
}

bool
ExtendedAttributes::set
(
  const char* name,
  const string& value,
  int flags
) {
  return set(name, value.c_str(), value.size(), flags);
}

bool
ExtendedAttributes::set
(
  const char* name,
  const void* value,
  size_t size,
  int flags
) {
  if (fd != -1)
    return set(fd, name, value, size, flags);
  else
    return set(path, name, value, size, flags);
}
}
}
}
