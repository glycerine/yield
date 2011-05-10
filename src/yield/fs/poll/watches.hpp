// yield/fs/poll/watches.hpp

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

#ifndef _YIELD_FS_POLL_WATCHES_HPP_
#define _YIELD_FS_POLL_WATCHES_HPP_

#include "yield/assert.hpp"
#include "yield/fs/path.hpp"

#include <map>
#include <utility>

namespace yield {
namespace fs {
namespace poll {
template <class WatchType>
class Watches : private std::map<Path, WatchType*> {
public:
  typedef std::map<Path, WatchType*>::const_iterator const_iterator;

public:
  ~Watches() {
    for (
      iterator watch_i = std::map<Path, WatchType*>::begin();
      watch_i != std::map<Path, WatchType*>::end();
      ++watch_i
    )
      delete watch_i->second;
  }

  const_iterator begin() const {
    return std::map<Path, WatchType*>::begin();
  }

  bool empty() const {
    return std::map<Path, WatchType*>::empty();
  }

  const_iterator end() const {
    return std::map<Path, WatchType*>::end();
  }

  WatchType* erase(const Path& path) {
    iterator watch_i = std::map<Path, WatchType*>::find(path);
    if (watch_i != end()) {
      WatchType* watch = watch_i->second;
      std::map<Path, WatchType*>::erase(watch_i);
      return watch;
    } else
      return NULL;
  }

  WatchType* find(const Path& path) const {
    const_iterator watch_i = std::map<Path, WatchType*>::find(path);
    if (watch_i != end())
      return watch_i->second;
    else
      return NULL;
  }

  void insert(const Path& path, YO_NEW_REF WatchType& watch) {
    if (std::map<Path,WatchType*>::find(path) == end())
      std::map<Path, WatchType*>::insert(std::make_pair(path, &watch));
    else
      debug_break();
  }
};
}
}
}

#endif
