// yield/fs/file_system.hpp

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

#ifndef _YIELD_FS_FILE_SYSTEM_HPP_
#define _YIELD_FS_FILE_SYSTEM_HPP_

#include "yield/fs/path.hpp"

#ifdef _WIN32
struct _FILETIME;
typedef _FILETIME FILETIME;

struct statvfs {
  unsigned long f_bsize;    // File system block size.
  unsigned long f_frsize;   // Fundamental file system block size.
  uint64_t    f_blocks;     // Total number of blocks on file system
  // in units of f_frsize.
  uint64_t    f_bfree;      // Total number of free blocks.
  uint64_t    f_bavail;     // Number of free blocks available to
  // non-privileged process.
  uint64_t    f_files;      // Total number of file serial numbers.
  uint64_t    f_ffree;      // Total number of free file serial numbers.
  uint64_t    f_favail;     // Number of file serial numbers available to
  // non-privileged process.
  unsigned long f_fsid;     // File system ID.
  unsigned long f_flag;     // Bit mask of f_flag values.
  unsigned long f_namemax;  // Maximum filename length.
};
#else
struct statvfs;
#endif

namespace yield {
class DateTime;

namespace fs {
class Directory;
class File;
class Path;
class Stat;

/**
  File system operations: creating, deleting, opening, and manipulating
    the metadata of files and directories.
  Per POSIX convention, the word "file" is used below to refer to directories,
    regular files, device files, et al.
*/
class FileSystem {
public:
#ifndef _WIN32
  /**
    Default mode bits for new files.
  */
  static mode_t FILE_MODE_DEFAULT;

  /**
    Default mode bits for new directories.
  */
  static mode_t DIRECTORY_MODE_DEFAULT;
#endif

public:
#ifndef _WIN32
  /**
    Check if the caller can access a file using the given access mode.
    @param path path to the file
    @param amode R_OK (read), W_OK (write), X_OK (execute) and/or F_OK (exists)
    @return true if the caller can access the file
  */
  bool access(const Path& path, int amode);

  /**
    Change the mode (permissions) of a file.
    @param path path to the file
    @param mode new mode (permissions) of the file
    @return true on success, false+errno on failure
  */
  bool chmod(const Path& path, mode_t mode);

  /**
    Change the owner of a file.
    @param path path to the file
    @param uid user id (uid) of the new owner
    @return true on success, false+errno on failure
  */
  bool chown(const Path& path, uid_t uid);

  /**
    Change the owner of a file.
    @param path path path to the file
    @param uid user id (uid) of the new owner
    @param gid group id (gid) of the new owner
    @return true on success, false+errno on failure
  */
  bool chown(const Path&path, uid_t uid, gid_t gid);
#endif

public:
  /**
    Create and open a new file.
    Equivalent to open(path, O_CREAT|O_TRUNC|O_WRONLY);
    @param path path to the new file
    @return a new File object on success, NULL+errno on failure
  */
  YO_NEW_REF File* creat(const Path& path);

#ifndef _WIN32
  /**
    Create and open a new file.
    Equivalent to open(path, O_CREAT|O_TRUNC|O_WRONLY, mode);
    @param path path to the new file
    @param mode mode (permissions) of the new file
    @return a new File object on success, NULL+errno on failure
  */
  YO_NEW_REF File* creat(const Path& path, mode_t mode);
#endif

public:
  /**
    Check if a file exists.
    @param path path to the file
    @return true if the file exists
  */
  bool exists(const Path& path);

  /**
    Check if a file is a directory.
    @param path path to the file
    @return true if the file is a directory
  */
  bool isdir(const Path& path);

  /**
    Check if a file is a regular file.
    @param path path to the file
    @return true if the file is a regular file
  */
  bool isreg(const Path& path);

public:
  /**
    Create a hard link to a file.
    @param old_path path to the existing file
    @param new_path path to the new link
    @return true on success, false+errno on failure
  */
  bool link(const Path& old_path, const Path& new_path);

public:
#ifndef _WIN32
  /**
    Retrieve metadata for a file, not following symbolic links.
    @param path path to the file
    @return a new Stat object on success, NULL+errno on failure
  */
  YO_NEW_REF Stat* lstat(const Path& path);
#endif

public:
  /**
    Create a directory.
    @param path path to the new directory
    @return true on success, false+errno on failure
  */
  bool mkdir(const Path& path);

