// yield/fs/file.hpp

// Copyright (c) 2010 Minor Gordon
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


#include "yield/channel.hpp"


#ifdef _WIN32
#define O_SYNC     010000
#define O_ASYNC    020000
#define O_DIRECT   040000
#define O_HIDDEN   0100000
#define O_NONBLOCK 0200000
#endif

#ifndef SEEK_SET
#define SEEK_SET    0
#define SEEK_CUR    1
#define SEEK_END    2
#endif


namespace yield {
class Log;


namespace fs {
class ExtendedAttributes;
class Path;
class Stat;


class File : public Channel {
public:
  class Lock : public Object {
  public:
    Lock
    (
      uint64_t start,
      uint64_t len,
      bool exclusive = true,
      pid_t pid = static_cast<pid_t>( -1 ), // getpid()
      int16_t whence = SEEK_SET
    )
      : exclusive( exclusive ),
        len( len ),
        pid( pid ),
        start( start ),
        whence( whence )
    { }

    uint64_t get_len() const {
      return len;
    }
    pid_t get_pid() const {
      return pid;
    }
    uint64_t get_start() const {
      return start;
    }
    int16_t get_whence() const {
      return whence;
    }
    bool is_exclusive() const {
      return exclusive;
    }

    // Object
    File::Lock& inc_ref() {
      return Object::inc_ref( *this );
    }

  private:
    bool exclusive;
    uint64_t len;
    pid_t pid;
    uint64_t start;
    int16_t whence;
  };

public:
  virtual ~File() { }

  virtual bool datasync() {
    return sync();
  }
  virtual YO_NEW_REF Stat* getattr() = 0;
  virtual YO_NEW_REF Lock* getlk( const Lock& ) = 0;
  virtual size_t getpagesize();
  virtual YO_NEW_REF ExtendedAttributes* openxattrs() = 0;
  virtual ssize_t pread( void*, size_t, uint64_t offset ) = 0;
  virtual ssize_t preadv( const iovec*, int, uint64_t offset ) = 0;
  virtual ssize_t pwrite( const void*, size_t, uint64_t offset ) = 0;
  virtual ssize_t pwritev( const iovec*, int, uint64_t offset ) = 0;
  virtual uint64_t seek( int64_t offset, uint8_t whence = SEEK_SET ) = 0;
  virtual bool setlk( const Lock& ) = 0;
  virtual bool setlkw( const Lock& ) = 0;
  YO_NEW_REF Stat* stat() {
    return getattr();
  }
  virtual bool sync() = 0;
  virtual uint64_t tell() = 0;
  virtual bool truncate( uint64_t new_size ) = 0;
  virtual bool unlk( const Lock& ) = 0;

  // Object
  File& inc_ref() {
    return Object::inc_ref( *this );
  }

protected:
  File() { }

  //protected:
  //  bool trace( Log&, const char* method, const Path&, bool ret );
  //  Stat* trace_getattr( Log&, const Path&, Stat* );
  //  Lock* trace_getlk( Log&, const Path&, const Lock&, Lock* );
  //  ssize_t trace_read( Log&, const Path&, void*, size_t, ssize_t ret );
  //  bool trace_setlk( Log&, const Path&, const Lock&, bool ret );
  //  bool trace_setlkw( Log&, const Path&, const Lock&, bool ret );
  //  bool trace_truncate( Log&, const Path&, uint64_t new_size, bool ret );
  //  bool trace_unlk( Log&, const Path&, const Lock&, bool ret );
  //  ssize_t trace_write( Log&, const Path&, const void*, size_t, ssize_t );
  //  ssize_t trace_writev( Log&, const Path&, const iovec*, int, ssize_t );
};

//std::ostream& operator<<( std::ostream& os, const File::Lock& lock );
}
}


#endif
