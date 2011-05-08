// yield/fs/win32/stat.cpp

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

#include "yield/fs/win32/stat.hpp"

#include <Windows.h>

namespace yield {
namespace fs {
namespace win32 {
Stat::Stat(
  const DateTime& atime,
  uint32_t attributes,
  const DateTime& ctime,
  const DateTime& mtime,
  int16_t nlink,
  uint64_t size
) : atime(atime),
  attributes(attributes),
  ctime(ctime),
  mtime(mtime),
  nlink(nlink),
  size(size)
{ }

Stat::Stat(const BY_HANDLE_FILE_INFORMATION& stbuf)
  : atime(stbuf.ftLastAccessTime),
    attributes(stbuf.dwFileAttributes),
    ctime(stbuf.ftCreationTime),
    mtime(stbuf.ftLastWriteTime),
    nlink(static_cast<int16_t>(stbuf.nNumberOfLinks)) {
  set_size(stbuf.nFileSizeLow, stbuf.nFileSizeHigh);
}

Stat::Stat(const WIN32_FILE_ATTRIBUTE_DATA& stbuf)
  : atime(stbuf.ftLastAccessTime),
    attributes(stbuf.dwFileAttributes),
    ctime(stbuf.ftCreationTime),
    mtime(stbuf.ftLastWriteTime),
    nlink(1) {
  set_size(stbuf.nFileSizeLow, stbuf.nFileSizeHigh);
}

Stat::Stat(const WIN32_FIND_DATA& stbuf)
  : atime(stbuf.ftLastAccessTime),
    attributes(stbuf.dwFileAttributes),
    ctime(stbuf.ftCreationTime),
    mtime(stbuf.ftLastWriteTime),
    nlink(1) {
  set_size(stbuf.nFileSizeLow, stbuf.nFileSizeHigh);
}

bool Stat::ISDEV() const {
  return (attributes & FILE_ATTRIBUTE_DEVICE) == FILE_ATTRIBUTE_DEVICE;
}

bool Stat::ISDIR() const {
  return (attributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
}

bool Stat::ISREG() const {
  return !ISDEV() && !ISDIR();
}

bool Stat::operator==(const Stat& other) const {
  return get_atime() == other.get_atime() &&
         get_attributes() == other.get_attributes() &&
         get_ctime() == other.get_ctime() &&
         get_mtime() == other.get_mtime() &&
         get_nlink() == other.get_nlink() &&
         get_size() == other.get_size();
}

Stat::operator BY_HANDLE_FILE_INFORMATION() const {
  BY_HANDLE_FILE_INFORMATION bhfi;
  memset(&bhfi, 0, sizeof(bhfi));
  bhfi.dwFileAttributes = get_attributes();
  bhfi.ftCreationTime = get_ctime();
  bhfi.ftLastAccessTime = get_atime();
  bhfi.ftLastWriteTime = get_mtime();
  ULARGE_INTEGER size;
  size.QuadPart = get_size();
  bhfi.nFileSizeLow = size.LowPart;
  bhfi.nFileSizeHigh = size.HighPart;
  bhfi.nNumberOfLinks = get_nlink();
  return bhfi;
}

Stat::operator WIN32_FILE_ATTRIBUTE_DATA() const {
  WIN32_FILE_ATTRIBUTE_DATA file_attribute_data;
  memset(&file_attribute_data, 0, sizeof(file_attribute_data));
  file_attribute_data.dwFileAttributes = get_attributes();
  file_attribute_data.ftCreationTime = get_ctime();
  file_attribute_data.ftLastAccessTime = get_atime();
  file_attribute_data.ftLastWriteTime = get_mtime();
  ULARGE_INTEGER size;
  size.QuadPart = get_size();
  file_attribute_data.nFileSizeLow = size.LowPart;
  file_attribute_data.nFileSizeHigh = size.HighPart;
  return file_attribute_data;
}

Stat::operator WIN32_FIND_DATA() const {
  WIN32_FIND_DATA find_data;
  memset(&find_data, 0, sizeof(find_data));
  find_data.dwFileAttributes = get_attributes();
  find_data.ftCreationTime = get_ctime();
  find_data.ftLastAccessTime = get_atime();
  find_data.ftLastWriteTime = get_mtime();
  ULARGE_INTEGER size;
  size.QuadPart = get_size();
  find_data.nFileSizeLow = size.LowPart;
  find_data.nFileSizeHigh = size.HighPart;
  return find_data;
}

Stat& Stat::operator=(const BY_HANDLE_FILE_INFORMATION& stbuf) {
  atime = stbuf.ftLastAccessTime;
  attributes = stbuf.dwFileAttributes;
  ctime = stbuf.ftCreationTime;
  mtime = stbuf.ftLastWriteTime;
  nlink = static_cast<int16_t>(stbuf.nNumberOfLinks);
  set_size(stbuf.nFileSizeLow, stbuf.nFileSizeHigh);
  return *this;
}

Stat& Stat::operator=(const WIN32_FILE_ATTRIBUTE_DATA& stbuf) {
  atime = stbuf.ftLastAccessTime;
  attributes = stbuf.dwFileAttributes;
  ctime = stbuf.ftCreationTime;
  mtime = stbuf.ftLastWriteTime;
  nlink = 1; // WIN32_FILE_ATTRIBUTE_DATA doesn't have a nNumberOfLinks
  set_size(stbuf.nFileSizeLow, stbuf.nFileSizeHigh);
  return *this;
}

Stat& Stat::operator=(const WIN32_FIND_DATA& stbuf) {
  atime = stbuf.ftLastAccessTime;
  attributes = stbuf.dwFileAttributes;
  ctime = stbuf.ftCreationTime;
  mtime = stbuf.ftLastWriteTime;
  nlink = 1; // WIN32_FIND_DATA doesn't have a nNumberOfLinks
  set_size(stbuf.nFileSizeLow, stbuf.nFileSizeHigh);
  return *this;
}

void Stat::set_size(uint32_t nFileSizeLow, uint32_t nFileSizeHigh) {
  ULARGE_INTEGER uliSize;
  uliSize.LowPart = nFileSizeLow;
  uliSize.HighPart = nFileSizeHigh;
  size = static_cast<size_t>(uliSize.QuadPart);
}
}
}
}
