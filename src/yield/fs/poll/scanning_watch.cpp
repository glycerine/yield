// yield/fs/poll/scanning_watch.cpp

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

#include "scanning_watch.hpp"
#include "yield/debug.hpp"

namespace yield {
namespace fs {
namespace poll {
bool ScanningWatch::equals(const Stat& left, const Stat& right) {
  return type(left) == type(right)
         &&
         left.get_mtime() == right.get_mtime()
         &&
         left.get_size() == right.get_size();
}

Directory::Entry::Type ScanningWatch::type(const Stat& stbuf) {
#ifdef _WIN32
  if (stbuf.ISDEV()) {
    return Directory::Entry::TYPE_DEV;
  } else if (stbuf.ISDIR()) {
    return Directory::Entry::TYPE_DIR;
  } else if (stbuf.ISREG()) {
    return Directory::Entry::TYPE_REG;
  }
#else
  if (stbuf.ISBLK()) {
    return Directory::Entry::TYPE_BLK;
  } else if (stbuf.ISCHR()) {
    return Directory::Entry::TYPE_CHR;
  } else if (stbuf.ISDIR()) {
    return Directory::Entry::TYPE_DIR;
  } else if (stbuf.ISFIFO()) {
    return Directory::Entry::TYPE_FIFO;
  } else if (stbuf.ISLNK()) {
    return Directory::Entry::TYPE_LNK;
  } else if (stbuf.ISREG()) {
    return Directory::Entry::TYPE_REG;
  } else if (stbuf.ISSOCK()) {
    return Directory::Entry::TYPE_SOCK;
  }
#endif
  else {
    debug_break();
    return Directory::Entry::TYPE_REG;
  }
}
}
}
}
