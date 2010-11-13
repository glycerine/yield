// yield/fs/win32/volume.hpp

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


#ifndef _YIELD_FS_WIN32_VOLUME_HPP_
#define _YIELD_FS_WIN32_VOLUME_HPP_


#include "yield/fs/volume.hpp"


namespace yield
{
  namespace fs
  {
    namespace win32
    {
      class Volume : public yield::fs::Volume
      {
      public:
        static void*
        mmap
        (
          size_t length,
          int prot,
          int flags,
          fd_t fd,
          uint64_t offset,
          void*& out_hFileMapping
        );

        // Volume
        bool access( const Path&, int );
        bool isdir( const Path& );
        bool isfile( const Path& );
        YO_NEW_REF yield::fs::Stat* getattr( const Path& );
        bool link( const Path& old_path, const Path& new_path );
        bool mkdir( const Path&, mode_t );

        yield::fs::File*
        mkfifo
        (
          const Path&,
          uint32_t flags,
          mode_t mode
        );

        YO_NEW_REF yield::fs::MemoryMappedFile*
        mmap
        (
          YO_NEW_REF yield::fs::File& file,
          void *start,
          size_t length,
          int prot,
          int flags,
          uint64_t offset
        );

        YO_NEW_REF yield::fs::File*
        open
        (
          const Path& path,
          uint32_t flags,
          mode_t mode,
          uint32_t attributes
        );

        YO_NEW_REF yield::fs::Directory* opendir( const Path& );
        YO_NEW_REF ExtendedAttributes* openxattrs( const Path& );
        bool readlink( const Path&, Path& );
        bool realpath( const Path&, OUT Path& );
        bool rename( const Path& from_path, const Path& to_path );
        bool rmdir( const Path& path );
        bool setattr( const Path&, const yield::fs::Stat& );
        bool statvfs( const Path&, struct statvfs& );
        bool symlink( const Path&, const Path& );
        bool truncate( const Path&, uint64_t );
        bool unlink( const Path& path );
        bool volname( const Path&, OUT Path& );
      };
    }
  }
}


#endif
