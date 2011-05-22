// yield/fs/win32/directory.cpp

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
#include "yield/date_time.hpp"
#include "yield/fs/directory.hpp"

#include <Windows.h>

namespace yield {
namespace fs {
using yield::i18n::tstring;

Directory::Directory(HANDLE hDirectory)
  : hDirectory(hDirectory), hFindFile(INVALID_HANDLE_VALUE)
{ }

Directory::~Directory() {
  close();
}

bool Directory::close() {
  if (hDirectory != INVALID_HANDLE_VALUE) {
    CloseHandle(hDirectory);
    hDirectory = INVALID_HANDLE_VALUE;

    if (hFindFile != INVALID_HANDLE_VALUE) {
      FindClose(hFindFile);
      hFindFile = INVALID_HANDLE_VALUE;
    }

    return true;
  } else
    return false;
}

Directory::Entry* Directory::read() {
  Entry* entry = NULL;
  if (read(entry))
    return entry;
  else
    return NULL;
}

bool Directory::read(OUT Entry& entry) {
  Entry* p_dentry = &entry;
  return read(p_dentry);
}

bool Directory::read(OUT Entry*& entry) {
  if (hFindFile == INVALID_HANDLE_VALUE) {
    const size_t file_name_info_size
    = sizeof(FILE_NAME_INFO) + sizeof(WCHAR) * MAX_PATH;
    char file_name_info_buffer[file_name_info_size];
    FILE_NAME_INFO* file_name_info = new(file_name_info_buffer) FILE_NAME_INFO;

    if (
      GetFileInformationByHandleEx(
        *this,
        FileNameInfo,
        file_name_info,
        file_name_info_size
      )
    ) {
      DWORD FileNameLength = file_name_info->FileNameLength;
      FileNameLength /= sizeof(wchar_t);
      wchar_t* FileName = file_name_info->FileName;
      Path search_pattern = Path(FileName, FileNameLength) / L'*';

      WIN32_FIND_DATA find_data;
      hFindFile = FindFirstFile(search_pattern.c_str(), &find_data);
      if (hFindFile != INVALID_HANDLE_VALUE) {
        if (entry == NULL)
          entry = new Entry(find_data);
        else
          *entry = find_data;

        return true;
      } else
        return false;
    } else
      return false;
  }

  WIN32_FIND_DATA find_data;
  while (FindNextFile(hFindFile, &find_data)) {
    if (entry == NULL)
      entry = new Entry(find_data);
    else
      *entry = find_data;

    return true;
  }

  FindClose(hFindFile);
  hFindFile = INVALID_HANDLE_VALUE;

  return false;
}

void Directory::rewind() {
  if (hFindFile != INVALID_HANDLE_VALUE) {
    FindClose(hFindFile);
    hFindFile = INVALID_HANDLE_VALUE;
  }
}


Directory::Entry::Entry(const WIN32_FIND_DATA& find_data)
  : Stat(find_data),
    name(find_data.cFileName) {
}

Directory::Entry::Type Directory::Entry::get_type() const {
  if (ISDEV())
    return TYPE_DEV;
  else if (ISDIR())
    return TYPE_DIR;
  else
    return TYPE_REG;
}

bool Directory::Entry::is_hidden() const {
  return (get_attributes() & FILE_ATTRIBUTE_HIDDEN) != 0;
}

bool Directory::Entry::is_special() const {
  return get_name() == Path::CURRENT_DIRECTORY
         ||
         get_name() == Path::PARENT_DIRECTORY;
}

Directory::Entry&
Directory::Entry::operator=(
  const WIN32_FIND_DATA& find_data
) {
  name = find_data.cFileName;
  Stat::operator=(find_data);
  return *this;
}
}
}
