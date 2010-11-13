// yield/thread/posix/pipe.cpp

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


#include "yield/thread/pipe.hpp"

#include <fcntl.h>


namespace yield
{
  namespace thread
  {
    bool Pipe::pipe( fd_t ends[2] )
    {
      return ::pipe( ends ) == 0;
    }


    bool Pipe::End::close()
    {
      if ( fd != -1 )
      {
        if ( ::close( fd ) != -1 )
        {
          fd = -1;
          return true;
        }
        else
          return false;
      }
      else
        return false;
    }

    ssize_t Pipe::End::read( void* buf, size_t buflen )
    {
      return ::read( *this, buf, buflen );
    }

    ssize_t Pipe::End::readv( const iovec* iov, int iovlen )
    {
      return ::readv( *this, iov, iovlen );
    }

    bool Pipe::End::set_blocking_mode( bool blocking_mode )
    {
      int current_fcntl_flags = fcntl( *this, F_GETFL, 0 );
      if ( blocking_mode )
      {
        if ( ( current_fcntl_flags & O_NONBLOCK ) == O_NONBLOCK )
          return fcntl( *this, F_SETFL, current_fcntl_flags ^ O_NONBLOCK ) != -1;
        else
          return true;
      }
      else
        return fcntl( *this, F_SETFL, current_fcntl_flags | O_NONBLOCK ) != -1;
    }

    ssize_t Pipe::End::write( const void* buf, size_t buflen )
    {
      return ::write( *this, buf, buflen );
    }

    ssize_t Pipe::End::writev( const iovec* iov, int iovlen )
    {
      return ::writev( *this, iov, iovlen );
    }
  }
}
