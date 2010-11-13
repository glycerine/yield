// yield/fs/darwin/extended_attributes.cpp

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


#include "extended_attributes.hpp"

#include <sys/xattr.h>


namespace yield
{
  namespace fs
  {
    namespace darwin
    {
      ExtendedAttributes( fd_t fd )
        : fd( fd )
      { }

      ExtendedAttributes( const Path& path )
        : fd( -1 ), path( path )
      { }

      ~ExtendedAttributes()
      {
        if ( fd != -1 )
          close( fd );
      }

      ssize_t ExtendedAttributes::get( const char* name, void* value, size_t size )
      {
        if ( fd != -1 )
          return fgetxattr( fd, name, value, size, 0, 0 );
        else
          return getxattr( path.c_str(), name, value, size, 0, 0 );
      }

      bool ExtendedAttributes::list( vector<string>& out_names )
      {
        size_t names_len;
        if ( fd != -1 )
          names_len = flistxattr( fd, NULL, 0, 0 );
        else
          names_len = listxattr( path.c_str(), NULL, 0 );

        if ( names_len > 0 )
        {
          char* names = new char[names_len];

          if ( fd != -1 )
            flistxattr( fd, names, names_len, 0 );
          else
            listxattr( path.c_str(), names, names_len, 0 );

          char* name = names;
          do
          {
            size_t name_len = strlen( name );
            out_names.push_back( string( name, name_len ) );
            name += name_len;
          }
          while ( static_cast<size_t>( name - names ) < names_len );
          delete [] names;
        }
        return true;
      }

      bool ExtendedAttributes::remove( const char* name )
      {
        if ( fd != -1 )
          return fremovexattr( fd, name, 0 ) != -1;
        else
          return removexattr( path.c_str(), name, 0 ) != -1;
      }

      bool
      ExtendedAttributes::set
      (
        const char* name,
        const void* value,
        size_t size,
        int flags
      )
      {
        if ( fd != -1 )
          return fsetxattr( fd, name, value, size, 0, flags ) != -1;
        else
          return setxattr( path, name, value, size, 0, flags ) != -1;
      }
    }
  }
}
