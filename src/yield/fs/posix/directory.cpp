// yield/fs/posix/directory.cpp

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
#include "yield/fs/directory.hpp"

#include <sys/stat.h>

namespace yield {
namespace fs {
Directory::Directory(DIR* dirp, const Path& path)
  : dirp(dirp), path(path)
{ }

Directory::~Directory() {
  close();
}

bool Directory::close() {
  if (dirp != NULL) {
    closedir(dirp);
    dirp = NULL;
    return true;
  } else {
    return false;
  }
}

Directory::Entry* Directory::read() {
  Entry* entry = NULL;
  if (read(entry)) {
    return entry;
  } else {
    return NULL;
  }
}

bool Directory::read(Entry& entry) {
  Entry* p_entry = &entry;
  return read(p_entry);
}

bool Directory::read(Entry*& entry) {
  struct dirent* dirent_;
  while ((dirent_ = readdir(dirp)) != NULL) {
    Entry::Type entry_type;
#if defined(__FreeBSD__) || defined(__linux__) || defined(__MACH__)
    switch (dirent_->d_type) {
    case DT_BLK:
      entry_type = Entry::TYPE_BLK;
      break;
    case DT_CHR:
      entry_type = Entry::TYPE_CHR;
      break;
    case DT_DIR:
      entry_type = Entry::TYPE_DIR;
      break;
    case DT_FIFO:
      entry_type = Entry::TYPE_FIFO;
      break;
    case DT_LNK:
      entry_type = Entry::TYPE_LNK;
      break;
    case DT_REG:
      entry_type = Entry::TYPE_REG;
      break;
    case DT_SOCK:
      entry_type = Entry::TYPE_SOCK;
      break;
    default:
      debug_break();
      entry_type = Entry::TYPE_REG;
      break;
    }
#else
    struct stat stbuf;
    if (stat((path / dirent_->d_name).c_str(), &stbuf) != 0) {
      return false;
    }

    if (S_ISBLK(stbuf.st_mode)) {
      entry_type = Entry::TYPE_BLK;
    } else if (S_ISCHR(stbuf.st_mode)) {
      entry_type = Entry::TYPE_CHR;
    } else if (S_ISDIR(stbuf.st_mode)) {
      entry_type = Entry::TYPE_DIR;
    } else if (S_ISFIFO(stbuf.st_mode)) {
      entry_type = Entry::TYPE_FIFO;
    } else if (S_ISLNK(stbuf.st_mode)) {
      entry_type = Entry::TYPE_LNK;
    } else if (S_ISREG(stbuf.st_mode)) {
      entry_type = Entry::TYPE_REG;
    } else if (S_ISSOCK(stbuf.st_mode)) {
      entry_type = Entry::TYPE_SOCK;
    } else {
      entry_type = Entry::TYPE_REG;
      debug_break();
    }
#endif

    if (entry == NULL) {
      entry = new Entry(dirent_->d_name, entry_type);
    } else {
      entry->set_name(dirent_->d_name);
      entry->set_type(entry_type);
    }

    return true;
  }

  return false;
}

void Directory::rewind() {
  rewinddir(dirp);
}


Directory::Entry::Type Directory::Entry::get_type() const {
  return type;
}

bool Directory::Entry::is_hidden() const {
  return !get_name().empty() && get_name()[0] == '.';
}

bool Directory::Entry::is_special() const {
  return get_name() == Path::CURRENT_DIRECTORY
         ||
         get_name() == Path::PARENT_DIRECTORY;
}
}
}
