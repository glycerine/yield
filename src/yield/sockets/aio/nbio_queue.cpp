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
      socket_t socket_ = socket_event->get_socket();
      SocketEvent::dec_ref(*socket_event);

      map<socket_t, SocketState*>::iterator socket_state_i
        = this->state.find(socket_);
      debug_assert_ne(socket_state_i, this->state.end());
      SocketState* socket_state = socket_state_i->second;

      uint16_t want_socket_events = 0;

      for (uint8_t aiocb_queue_i = 0; aiocb_queue_i < 4; ++aiocb_queue_i) {
        AIOCB* aiocb = socket_state->aiocb_queues[aiocb_queue_i];
        if (aiocb != NULL) {
          AIOCB::RetryStatus aiocb_retry_status;
          if (aiocb->get_socket().set_blocking_mode(false))
            aiocb_retry_status = aiocb->retry();
          else {
            aiocb_retry_status = AIOCB::RETRY_STATUS_ERROR;
            aiocb->set_error(Exception::get_last_error_code());
          }

          if (
            aiocb_retry_status == AIOCB::RETRY_STATUS_COMPLETE
            ||
            aiocb_retry_status == AIOCB::RETRY_STATUS_ERROR
          ) {
            if (aiocb->get_next_aiocb() == NULL) {
              socket_state->aiocb_queues[aiocb_queue_i] = NULL;
              bool have_more_aiocbs = false;
              for (aiocb_queue_i = 0; aiocb_queue_i < 4; ++aiocb_queue_i) {
                if (socket_state->aiocb_queues[aiocb_queue_i] != NULL) {
                  have_more_aiocbs = true;
                  break;
                }
              }

              if (!have_more_aiocbs) {
                delete socket_state;
                this->state.erase(socket_state_i);
                socket_event_queue.dissociate(socket_);
              }
            } else {
              socket_state->aiocb_queues[aiocb_queue_i]
                = &aiocb->get_next_aiocb()->inc_ref();
              aiocb->set_next_aiocb(NULL);
            }

            return aiocb;
          } else if (aiocb_retry_status == AIOCB::RETRY_STATUS_WANT_READ) {
            want_socket_events |= POLLIN;
            break;
          } else if (aiocb_retry_status == AIOCB::RETRY_STATUS_WANT_WRITE) {
            want_socket_events |= POLLOUT;
            break;
          }
        }
      }

      debug_assert_ne(want_socket_events, 0);
      socket_event_queue.associate(socket_, want_socket_events);

      return NULL;
    }
    break;

    case acceptAIOCB::TYPE_ID:
    case connectAIOCB::TYPE_ID:
    case recvAIOCB::TYPE_ID:
    case sendAIOCB::TYPE_ID: {
      AIOCB* aiocb = static_cast<AIOCB*>(event);

      map<socket_t, SocketState*>::iterator socket_state_i
        = this->state.find(aiocb->get_socket());
      SocketState* socket_state;

      if (socket_state_i == this->state.end()) {
        AIOCB::RetryStatus aiocb_retry_status;
        if (aiocb->get_socket().set_blocking_mode(false))
          aiocb_retry_status = aiocb->retry();
        else {
          aiocb_retry_status = AIOCB::RETRY_STATUS_ERROR;
          aiocb->set_error(Exception::get_last_error_code());
        }

        if (
          aiocb_retry_status == AIOCB::RETRY_STATUS_COMPLETE
          ||
          aiocb_retry_status == AIOCB::RETRY_STATUS_ERROR
        )
          return aiocb;
        else if (aiocb_retry_status == AIOCB::RETRY_STATUS_WANT_READ)
          socket_event_queue.associate(aiocb->get_socket(), POLLIN);
        else if (aiocb_retry_status == AIOCB::RETRY_STATUS_WANT_WRITE)
          socket_event_queue.associate(aiocb->get_socket(), POLLOUT);

        socket_state = new SocketState();
        this->state[aiocb->get_socket()] = socket_state;
      } else
        socket_state = socket_state_i->second;

      uint8_t aiocb_queue_i;
      switch (aiocb->get_type_id()) {
      case acceptAIOCB::TYPE_ID: aiocb_queue_i = 0; break;
      case connectAIOCB::TYPE_ID: aiocb_queue_i = 1; break;
      case recvAIOCB::TYPE_ID: aiocb_queue_i = 3; break;
      case sendAIOCB::TYPE_ID: aiocb_queue_i = 2; break;
      default: DebugBreak(); aiocb_queue_i = 0; break;
      }

      if (socket_state->aiocb_queues[aiocb_queue_i] == NULL)
        socket_state->aiocb_queues[aiocb_queue_i] = aiocb;
      else {
        AIOCB* last_aiocb = socket_state->aiocb_queues[aiocb_queue_i];
        while (last_aiocb->get_next_aiocb() != NULL)
          last_aiocb = last_aiocb->get_next_aiocb();
        last_aiocb->set_next_aiocb(aiocb);
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
