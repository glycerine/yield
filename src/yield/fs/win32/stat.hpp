// yield/fs/win32/stat.hpp

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


#ifndef _YIELD_FS_WIN32_STAT_HPP_
#define _YIELD_FS_WIN32_STAT_HPP_


#include "yield/fs/stat.hpp"


struct _BY_HANDLE_FILE_INFORMATION;
typedef _BY_HANDLE_FILE_INFORMATION BY_HANDLE_FILE_INFORMATION;
struct _WIN32_FILE_ATTRIBUTE_DATA;
typedef struct _WIN32_FILE_ATTRIBUTE_DATA WIN32_FILE_ATTRIBUTE_DATA;
struct _WIN32_FIND_DATAW;
typedef _WIN32_FIND_DATAW WIN32_FIND_DATAW;
typedef WIN32_FIND_DATAW WIN32_FIND_DATA;


namespace yield {
namespace fs {
namespace win32 {
class Stat : public yield::fs::Stat {
public:
  Stat();
  Stat(const BY_HANDLE_FILE_INFORMATION&);
  Stat(const WIN32_FILE_ATTRIBUTE_DATA&);
  Stat(const WIN32_FIND_DATA&);

  operator BY_HANDLE_FILE_INFORMATION() const;
  operator WIN32_FILE_ATTRIBUTE_DATA() const;
  operator WIN32_FIND_DATA() const;

  Stat& operator=(const BY_HANDLE_FILE_INFORMATION&);
  Stat& operator=(const WIN32_FILE_ATTRIBUTE_DATA&);
  Stat& operator=(const WIN32_FIND_DATA&);

  // Stat
  const DateTime& get_atime() const {
    return atime;
  }
  uint32_t get_attributes() const {
    return attributes;
  }
  const DateTime& get_ctime() const {
    return ctime;
  }
  const DateTime& get_mtime() const {
    return mtime;
  }
  int16_t get_nlink() const {
    return nlink;
  }
  uint64_t get_size() const {
    return size;
  }
  Type get_type() const;

private:
  void set_size(uint32_t nFileSizeLow, uint32_t nFileSizeHigh);

private:
  DateTime atime;
  uint32_t attributes;
  DateTime ctime;
  DateTime mtime;
  int16_t nlink;
  uint64_t size;
};
}
}
}


#endif
