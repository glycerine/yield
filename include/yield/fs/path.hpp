// yield/fs/path.hpp

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

#ifndef _YIELD_FS_PATH_HPP_
#define _YIELD_FS_PATH_HPP_

#include "yield/i18n/tstring.hpp"

#include <ostream> // For std::ostream
#include <utility> // For std::make_pair, std::pair

namespace yield {
namespace fs {
/**
  A platform-independent file system path class.
*/
class Path : public ::yield::i18n::tstring {
public:
  /**
    The platform-specific path of the current directory (usually ".").
  */
  const static Path CURRENT_DIRECTORY;

  /**
    The platform-specific path of the parent directory (usually "..").
  */
  const static Path PARENT_DIRECTORY;

  /**
    The platform-specific path separator ('/' on POSIX, '\\' on Win32).
  */
  const static tstring::value_type SEPARATOR;

public:
  /**
    Construct an empty Path.
  */
  Path() { }

  /**
    Construct a Path from a single character.
    @param path new contents of the Path
  */
  Path(char path) : tstring(path) { }

  /**
    Construct a Path from a C string.
    @param path new contents of the Path, copied
  */
  Path(const char* path) : tstring(path) { }

  /**
    Construct a Path from a string and length.
    @param path new contents of the Path, copied
    @param path_len length of path
  */
  Path(const char* path, size_t path_len) : tstring(path, path_len) { }

  /**
    Construct a Path from a C++ string.
    @param path new contents of the Path, copied
  */
  Path(const string& path) : tstring(path) { }

  /**
    Construct a Path from a tstring.
    @param path new contents of the Path, copied
  */
  Path(const tstring& path) : tstring(path) { }

#ifdef _WIN32
  /**
    Construct a Path from a single wide character.
    @param path new contents of the Path
  */
  Path(wchar_t path) : tstring(path) { }

  /**
    Construct a Path from a wide C string.
    @param path new contents of the Path, copied
  */
  Path(const wchar_t* path) : tstring(path) { }

  /**
    Construct a Path from a wide string and length.
    @param path new contents of the Path, copied
    @param path_len length of path
  */
  Path(const wchar_t* path, size_t path_len) : tstring(path, path_len) { }

  /**
    Construct a Path from a C++ wstring.
    @param path new contents of the Path, copied
  */
  Path(const std::wstring& path) : tstring(path) { }
#endif

  /**
    Construct the parent path of this path, usually a directory path.
    @return the parem Path of this path
  */
  Path parent_path() const {
    if (size() > 1 || operator[](0) != SEPARATOR) {
      vector<Path> components;
      split(components);
      if (components.size() > 1)
        return components[components.size() - 2];
      else
        return Path(SEPARATOR);
    } else
      return Path(SEPARATOR);
  }

  /**
    Split the path into head and tail components e.g.,
      "/usr/local/bin" is split into ("/usr/local", "bin")
    @return pair of head and tail components
  */
  std::pair<Path, Path> split() const {
    size_type sep = find_last_of(SEPARATOR);
    if (sep != npos)
      return make_pair(substr(0, sep), substr(sep + 1));
    else
      return make_pair(Path(), *this);
  }

  /**
    Split the path into components e.g.,
      "/usr/local/bin" is split into ("usr", "local", "bin")
    @param[out] components sequence of the path's components
  */
  template <class ContainerType>
  void split(ContainerType& components) const {
    size_t last_sep = find_first_not_of(SEPARATOR, 0);
    size_t next_sep = find_first_of(SEPARATOR, last_sep);

    while (next_sep != tstring::npos || last_sep != tstring::npos) {
      components.push_back(Path(substr(last_sep, next_sep - last_sep)));
      last_sep = find_first_not_of(SEPARATOR, next_sep);
      next_sep = find_first_of(SEPARATOR, last_sep);
    }
  }

  /**
    Split a path into a stem and its extension e.g.,
      file.txt is split into ("file", "txt")
    @return a pair with the path's stem and its file extension
  */
  std::pair<Path, Path> splitext() const;
};


/**
  Join two paths with a separator e.g.,
    Path("usr") / "local" results in Path("usr/local") on POSIX systems.
  Only adds a separator if necessary.
  @param left left Path to join
  @param right right Path to join
  @return the left and right Paths joined by a separator
*/
inline Path operator/(const Path& left, const Path& right) {
  if (left.empty())
    return right;
  else if (right.empty())
    return left;
  else {
    yield::i18n::tstring combined(left);

    if (
      left[left.size() - 1] != Path::SEPARATOR
      &&
      right[0] != Path::SEPARATOR
    )
      combined += Path::SEPARATOR;

    combined.append(right);

    return Path(combined);
  }
}

/**
  Print a Path.
  @param os ostream to print the path to
  @param path Path to print
  @return os
*/
std::ostream& operator<<(std::ostream& os, const Path& path);
}
}

#endif
