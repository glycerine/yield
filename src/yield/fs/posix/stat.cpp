// yield/fs/posix/stat.cpp

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


#include "stat.hpp"


namespace yield
{
  namespace fs
  {
    namespace posix
    {
      Stat::Stat()
      : atime( INVALID_ATIME ),
        ctime( INVALID_CTIME ),
        mtime( INVALID_MTIME )
      {
        st_blksize = INVALID_BLKSIZE;
        st_blocks = INVALID_BLOCKS;
        st_dev = INVALID_DEV;
        st_gid = INVALID_GID;
        st_ino = INVALID_INO;
        st_mode = INVALID_MODE;
        st_nlink = INVALID_NLINK;
        st_rdev = INVALID_RDEV;
        st_size = INVALID_SIZE;
        st_uid = INVALID_UID;
      }

      Stat::Stat( const struct stat& stbuf )
      : atime( stbuf.st_atime ),
        ctime( stbuf.st_ctime ),
        mtime( stbuf.st_mtime )
      {
        *this = stbuf;
      }

      Stat::Type Stat::get_type() const
      {
        if ( S_ISBLK( get_mode() ) ) return TYPE_BLK;
        else if ( S_ISCHR( get_mode() ) ) return TYPE_CHR;
        else if ( S_ISDIR( get_mode() ) ) return TYPE_DIR;
        else if ( S_ISFIFO( get_mode() ) ) return TYPE_FIFO;
        else if ( S_ISLNK( get_mode() ) ) return TYPE_LNK;
        else if ( S_ISREG( get_mode() ) ) return TYPE_REG;
        else if ( S_ISSOCK( get_mode() ) ) return TYPE_SOCK;
        else return yield::fs::Stat::get_type();
      }

      Stat::operator struct stat() const
      {
        struct stat stbuf;
        memset( &stbuf, 0, sizeof( stbuf ) );
        stbuf.st_dev = static_cast<dev_t>( get_dev() );
        stbuf.st_ino = static_cast<ino_t>( get_ino() );
        stbuf.st_mode = get_mode();
        stbuf.st_nlink = get_nlink();
        stbuf.st_uid = get_uid();
        stbuf.st_gid = get_gid();
        stbuf.st_rdev = static_cast<dev_t>( get_rdev() );
        stbuf.st_size = static_cast<off_t>( get_size() );
        stbuf.st_atime = static_cast<time_t>( get_atime().as_unix_date_time_s() );
        stbuf.st_mtime = static_cast<time_t>( get_mtime().as_unix_date_time_s() );
        stbuf.st_ctime = static_cast<time_t>( get_ctime().as_unix_date_time_s() );
        stbuf.st_blksize = get_blksize();
        stbuf.st_blocks = get_blocks();
        return stbuf;
      }

      Stat& Stat::operator=( const struct stat& stbuf )
      {
        atime = DateTime( stbuf.st_atime );
        st_blksize = stbuf.st_blksize;
        st_blocks = stbuf.st_blocks;
        ctime = DateTime( stbuf.st_ctime );
        st_dev = stbuf.st_dev;
        st_gid = stbuf.st_gid;
        st_ino = stbuf.st_ino;
        st_mode = stbuf.st_mode;
        mtime = DateTime( stbuf.st_mtime );
        st_nlink = stbuf.st_nlink;
        st_rdev = stbuf.st_rdev;
        st_size = stbuf.st_size;
        st_uid = stbuf.st_uid;
        return *this;
      }
    }
  }
}