  /**
    Create a directory.
    @param path path to the new directory
    @param mode mode (permissions) of the new directory
    @return true on success, false+errno on failure
  */
#ifndef _WIN32
  bool mkdir(const Path& path, mode_t mode);
#endif

public:
#ifdef _WIN32
  /**
    Create a named pipe.
    @param path path to the new named pipe
    @param flags (same as open flags) for the new named pipe
    @return a new File object on success, NULL+errno on failure
  */
  YO_NEW_REF File* mkfifo(const Path& path, uint32_t flags = 0);
#else
  /**
    Create a named pipe.
    @param path path to the new named pipe
    @param flags (same as open flags) for the new named pipe
    @param mode mode (permissions) of the new named pipe
    @return a new File object on success, NULL+errno on failure
  */
  YO_NEW_REF File*
  mkfifo(
    const Path& path,
    uint32_t flags = 0,
    mode_t mode = FILE_MODE_DEFAULT
  );
#endif

public:
  /**
    Create a tree of directories.
    Tries to create the top-level directory specified by path, then
      any subdirectories as necessary.
    @param path path to the new directory
    @return true on success
  */
  bool mktree(const Path& path);

#ifndef _WIN32
  /**
    Create a tree of directories.
    Tries to create the top-level directory specified by path, then
      any subdirectories as necessary.
    @param path path to the new directory
    @param mode mode (permissions) of the new directory/directories
    @return true on success
  */
  bool mktree(const Path& path, mode_t mode);
#endif

public:
#ifdef _WIN32
  /**
    Open and/or create a file.
    @param path path to the file
    @param flags open flags (O_RDONLY, O_TRUNC, et al.)
    @param attributes Win32 attributes of a new file
    @return a new File object on success, NULL+errno on failure
  */
  YO_NEW_REF File*
  open(
    const Path& path,
    uint32_t flags = 0,
    uint32_t attributes = 0
  );
#else
  /**
    Open and/or create a file.
    @param path path to the file
    @param flags open flags (O_RDONLY, O_TRUNC, et al.)
    @param mode mode (permissions) of a new file
    @return a new File object on success, NULL+errno on failure
  */
  YO_NEW_REF File*
  open(
    const Path& path,
    uint32_t flags = 0,
    mode_t mode = FILE_MODE_DEFAULT
  );
#endif

public:
  /**
    Open a directory for iteration.
    @param path path to the directory
    @return a new Directory object on success, NULL+errno on failure
  */
  YO_NEW_REF Directory* opendir(const Path& path);

public:
#ifndef _WIN32
  /**
    Read the target of a symbolic link.
    @param path path to the symbolic link
    @param[out] target_path path to which the symbolic link points
    @return true on success, false+errno on failure
  */
  bool readlink(const Path& path, OUT Path& target_path);
#endif

public:
  /**
    Resolve the real path to a file: normalizing relative paths and 
      following symbolic links.
    @param path path to the file
    @param[out] realpath real path of a file
    @return true on success, false+errno on failure
  */
  bool realpath(const Path& path, OUT Path& realpath);

public:
  /**
    Rename a file.
    @param from_path old name of the file
    @param to_path new name of the file
    @return true on success, false+errno on failure
  */
  bool rename(const Path& from_path, const Path& to_path);

public:
  /**
    Delete a directory.
    @param path path to the directory
    @return true on success, false+errno on failure
  */
  bool rmdir(const Path& path);

  /**
    Delete a directory tree: unlink'ing files and rmdir'ing directories
      under path.
    @param path path to the parent directory
    @return true on success, false+errno on failure
  */    
  bool rmtree(const Path& path);

public:
  /**
    Retrieve metadata for a file, following symbolic links.
    @param path path to the file
    @return a new Stat object on success, NULL+errno on failure
  */
  YO_NEW_REF Stat* stat(const Path& path);

  /**
    Retrieve metadata for a file system.
    @param path any path on the file system
    @param[out] statvfs_ file system metadata
    @return true on success, false+errno on failure
  */
  bool statvfs(const Path& path, OUT struct statvfs& statvfs_);

public:
#ifndef _WIN32
  /**
    Create a symbolic link to a file.
    @param old_path path to the existing file
    @param new_path path to the new link
    @return true on success, false+errno on failure
  */
  bool symlink(const Path& old_path, const Path& new_path);
#endif

public:
  /**
    Create a file if it does not exist, otherwise update its times.
    @param path path to the file
    @return true on success, false+errno on failure
  */
  bool touch(const Path& path);

#ifndef _WIN32
  /**
    Create a file if it does not exist, otherwise update its times to
      current time.
    @param path path to the file
    @param mode mode of the file if it is created
    @return true on success, false+errno on failure
  */
  bool touch(const Path& path, mode_t mode);
#endif

public:
  /**
    Delete a file.
    @param path path to the file
    @return true on success, false+errno on failure
  */
  bool unlink(const Path& path);

public:
  /**
    Update a file's times.
    @param path path to the file
    @param atime new access time of the file
    @param mtime new modification time of the file
    @return true on success, false+errno on failure
  */
  bool utime(const Path& path, const DateTime& atime, const DateTime& mtime);

#ifdef _WIN32
  /**
    Update a file's times.
    @param path path to the file
    @param atime new access time of the file
    @param mtime new modification time of the file
    @param ctime new creation time of the file
    @return true on success, false+errno on failure
  */
  bool
  utime(
    const Path& path,
    const DateTime& atime,
    const DateTime& mtime,
    const DateTime& ctime
  );
#endif

private:
#ifdef _WIN32
  bool
  utime(
    const Path& path,
    const FILETIME* ftCreationTime,
    const FILETIME* ftLastAccessTime,
    const FILETIME* ftLastWriteTime
  );
#endif
};
}
}

#endif
