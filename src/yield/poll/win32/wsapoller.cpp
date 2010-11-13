// yield/poll/win32/wsapoller.cpp

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


#include "wsapoller.hpp"
#include "yield/assert.hpp"
#include "yield/exception.hpp"
#include "yield/poll/socket_event.hpp"


namespace yield
{
  namespace poll
  {
    namespace win32
    {
      using yield::thread::NonBlockingConcurrentQueue;


      #if _WIN32_WINNT >= 0x0600
        WSAPoller::WSAPoller()
        {
          associate( wake_socket_pair.first(), POLLIN );
        }

        bool
        WSAPoller::associate
        (
          socket_t socket_,
          int16_t events
        )
        {
          if ( events > 0 )
          {
            for
            (
              vector<pollfd>::iterator pollfd_i = pollfds.begin();
              pollfd_i != pollfds.end();
              ++pollfd_i
            )
            {
              if ( ( *pollfd_i ).fd == socket_ )
              {
                ( *pollfd_i ).events = events;
                return true;
              }
            }

            pollfd pollfd_;
            memset( &pollfd_, 0, sizeof( pollfd_ ) );
            pollfd_.fd = socket_;
            pollfd_.events = events;
            pollfds.push_back( pollfd_ );
            return true;
          }
          else
            return dissociate( socket_ );
        }

        Event* WSAPoller::dequeue( const Time& timeout )
        {
          int ret
            = WSAPoll
              (
                &pollfds[0],
                pollfds.size(),
                static_cast<int>( timeout.ms() )
              );

          if ( ret > 0 )
          {
            vector<pollfd>::const_iterator pollfd_i = pollfds.begin();

            do
            {
              const pollfd& pollfd_ = *pollfd_i;

              if ( pollfd_.revents != 0 )
              {
                if ( pollfd_.fd == wake_socket_pair.first() )
                  return NonBlockingConcurrentQueue<Event, 32>::trydequeue();
                else
                  return new SocketEvent( pollfd_.revents, pollfd_.fd );
              }
            } while ( ++pollfd_i < pollfds.end() );
          }

          return NULL;
        }

        bool WSAPoller::dissociate( socket_t socket_ )
        {
          for
          (
            vector<pollfd>::iterator pollfd_i = pollfds.begin();
            pollfd_i != pollfds.end();
            ++pollfd_i
          )
          {
            if ( ( *pollfd_i ).fd == socket_ )
            {
              pollfds.erase( pollfd_i );
              return true;
            }
          }

          return false;
        }

        bool WSAPoller::enqueue( Event& event )
        {
          bool ret = NonBlockingConcurrentQueue<Event, 32>::enqueue( event );
          debug_assert( ret );
          wake_socket_pair.second().write( "m", 1 );
          return ret;
        }
      #endif
    }
  }
}
