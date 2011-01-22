// yield/aio/net/sockets/nbio_queue.cpp

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


#include "nbio_queue.hpp"
#include "yield/assert.hpp"
#include "yield/exception.hpp"
#include "yield/time.hpp"
#include "yield/aio/net/sockets/accept_aiocb.hpp"
#include "yield/aio/net/sockets/connect_aiocb.hpp"
#include "yield/aio/net/sockets/recv_aiocb.hpp"
#include "yield/aio/net/sockets/send_aiocb.hpp"
#include "yield/net/sockets/stream_socket.hpp"
#include "yield/poll/socket_event.hpp"


namespace yield {
namespace aio {
namespace net {
namespace sockets {
using std::map;
using yield::poll::SocketEvent;
using yield::net::sockets::SocketAddress;
using yield::net::sockets::StreamSocket;


Event* NBIOQueue::dequeue( const Time& timeout ) {
  Event* event = socket_event_queue.dequeue( timeout );

  if ( event != NULL ) {
    switch ( event->get_type_id() ) {
    case SocketEvent::TYPE_ID: {
      SocketEvent* socket_event = static_cast<SocketEvent*>( event );

      map<socket_t, AIOCB*>::iterator aiocb_i
      = issued_aiocbs.find( socket_event->get_socket() );
      debug_assert_ne( aiocb_i, issued_aiocbs.end() );

      SocketEvent::dec_ref( *socket_event );

      AIOCB* aiocb = aiocb_i->second;

      if ( aiocb->get_channel().set_blocking_mode( false ) ) {
        switch ( aiocb->retry() ) {
        case AIOCB::RETRY_STATUS_WANT_READ:
        case AIOCB::RETRY_STATUS_WANT_WRITE: {
          DebugBreak();
          return NULL;
        }
        }
      } else
        aiocb->set_error( Exception::get_last_error_code() );

      if ( aiocb->get_next_aiocb() != NULL )
        aiocb_i->second = aiocb->get_next_aiocb();
      else
        issued_aiocbs.erase( aiocb_i );

      return aiocb;
    }
    break;

    case acceptAIOCB::TYPE_ID:
    case connectAIOCB::TYPE_ID:
    case recvAIOCB::TYPE_ID:
    case sendAIOCB::TYPE_ID: {
      AIOCB* aiocb = static_cast<AIOCB*>( event );

      if ( aiocb->get_socket().set_blocking_mode( false ) ) {
        switch ( aiocb->retry() ) {
        case AIOCB::RETRY_STATUS_COMPLETE:
        case AIOCB::RETRY_STATUS_ERROR:
          return aiocb;

        default: {
          DebugBreak();

          map<socket_t, AIOCB*>::iterator aiocb_i
          = issued_aiocbs.find( aiocb->get_socket() );

          if ( aiocb_i != issued_aiocbs.end() ) {
            AIOCB* aiocb = aiocb_i->second;
            while ( aiocb->get_next_aiocb() != NULL )
              aiocb = aiocb->get_next_aiocb();
            aiocb->set_next_aiocb( aiocb );
          } else
            issued_aiocbs[aiocb->get_socket()] = aiocb;

          return NULL;
        }
        }
      } else {
        aiocb->set_error( Exception::get_last_error_code() );
        return aiocb;
      }
    }
    break;

    default:
      return event;
    }
  } else
    return NULL;
}

bool NBIOQueue::enqueue( Event& event ) {
  return socket_event_queue.enqueue( event );
}
}
}
}
}
