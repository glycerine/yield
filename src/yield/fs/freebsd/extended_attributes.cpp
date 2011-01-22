// yield/fs/freebsd/extended_attributes.cpp

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

#include <sys/extattr.h>


namespace yield {
namespace fs {
namespace freebsd {
ExtendedAttributes(fd_t fd)
  : fd(fd)
{ }

ExtendedAttributes(const Path& path)
  : fd(-1), path(path)
{ }

~ExtendedAttributes() {
  if (fd != -1)
    close(fd);
}

ssize_t ExtendedAttributes::get(const char* name, void* value, size_t size) {
  static int namespaces[]
  = { EXTATTR_NAMESPACE_USER, EXTATTR_NAMESPACE_SYSTEM };

  for (int namespace_i = 0; namespace_i < 2; namespace_i++) {
    ssize_t ret;

    if (fd != -1) {
      ret = extattr_get_fd
            (
              fd,
              namespaces[namespace_i],
              name,
              value,
              size
            );
    } else {
      ret = extattr_get_file
            (
              path.c_str(),
              namespaces[namespace_i],
              name,
              value,
              size
            );
    }

    if (ret != -1)
      return ret;
  }

  return -1;
}

bool ExtendedAttributes::list(OUT vector<string>& out_names) {
  static int namespaces[]
  = { EXTATTR_NAMESPACE_SYSTEM, EXTATTR_NAMESPACE_USER };

  for (int namespace_i = 0; namespace_i < 2; namespace_i++) {
    char* names;
    ssize_t names_len;

    if (fd != -1) {
      names_len = extattr_list_fd(fd, namespaces[namespace_i], NULL, 0);

      if (names_len > 0) {
        names = new char[names_len];

        ssize_t ret
        = extattr_list_fd
          (
            fd,
            namespaces[namespace_i],
            names,
            names_len
          );

        debug_assert_eq(ret, names_len);
      } else
        continue;
    } else {
      names_len
      = extattr_list_file
        (
          path.c_str(),
          namespaces[namespace_i],
          NULL,
          0
        );

      if (names_len > 0) {
        names = new char[names_len];

        ssize_t ret
        = extattr_list_file
          (
            path.c_str(),
            namespaces[namespace_i],
            names,
            names_len
          );

        debug_assert_eq(ret, names_len);
      } else
        continue;
    }

    char* name = names;
    do {
      size_t name_len = name[0];
      out_names.push_back(string(name, name_len));
      name += name_len + 1;
    } while (static_cast<size_t>(name - names) < names_len);
    delete [] names;
  }

  return true;
}

bool ExtendedAttributes::remove(const char* name) {
  if (fd != -1)
    return extattr_delete_fd(fd, EXTATTR_NAMESPACE_USER, name) != -1;
  else {
    return extattr_delete_file
           (
             path.c_str(),
             EXTATTR_NAMESPACE_USER,
             name
           ) != -1;
  }
}

bool
ExtendedAttributes::set
(
  const char* name,
  const void* value,
  size_t size,
  int
) {
  if (fd != -1) {
    return extattr_set_fd
           (
             fd,
             EXTATTR_NAMESPACE_USER,
             name,
             value,
             size
           ) != -1;
  } else {
    return extattr_set_file
           (
             path.c_str(),
             EXTATTR_NAMESPACE_USER,
             name,
             value,
             size
           ) != -1;
  }
}
}
}
}
