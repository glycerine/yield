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


#include "directory.hpp"


namespace yield {
namespace fs {
namespace posix {
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
  } else
    return false;
}

yield::fs::Directory::Entry* Directory::read(Entry::Type types) {
  Entry* entry = new Entry;
  if (read(*entry, types))
    return entry;
  else {
    delete entry;
    return NULL;
  }
}

bool Directory::read
(
  OUT yield::fs::Directory::Entry& entry,
  Entry::Type types
) {
  dirent* dirent_;
  while ((dirent_ = readdir(dirp)) != NULL) {
    static_cast<Entry&>(entry) = *dirent_;   // To set entry.name

    struct stat stbuf;
    if (stat((path / entry.get_name()).c_str(), &stbuf) != -1) {
      static_cast<Entry&>(entry) = stbuf;
      if ((entry.get_type() & types) == entry.get_type())
        return true;
    }
  }

  return false;
}

void Directory::rewind() {
  rewinddir(dirp);
}


bool Directory::Entry::is_hidden() const {
  return !get_name().empty() && get_name()[0] == '.';
}

bool Directory::Entry::is_special() const {
  return get_name() == Path::CURRENT_DIRECTORY
         ||
         get_name() == Path::PARENT_DIRECTORY;
}

Directory::Entry& Directory::Entry::operator=(const dirent& dirent_) {
  this->name = Path(dirent_.d_name);
  return *this;
}

Directory::Entry& Directory::Entry::operator=(const struct stat& stbuf) {
  this->stbuf = stbuf;
  return *this;
}
}
}
}
