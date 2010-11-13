// yield/net/sockets/posix/tcp_socket.cpp

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


#include "yield/net/sockets/tcp_socket.hpp"

#include <netinet/in.h> // For the IPPROTO_* constants
#include <netinet/tcp.h> // For the TCP_* constants
#include <sys/socket.h>


namespace yield
{
  namespace net
  {
    namespace sockets
    {
      int TCPSocket::DOMAIN_DEFAULT = AF_INET;
      int TCPSocket::PROTOCOL = IPPROTO_TCP;


      bool TCPSocket::setsockopt( Option option, bool onoff )
      {
        if ( option == OPTION_TCP_NODELAY )
        {
          int optval = onoff ? 1 : 0;
          return ::setsockopt
                 (
                   *this,
                   IPPROTO_TCP,
                   TCP_NODELAY,
                   reinterpret_cast<char*>( &optval ),
                   static_cast<int>( sizeof( optval ) )
                 ) == 0;
        }
        else
          return Socket::setsockopt( option, onoff );
      }
    }
  }
}
