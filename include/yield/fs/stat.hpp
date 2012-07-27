// yield/fs/stat.hpp

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

#ifndef _YIELD_FS_STAT_HPP_
#define _YIELD_FS_STAT_HPP_

#include "yield/date_time.hpp"
#include "yield/object.hpp"

#ifdef _WIN32
struct _BY_HANDLE_FILE_INFORMATION;
typedef _BY_HANDLE_FILE_INFORMATION BY_HANDLE_FILE_INFORMATION;
struct _WIN32_FILE_ATTRIBUTE_DATA;
typedef struct _WIN32_FILE_ATTRIBUTE_DATA WIN32_FILE_ATTRIBUTE_DATA;
struct _WIN32_FIND_DATAW;
typedef _WIN32_FIND_DATAW WIN32_FIND_DATAW;
typedef WIN32_FIND_DATAW WIN32_FIND_DATA;
#else
#include <sys/stat.h>
#endif

namespace yield {
namespace fs {
/**
  File metadata, akin to struct stat on POSIX systems.
*/
class Stat : public Object {
public:
#ifdef _WIN32
  /**
    Construct a Stat from its component parts.
    @param atime file last access time
    @param attributes file attributes
    @param ctime file creation time
    @param mtime file last modified time
    @param nlink number of links to the file
    @param size size of the file
  */
  Stat(
    const DateTime& atime,
    uint32_t attributes,
    const DateTime& ctime,
    const DateTime& mtime,
    int16_t nlink,
    uint64_t size
  );

  /**
    Construct a Stat from a struct BY_HANDLE_FILE_INFORMATION.
    @param by_handle_file_information struct describing file metadata
  */
  Stat(const BY_HANDLE_FILE_INFORMATION& by_handle_file_information);

  /**
    Construct a Stat from a struct WIN32_FILE_ATTRIBUTE_DATA.
    @param win32_file_attribute_data struct describing file metadata
  */
  Stat(const WIN32_FILE_ATTRIBUTE_DATA& win32_file_attribute_data);

  /**
    Construct a Stat from a struct WIN32_FIND_DATA.
    @param win32_find_data struct describing file metadata
  */
  Stat(const WIN32_FIND_DATA& win32_find_data);
#else
  /**
    Construct a Stat from a struct stat.
    @param stbuf struct describing file metadata
  */
  Stat(const struct stat& stbuf);
#endif

public:
  /**
    Get the last access time of the file referred to by this Stat.
    @return the last access time of the file referred to by this Stat
  */
  const DateTime& get_atime() const {
    return atime;
  }

#ifdef _WIN32
  /**
    Get the Win32 attributes of the file referred to by this Stat.
    @return the Win32 attributes of the file referred to by this Stat
  */
  uint32_t get_attributes() const;
#endif

#ifndef _WIN32
  /**
    Get the block size of the file referred to by this Stat.
    @return the block size of the file referred to by this Stat
  */
  uint64_t get_blksize() const;

  /**
    Get the block count of the file referred to by this Stat.
    @return the block count of the file referred to by this Stat
  */
  uint64_t get_blocks() const;
#endif


  /**
    Get the creation time of the file referred to by this Stat.
    @return the creation time of the file referred to by this Stat
  */
  const DateTime& get_ctime() const {
    return ctime;
  }

#ifndef _WIN32
  /**
    Get the ID of the device containing the file referred to by this Stat.
    @return the ID of the device containing file referred to by this Stat
  */
  uint64_t get_dev() const;

  /**
    Get the group ID (gid) of the owner of the file referred to by this Stat.
    @return the group ID (gid) of the owner of the file referred to by this Stat
  */
  gid_t get_gid() const;

  /**
    Get the inode number of the file referred to by this Stat.
    @return the inode number of the file referred to by this Stat
  */
  uint64_t get_ino() const;

  /**
    Get the mode bits (including permissions) of the file referred to by this
      Stat.
    @return the mode bits (including permissions) of the file referred to by
      this Stat
  */
  mode_t get_mode() const;
#endif

  const DateTime& get_mtime() const {
    return mtime;
  }

  /**
    Get the number of hard links that point to the file referred to by this
      Stat.
    @return the number of hard links that point to the file referred to by this
      Stat
  */
  int16_t get_nlink() const;

#ifndef _WIN32
  /**
    Get the device ID of the file referred to by this Stat, if the file is a
      device.
    @return the device ID of the file referred to by this Stat, if the file is
      a device.
  */
  uint64_t get_rdev() const;
#endif

