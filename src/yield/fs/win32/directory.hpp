// yield/fs/win32/directory.hpp

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


#ifndef _YIELD_FS_WIN32_DIRECTORY_HPP_
#define _YIELD_FS_WIN32_DIRECTORY_HPP_


#include "stat.hpp"
#include "yield/fs/directory.hpp"
#include "yield/fs/path.hpp"


namespace yield
{
  namespace fs
  {
    namespace win32
    {
      class Directory : public yield::fs::Directory
      {
      public:
        class Entry : public yield::fs::Directory::Entry
        {
        public:
          Entry() { }

          Entry& operator=( const WIN32_FIND_DATA& );

          // Directory::Entry
          const Path& get_name() const { return name; }
          bool is_hidden() const;
          bool is_special() const;

          // Stat
          const DateTime& get_atime() const { return stbuf.get_atime(); }
          uint32_t get_attributes() const { return stbuf.get_attributes(); }
          virtual const DateTime& get_ctime() const { return stbuf.get_ctime(); }
          const DateTime& get_mtime() const { return stbuf.get_mtime(); }
          int16_t get_nlink() const { return stbuf.get_nlink(); }
          uint64_t get_size() const { return stbuf.get_size(); }
          Type get_type() const { return stbuf.get_type(); }

        private:
          Path name;
          yield::fs::win32::Stat stbuf;
        };

      public:
        Directory( fd_t hDirectory );
        virtual ~Directory();

        operator fd_t() const { return hDirectory; }

        // Object
        Directory& inc_ref() { return Object::inc_ref( *this ); }

        // Directory
        bool close();
        yield::fs::Directory::Entry* read( Entry::Type types );
        bool read( yield::fs::Directory::Entry&, Entry::Type types );
        void rewind();

      private:
        fd_t hDirectory, hFindFile;
      };
    }
  }
}


#endif
