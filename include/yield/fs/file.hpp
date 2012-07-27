// yield/fs/file.hpp

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

#ifndef _YIELD_FS_FILE_HPP_
#define _YIELD_FS_FILE_HPP_

#include "yield/buffer.hpp"
#include "yield/channel.hpp"

#ifdef _WIN32
#define O_SYNC     010000
#define O_ASYNC    020000
#define O_DIRECT   040000
#define O_HIDDEN   0100000
#define O_NONBLOCK 0200000
#else
#include <fcntl.h>
#endif

namespace yield {
namespace fs {
class Map;
class Stat;

/**
  File operations: read, write, retrieve metadata, et al.
  Wraps a platform-specific file handle/descriptor.
*/
class File : public Channel {
public:
  const static uint32_t TYPE_ID = 1965528734UL;

public:
  /**
    An advisory file lock.
  */
  class Lock : public Object {
  public:
    Lock(
      uint64_t start,
      uint64_t len,
      bool exclusive = true,
#ifndef _WIN32
      pid_t pid = -1,
#endif
      int16_t whence = SEEK_SET
    );

#ifndef _WIN32
    Lock(const struct flock& flock_);
#endif

  public:
    /**
      Get the length (extent) of the lock in bytes.
      @return the length (extent) of the lock in bytes.
    */
    uint64_t get_len() const;

#ifndef _WIN32
    /**
      Get the process ID (pid) of the owner of this lock.
      @return the process ID (pid) of the owner of this lock
    */
    pid_t get_pid() const;
#endif

    /**
      Get the start offset in the file of this lock.
      @return the start offset in the file of this lock.
    */
    uint64_t get_start() const;

    /**
      Get the nature of the start offset of this lock relative to the current
        file pointer: SEEK_CUR, SEEK_END, SEEK_SET.
      @return the nature of the start offset
    */
    int16_t get_whence() const;

    /**
      Check if this lock is exclusive (read/write).
      @return true if the lock is exclusive (read/write), false if it is shared (read-only)
    */
    bool is_exclusive() const;

  public:
#ifndef _WIN32
    /**
      Cast this lock to a struct flock.
      @return a struct flock representing this lock
    */
    operator struct flock() const {
      return flock_;
    }
#endif

  public:
    // yield::Object
    File::Lock& inc_ref() {
      return Object::inc_ref(*this);
    }

  private:
#ifdef _WIN32
    bool exclusive;
    uint64_t len;
    uint64_t start;
    int16_t whence;
#else
    struct flock flock_;
#endif
  };

public:
  /**
    A memory-mapped region in a file.
  */
  class Map : public Buffer {
  public:
    ~Map();

  public:
    /**
      Get the file underlying this map.
      @return the file underlying this map
    */
    File& get_file() {
      return file;
    }

    /**
      Get the offset in the file at which this map starts.
      @return the offset in the file at which this map starts
    */
    uint64_t get_file_offset() const {
      return file_offset;
    }

    /**
      Check if this map is read-only.
      @return true if the map is read-only, false if it is read-write.
    */
    bool is_read_only() const;

    /**
      Check if this map is shared (meaning that changes are visible to all users
        of the file) or private (changes are private to the mapper).
      @return true if the map is shared, false if the map is private
    */
    bool is_shared() const;

  public:
    /**
      Flush changes to the map to disk.
      @return true on success, false+errno on failure
    */
    bool sync();

    /**
      Flush changes to the map in the specified byte range to disk.
      @param offset offset of the byte range
      @param length length of the byte range
      @return true on success, false+errno on failure
    */
    bool sync(size_t offset, size_t length);

    /**
      Flush changes to the map in the specified byte range to disk.
      @param ptr pointer to the start of the byte range
      @param length length of the byte range
      @return true on success, false+errno on failure
    */
    bool sync(void* ptr, size_t length);

  public:
    /**
      Unmap the mapped region, barring further access via this Map.
      @return true on success, false+errno on failure
    */
    bool unmap();

  private:
    friend class File;

    Map(
      size_t capacity,
      void* data,
      File& file,
#ifdef _WIN32
      fd_t file_mapping,
#endif
      uint64_t file_offset,
#ifdef _WIN32
      bool read_only,
      bool shared
#else
      int flags,
      int prot
#endif
    );

  private:
    File& file;
#ifdef _WIN32
    fd_t file_mapping;
#endif
    uint64_t file_offset;
#ifdef _WIN32
    bool read_only;
    bool shared;
#else
    int flags;
    int prot;
#endif
  };

public:
  /**
    Construct a File by wrapping a file descriptor.
    Takes ownership of the file descriptor.
    @param fd the file descriptor to take ownership of
  */
  File(fd_t fd);

  /**
    Destruct a File, closing its file descriptor.
  */
  ~File();

public:
  /**
    Flush any changes to a file's data to disk, ignoring metadata changes.
    @return true on success, false+errno on failure
  */
  bool datasync();

public:
  /**
    Duplicate the underlying file descriptor and wrap that in a new File.
    @return a new File with a dup'd file descriptor on success, NULL+errno on failure
  */
  YO_NEW_REF File* dup() {
    return dup(fd);
  }

  /**
    Duplicate a file descriptor and wrap it in a new File.
    @param fd file descriptor to duplicate
    @return a new File with a dup'd file descriptor on success, NULL+errno on failure
  */
  static YO_NEW_REF File* dup(fd_t fd);