  /**
    Get the size of the file referred to by this Stat.
    @return the size of the file referred to by this Stat
  */
  uint64_t get_size() const;

#ifndef _WIN32
  /**
    Get the user ID (uid) of the owner of the file referred to by this Stat.
    @return the user ID (uid) of the owner of the file referred to by this Stat
  */
  uid_t get_uid() const;
#endif

public:
#ifndef _WIN32
  /**
    Check if this Stat refers to a block device.
    @return true if this Stat refers to a block device
  */
  bool ISBLK() const;

  /**
    Check if this Stat refers to a character device.
    @return true if this Stat refers to a character device
  */
  bool ISCHR() const;
#endif

#ifdef _WIN32
  /**
    Check if this Stat refers to a device.
    @return true if this Stat refers to a device
  */
  bool ISDEV() const;
#endif

  /**
    Check if this Stat refers to a directory.
    @return true if this Stat refers to a directory
  */
  bool ISDIR() const;

#ifndef _WIN32
  /**
    Check if this Stat refers to a named pipe.
    @return true if this Stat refers to a named pipe
  */
  bool ISFIFO() const;

  /**
    Check if this Stat refers to a symbolic link.
    @return true if this Stat refers to a symbolic link
  */
  bool ISLNK() const;
#endif

  /**
    Check if this Stat refers to a regular file.
    @return true if this Stat refers to a regular file
  */
  bool ISREG() const;

#ifndef _WIN32
  /**
    Check if this Stat refers to a Unix socket.
    @return true if this Stat refers to a Unix socket
  */
  bool ISSOCK() const;
#endif

public:
  /**
    Compare two Stat objects for equality
    @param other Stat object to compare this one to
    @return true if the Stat objects are equal
  */
  bool operator==(const Stat& other) const;

public:
#ifdef _WIN32
  /**
    Cast this Stat to a struct BY_HANDLE_FILE_INFORMATION.
    @return this Stat as a struct BY_HANDLE_FILE_INFORMATION
  */
  operator BY_HANDLE_FILE_INFORMATION() const;

  /**
    Cast this Stat to a struct WIN32_FILE_ATTRIBUTE_DATA.
    @return this Stat as a struct WIN32_FILE_ATTRIBUTE_DATA
  */
  operator WIN32_FILE_ATTRIBUTE_DATA() const;

  /**
    Cast this Stat to a struct WIN32_FIND_DATA.
    @return this Stat as a struct WIN32_FIND_DATA
  */
  operator WIN32_FIND_DATA() const;
#else
  /**
    Cast this Stat to a struct stat.
    @return this Stat as a struct stat
  */
  operator struct stat() const;
#endif

public:
#ifdef _WIN32
  /**
    Replace the contents of this Stat with the contents of a struct
      BY_HANDLE_FILE_INFORMATION.
    @param by_handle_file_info new contents of this Stat
    @return *this
  */
  Stat& operator=(const BY_HANDLE_FILE_INFORMATION& by_handle_file_info);

  /**
    Replace the contents of this Stat with the contents of a struct
      WIN32_FILE_ATTRIBUTE_DATA.
    @param win32_file_attribute_data new contents of this Stat
    @return *this
  */
  Stat& operator=(const WIN32_FILE_ATTRIBUTE_DATA& win32_file_attribute_data);

  /**
    Replace the contents of this Stat with the contents of a struct
      WIN32_FIND_DATA.
    @param win32_find_data new contents of this Stat
    @return *this
  */
  virtual Stat& operator=(const WIN32_FIND_DATA& win32_find_data);
#endif

public:
  // yield::Object
  Stat& inc_ref() {
    return Object::inc_ref(*this);
  }

private:
  void set_size(uint32_t nFileSizeLow, uint32_t nFileSizeHigh);

private:
  DateTime atime;
#ifdef _WIN32
  uint32_t attributes;
#else
  blksize_t blksize;
  blkcnt_t blocks;
#endif
  DateTime ctime;
#ifndef _WIN32
  dev_t dev;
  gid_t gid;
  ino_t ino;
  mode_t mode;
#endif
  DateTime mtime;
#ifdef _WIN32
  int16_t nlink;
#else
  nlink_t nlink;
  dev_t rdev;
#endif
  uint64_t size;
#ifndef _WIN32
  uid_t uid;
#endif
};
}
}

#endif
