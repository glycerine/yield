// yield/fs/bsd/directory.cpp

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


#include "directory.hpp"
#include "yield/assert.hpp"


namespace yield
{
  namespace fs
  {
    namespace bsd
    {
      Directory::Directory( DIR* dirp, const Path& path )
        : yield::fs::posix::Directory( dirp, path )
      { }

      bool Directory::read
      (
        OUT yield::fs::Directory::Entry& entry,
        Entry::Type types
      )
      {
        dirent* dirent_;
        while ( ( dirent_ = readdir( *this ) ) != NULL )
        {
          Entry::Type entry_type;
          switch ( dirent_->d_type )
          {
            case DT_FIFO: entry_type = Entry::TYPE_FIFO; break;
            case DT_CHR: entry_type = Entry::TYPE_CHR; break;
            case DT_DIR: entry_type = Entry::TYPE_DIR; break;
            case DT_BLK: entry_type = Entry::TYPE_BLK; break;
            case DT_REG: entry_type = Entry::TYPE_REG; break;
            case DT_LNK: entry_type = Entry::TYPE_LNK; break;
            case DT_SOCK: entry_type = Entry::TYPE_SOCK; break;
            default: DebugBreak(); entry_type = Entry::INVALID_TYPE; break;
          }

          if ( ( entry_type & types ) == entry_type )
          {
            static_cast<Entry&>( entry ) = *dirent_; // To set entry.name
            Path entry_path( get_path() / entry.get_name() );

            struct stat stbuf;
            if ( stat( entry_path.c_str(), &stbuf ) != -1 )
            {
              static_cast<Entry&>( entry ) = stbuf;
              return true;
            }
          }
        }

        return false;
      }
    }
  }
}