  /**
    Duplicate a file stream's underlying descriptor and wrap it in a new File.
    @param file file stream with a descriptor to duplicate
    @return a new File with a dup'd file descriptor on success, NULL+errno on failure
  */
  static YO_NEW_REF File* dup(FILE* file);

public:
#ifndef _WIN32
  /**
    Find an advisory lock on this file that conflicts (overlaps) with the
      specified lock.
    @param lock lock that would cause a conflict
    @return a new Lock specifying an existing lock that would conflict with lock on
      success, NULL+errno on failure
  */
  YO_NEW_REF Lock* getlk(const Lock& lock);
#endif

public:
  /**
    Map a region of this file into memory.
    @param length in bytes of the region to map
    @param offset offset in the file of the region to map
    @param read_only true if the resulting map should be read-only or false
      if it should be read-write
    @param shared true if the resulting map should be shared (changes visible
      to all users of the file) or fales if it should be private (changes
      visible only to the mapper)
    @return a new Map object on success, NULL+errno on failure
  */
  YO_NEW_REF Map*
  mmap(
    size_t length = SIZE_MAX,
    off_t offset = 0,
    bool read_only = false,
    bool shared = true
  );

public:
  /**
    Get the platform-specific file descriptor underlying this file.
    @return the platform-specific file descriptor underlying this file
  */
  operator fd_t() const {
    return fd;
  }

public:
  /**
    Read from the file from the specified offset.
    @param[in, out] buffer buffer into which to read
    @param offset offset from which to read
    @return the number of bytes read on success, -1+errno on failure
  */
  ssize_t pread(Buffer& buffer, off_t offset);

  /**
    Read from the file from the specified offset.
    @param[in, out] buf buffer into which to read
    @param buflen length of the memory region pointed to by buf
    @param offset offset from which to read
    @return the number of bytes read on success, -1+errno on failure
  */
  ssize_t pread(void* buf, size_t buflen, off_t offset);

  /**
    Read from the file from the specified offset.
    @param[in, out] iov pointer to an array of iovecs describing buffers in which to read
    @param iovlen length of the iovecs array
    @param offset offset from which to read
    @return the number of bytes read on success, -1+errno on failure
  */
  ssize_t preadv(const iovec* iov, int iovlen, off_t offset);

public:
  /**
    Write to the file from the specified offset.
    @param buffer buffer from which to write
    @param offset offset from which to write
    @return the number of bytes written on success, -1+errno on failure
  */
  ssize_t pwrite(const Buffer& buffer, off_t offset);

  /**
    Write to the file from the specified offset.
    @param buf buffer from which to write
    @param buflen length of the memory region pointed to by buf
    @param offset offset from which to write
    @return the number of bytes written on success, -1+errno on failure
  */
  ssize_t pwrite(const void* buf, size_t buflen, off_t offset);

  /**
    Write to the file from the specified offset.
    @param iov pointer to an array of iovecs describing buffers from which to write
    @param iovlen length of the iovecs array
    @param offset offset from which to write
    @return the number of bytes written on success, -1+errno on failure
  */
  ssize_t pwritev(const iovec* iov, int iovlen, off_t offset);

public:
  /**
    Change the current file pointer, the default offset from which
      reads and writes occur.
   @param offset offset of the new file pointer
   @param whence the nature of offset: SEEK_CUR (relative from the
     current file pointer); SEEK_END (relative from the end of the file); or
     SEEK_SET (absolute from the beginning of the file)
   @return the new absolute offset of the current file pointer from the
     beginning of the file on success or (off_t)-1+errno on failure
  */
  off_t seek(off_t offset, uint8_t whence = SEEK_SET);

public:
  /**
    Acquire an advisory lock on this file, non-blocking.
    @param lock Lock object describing the lock to acquire
    @return true on success, false+errno on failure
  */
  bool setlk(const Lock& lock);

  /**
    Acquire an advisory lock on this file, blocking.
    @param lock Lock object describing the lock to acquire
    @return true on success, false+errno on failure
  */
  bool setlkw(const Lock& lock);

public:
  /**
    Retrieve the metadata for this file.
    @return a new Stat object on success, NULL+errno on failure.
  */
  YO_NEW_REF Stat* stat();

public:
  /**
    Flush data and metadata changes to this file to disk.
    @return true on success, false+errno on failure
  */
  bool sync();

public:
  /**
    Get the current file pointer, absolute from the beginning of the file.
    @return the current file pointer on success, (off_t)-1 on failure
  */
  off_t tell();

public:
  /**
    Truncate the file's data to the specified length.
    May expand the file on some platforms if length is greater than the current
      file length.
    @param length new length of the file
    @return true on success, false+errno on failure
  */
  bool truncate(off_t length);

public:
  /**
    Release an advisory lock previously acquired with setlk or setlkw.
    @param lock Lock object describing the advisory lock
    @return true on success, false+errno on failure
  */
  bool unlk(const Lock& lock);

public:
  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }

  const char* get_type_name() const {
    return "yield::fs::File";
  }

  File& inc_ref() {
    return Object::inc_ref(*this);
  }

public:
  // yield::Channel
  bool close();
  ssize_t read(Buffer& buffer);
  ssize_t read(void* buf, size_t buflen);
  ssize_t readv(const iovec* iov, int iovlen);
  ssize_t write(const Buffer& buffer);
  ssize_t write(const void* buf, size_t buflen);
  ssize_t writev(const iovec* iov, int iovlen);

private:
#ifdef _WIN32
  bool setlk(const Lock&, bool wait);
#endif

private:
  fd_t fd;
};
}
}

#endif
