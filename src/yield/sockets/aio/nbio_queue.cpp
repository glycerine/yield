// yield/sockets/aio/nbio_queue.cpp

// Copyright (c) 2011 Minor Gordon
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

#include "yield/assert.hpp"
#include "yield/exception.hpp"
#include "yield/time.hpp"
#include "yield/sockets/stream_socket.hpp"
#include "yield/sockets/aio/accept_aiocb.hpp"
#include "yield/sockets/aio/nbio_queue.hpp"
#include "yield/sockets/aio/connect_aiocb.hpp"
#include "yield/sockets/aio/recv_aiocb.hpp"
#include "yield/sockets/aio/send_aiocb.hpp"
#include "yield/sockets/poll/socket_event.hpp"

namespace yield {
namespace sockets {
namespace aio {
using std::map;
using yield::sockets::poll::SocketEvent;

Event* NBIOQueue::dequeue(const Time& timeout) {
  Event* event = socket_event_queue.dequeue(timeout);

  if (event != NULL) {
    switch (event->get_type_id()) {
    case SocketEvent::TYPE_ID: {
      SocketEvent* socket_event = static_cast<SocketEvent*>(event);

      map<socket_t, AIOCB*>::iterator issued_aiocb_i
        = issued_aiocbs.find(socket_event->get_socket());
      debug_assert_ne(issued_aiocb_i, issued_aiocbs.end());

      SocketEvent::dec_ref(*socket_event);

      AIOCB* aiocb = issued_aiocb_i->second;

      if (aiocb->get_socket().set_blocking_mode(false)) {
        switch (aiocb->retry()) {
        case AIOCB::RETRY_STATUS_COMPLETE:
        case AIOCB::RETRY_STATUS_ERROR: break; // Drop down to return aiocb

        case AIOCB::RETRY_STATUS_WANT_READ: {
          socket_event_queue.associate(aiocb->get_socket(), POLLIN);
          return NULL;
        }
        break;

        case AIOCB::RETRY_STATUS_WANT_WRITE: {
          socket_event_queue.associate(aiocb->get_socket(), POLLOUT);
          return NULL;
        }
        }
      } else
        aiocb->set_error(Exception::get_last_error_code());

      if (aiocb->get_next_aiocb() != NULL) {
        issued_aiocb_i->second = aiocb->get_next_aiocb();
        // We can't assume the next AIOCB is waiting on the same socket
        // events. The options are to (1) try it here and then store it 
        // away in a separate "completed AIOCBs queue" or (2) turn on all
        // socket events and wait until dequeue() is called again to retry
        // the next aiocb.
        // Given that multiple outstanding AIOCBs are somewhat rare anyway
        // we'll go with the simpler option, (2).
        socket_event_queue.associate(
          aiocb->get_socket(),
          POLLERR|POLLIN|POLLOUT
        );
      }
      else {
        issued_aiocbs.erase(issued_aiocb_i);
        socket_event_queue.dissociate(aiocb->get_socket());
      }

      return aiocb;
    }
    break;

    case acceptAIOCB::TYPE_ID:
    case connectAIOCB::TYPE_ID:
    case recvAIOCB::TYPE_ID:
    case sendAIOCB::TYPE_ID: {
      AIOCB* aiocb = static_cast<AIOCB*>(event);

      map<socket_t, AIOCB*>::iterator issued_aiocb_i
        = issued_aiocbs.find(aiocb->get_socket());

      if (issued_aiocb_i == issued_aiocbs.end()) {
        if (aiocb->get_socket().set_blocking_mode(false)) {
          switch (aiocb->retry()) {
            case AIOCB::RETRY_STATUS_COMPLETE:
            case AIOCB::RETRY_STATUS_ERROR: return aiocb;

            case AIOCB::RETRY_STATUS_WANT_READ: {
              socket_event_queue.associate(aiocb->get_socket(), POLLIN);
            }
            break; // Drop down to add aiocb to issued_aiocbs

            case AIOCB::RETRY_STATUS_WANT_WRITE: {
              socket_event_queue.associate(aiocb->get_socket(), POLLOUT);
            }
            break; // Drop down to add aiocb to issued_aiocbs
          }

          issued_aiocbs[aiocb->get_socket()] = aiocb;
        } else {
          aiocb->set_error(Exception::get_last_error_code());
          return aiocb;
        }
      }
      else { // Already an issued AIOCB for this socket; add aiocb to the queue
        AIOCB* issued_aiocb = issued_aiocb_i->second;
        while (issued_aiocb->get_next_aiocb() != NULL)
          issued_aiocb = issued_aiocb->get_next_aiocb();
        issued_aiocb->set_next_aiocb(aiocb);
      }

      return NULL;
    }
    break;

    default:
      return event;
    }
  } else
    return NULL;
}

bool NBIOQueue::enqueue(Event& event) {
  return socket_event_queue.enqueue(event);
}
}
}
}
