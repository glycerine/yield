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

#include "yield/object.hpp"
#include "yield/i18n/tstring.hpp"

#include <ostream> // For std::ostream
#include <utility> // For std::make_pair, std::pair

namespace yield {
namespace fs {
class Path : public yield::i18n::tstring {
public:
  const static Path CURRENT_DIRECTORY;
  const static Path PARENT_DIRECTORY;
  const static tstring::value_type SEPARATOR;

public:
  Path() { }
  Path(char path) : tstring(path) { }
  Path(const char* path) : tstring(path) { }
  Path(const char* path, size_t path_len) : tstring(path, path_len) { }
  Path(const string& path) : tstring(path) { }
  Path(const tstring& path) : tstring(path) { }
  Path(wchar_t path) : tstring(path) { }
  Path(const wchar_t* path) : tstring(path) { }
  Path(const wchar_t* path, size_t path_len) : tstring(path, path_len) { }
  Path(const std::wstring& path) : tstring(path) { }

  Path parent_path() const {
    if (size() > 1 || operator[](0) != SEPARATOR) {
      vector<Path> parts;
      split(parts);
      if (parts.size() > 1)
        return parts[parts.size() - 2];
      else
        return Path(SEPARATOR);
    } else
      return Path(SEPARATOR);
  }

  std::pair<Path, Path> split() const {
    size_type sep = find_last_of(SEPARATOR);
    if (sep != npos)
      return make_pair(substr(0, sep), substr(sep + 1));
    else
      return make_pair(Path(), *this);
  }

  template <class ContainerType>
  void split(OUT ContainerType& parts) const {
    size_t last_sep = find_first_not_of(SEPARATOR, 0);
    size_t next_sep = find_first_of(SEPARATOR, last_sep);

    while (next_sep != tstring::npos || last_sep != tstring::npos) {
      parts.push_back(Path(substr(last_sep, next_sep - last_sep)));
      last_sep = find_first_not_of(SEPARATOR, next_sep);
      next_sep = find_first_of(SEPARATOR, last_sep);
    }
  }

  std::pair<Path, Path> splitext() const;
};


inline Path operator/(const Path& left, const Path& right) {
  if (left.empty())
    return right;
  else if (right.empty())
    return left;
  else {
    yield::i18n::tstring combined(left);

    if
    (
      left[left.size() - 1] != Path::SEPARATOR
      &&
      right[0] != Path::SEPARATOR
    )
      combined += Path::SEPARATOR;

    combined.append(right);

    return Path(combined);
  }
}

std::ostream& operator<<(std::ostream&, const Path&);
}
}

#endif
