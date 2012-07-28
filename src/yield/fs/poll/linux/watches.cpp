// yield/fs/poll/linux/watches.cpp

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

#include "watches.hpp"
#include "yield/debug.hpp"

namespace yield {
namespace fs {
namespace poll {
namespace linux {
using std::make_pair;
using std::map;

Watches::~Watches() {
  for (iterator watch_i = begin(); watch_i != end(); ++watch_i) {
    delete watch_i->second;
  }
}

Watch* Watches::erase(const Path& path) {
  for (iterator watch_i = begin(); watch_i != end(); ++watch_i) {
    if (watch_i->second->get_path() == path) {
      Watch* watch = watch_i->second;
      map<int, Watch*>::erase(watch_i);
      return watch;
    }
  }

  return NULL;
}

Watch* Watches::find(const Path& path) {
  for (iterator watch_i = begin(); watch_i != end(); ++watch_i) {
    if (watch_i->second->get_path() == path) {
      return watch_i->second;
    }
  }

  return NULL;
}

Watch* Watches::find(int wd) {
  iterator watch_i = map<int, Watch*>::find(wd);
  if (watch_i != end()) {
    return watch_i->second;
  } else {
    return NULL;
  }
}

void Watches::insert(Watch& watch) {
  iterator watch_i = map<int, Watch*>::find(watch.get_wd());
  debug_assert_eq(watch_i, end());
  map<int, Watch*>::insert(make_pair(watch.get_wd(), &watch));
}
}
}
}
}
