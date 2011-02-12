// yield/fs/posix/directory.hpp

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

#ifndef _YIELD_FS_POSIX_DIRECTORY_HPP_
#define _YIELD_FS_POSIX_DIRECTORY_HPP_

#include "yield/fs/path.hpp"

#include <dirent.h>

namespace yield {
namespace fs {
namespace posix {
class Directory : public Object {
public:
  class Entry : public Object {
  public:
    typedef uint8_t Type;
    const static Type TYPE_BLK = 1;
    const static Type TYPE_CHR = 2;
    const static Type TYPE_DIR = 4;
    const static Type TYPE_FIFO = 8;
    const static Type TYPE_LNK = 16;
    const static Type TYPE_REG = 32;
    const static Type TYPE_SOCK = 64;

  public:
    Entry(const Path& name, Type type)
      : name(name), type(type)
    { }

  public:
    const Path& get_name() const {
      return name;
    }

    Type get_type() const {
      return type;
    }

    bool ISCHR() const {
      return get_type() == TYPE_CHR;
    }

    bool ISDIR() const {
      return get_type() == TYPE_DIR;
    }

    bool ISFIFO() const {
      return get_type() == TYPE_FIFO;
    }

    bool ISLNK() const {
      return get_type() == TYPE_LNK;
    }

    bool ISREG() const {
      return get_type() == TYPE_REG;
    }

    bool ISSOCK() const {
      return get_type() == TYPE_SOCK;
    }

    bool is_hidden() const;
    bool is_special() const;

    void set_name(const Path& name) {
      this->name = name;
    }

    void set_type(Type type) {
      this->type = type;
    }

  private:
    Path name;
    Type type;
  };

public:
  Directory(DIR* dirp, const Path& path);
  virtual ~Directory();

  // Object
  Directory& inc_ref() {
    return Object::inc_ref(*this);
  }

  // Directory
  bool close();
  YO_NEW_REF Entry* read();
  bool read(OUT Entry&);
  void rewind();

protected:
  Directory(DIR* dirp);

  operator DIR* () {
    return dirp;
  }

  const Path& get_path() const {
    return path;
  }

  virtual bool read(OUT Entry*&);

private:
  DIR* dirp;
  Path path;
};
}
}
}

#endif
