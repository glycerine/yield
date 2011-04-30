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
#include "yield/buffer.hpp"
#include "yield/exception.hpp"
#include "yield/log.hpp"
#include "yield/time.hpp"
#include "yield/sockets/stream_socket.hpp"
#include "yield/sockets/aio/accept_aiocb.hpp"
#include "yield/sockets/aio/nbio_queue.hpp"
#include "yield/sockets/aio/connect_aiocb.hpp"
#include "yield/sockets/aio/recv_aiocb.hpp"
#include "yield/sockets/aio/send_aiocb.hpp"
#include "yield/sockets/aio/sendfile_aiocb.hpp"
#include "yield/sockets/poll/socket_event.hpp"

namespace yield {
namespace sockets {
namespace aio {
using std::map;
using yield::sockets::poll::SocketEvent;

NBIOQueue::NBIOQueue(YO_NEW_REF Log* log) : log(log) {
}

NBIOQueue::~NBIOQueue() {
  Log::dec_ref(log);
}

Event* NBIOQueue::dequeue(const Time& timeout) {
  Time timeout_remaining = timeout;

  for (;;) {
    Time start_time = Time::now();

    Event* event = socket_event_queue.dequeue(timeout_remaining);

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

        for (uint8_t aiocb_priority = 0; aiocb_priority < 4; ++aiocb_priority) {
          AIOCB* aiocb = socket_state->aiocb_queues[aiocb_priority];
          if (aiocb != NULL) {
            RetryStatus aiocb_retry_status = retry(*aiocb);

            if (
              aiocb_retry_status == RETRY_STATUS_COMPLETE
              ||
              aiocb_retry_status == RETRY_STATUS_ERROR
            ) {
              if (aiocb->get_next_aiocb() == NULL) {
                socket_state->aiocb_queues[aiocb_priority] = NULL;

                if (socket_state->empty()) {
                  delete socket_state;
                  this->state.erase(socket_state_i);
                  socket_event_queue.dissociate(socket_);
                }
              } else {
                socket_state->aiocb_queues[aiocb_priority]
                  = &aiocb->get_next_aiocb()->inc_ref();
                aiocb->set_next_aiocb(NULL);
              }

              return aiocb;
            } else if (aiocb_retry_status == RETRY_STATUS_WANT_READ) {
              want_socket_events |= POLLIN;
              break;
            } else if (aiocb_retry_status == RETRY_STATUS_WANT_WRITE) {
              want_socket_events |= POLLOUT;
              break;
            }
          }
        }

        debug_assert_ne(want_socket_events, 0);
        socket_event_queue.associate(socket_, want_socket_events);
      }
      break;

      case acceptAIOCB::TYPE_ID:
      case connectAIOCB::TYPE_ID:
      case recvAIOCB::TYPE_ID:
      case sendAIOCB::TYPE_ID:
      case sendfileAIOCB::TYPE_ID: {
        AIOCB* aiocb = static_cast<AIOCB*>(event);

        map<socket_t, SocketState*>::iterator socket_state_i
          = this->state.find(aiocb->get_socket());

        if (socket_state_i == this->state.end()) {
          switch (retry(*aiocb)) {
          case RETRY_STATUS_COMPLETE:
          case RETRY_STATUS_ERROR: return aiocb;
          case RETRY_STATUS_WANT_READ:
            socket_event_queue.associate(aiocb->get_socket(), POLLIN); break;
          case RETRY_STATUS_WANT_WRITE:
            socket_event_queue.associate(aiocb->get_socket(), POLLOUT); break;
          }

          SocketState* socket_state = new SocketState();
          uint8_t aiocb_priority = get_aiocb_priority(*aiocb);
          socket_state->aiocb_queues[aiocb_priority] = aiocb;
          this->state[aiocb->get_socket()] = socket_state;
        } else {
          SocketState* socket_state = socket_state_i->second;

          uint8_t aiocb_priority = get_aiocb_priority(*aiocb);

          // Check if there's already an AIOCB with an equal or higher priority
          // on this socket. If not, retry aiocb.
          bool should_retry_aiocb = true;
          for (
            int8_t check_aiocb_priority = aiocb_priority;
            check_aiocb_priority >= 0;
            --check_aiocb_priority
          ) {
            if (socket_state->aiocb_queues[check_aiocb_priority] != NULL) {
              should_retry_aiocb = false;
              break;
            }
          }

          if (should_retry_aiocb) {
            switch (retry(*aiocb)) {
            case RETRY_STATUS_COMPLETE:
            case RETRY_STATUS_ERROR:
              debug_assert_false(socket_state->empty()); return aiocb;
            case RETRY_STATUS_WANT_READ:
              socket_event_queue.associate(aiocb->get_socket(), POLLIN); break;
            case RETRY_STATUS_WANT_WRITE:
              socket_event_queue.associate(aiocb->get_socket(), POLLOUT); break;
            }
          }

          if (socket_state->aiocb_queues[aiocb_priority] == NULL)
            socket_state->aiocb_queues[aiocb_priority] = aiocb;
          else {
            AIOCB* last_aiocb = socket_state->aiocb_queues[aiocb_priority];
            while (last_aiocb->get_next_aiocb() != NULL)
              last_aiocb = last_aiocb->get_next_aiocb();
            last_aiocb->set_next_aiocb(aiocb);
          }
        }
      }
      break;

      default:
        return event;
      }
    }

    Time elapsed_time = Time::now() - start_time;
    if (timeout_remaining > elapsed_time)
      timeout_remaining -= elapsed_time;
    else
      return NULL;
  }
}

bool NBIOQueue::enqueue(Event& event) {
  return socket_event_queue.enqueue(event);
}

uint8_t NBIOQueue::get_aiocb_priority(const AIOCB& aiocb) {
  switch (aiocb.get_type_id()) {
  case acceptAIOCB::TYPE_ID: return 0;
  case connectAIOCB::TYPE_ID: return 1;
  case recvAIOCB::TYPE_ID: return 3;
  case sendAIOCB::TYPE_ID: return 2;
  case sendfileAIOCB::TYPE_ID: return 2;
  default: DebugBreak(); return 0;
  }
}

template <class AIOCBType> void NBIOQueue::log_completion(AIOCBType& aiocb) {
  if (log != NULL) {
    log->get_stream(Log::Level::DEBUG) <<
      get_type_name() << ": completed " << aiocb;
  }
}

template <class AIOCBType> void NBIOQueue::log_error(AIOCBType& aiocb) {
  if (log != NULL) {
    log->get_stream(Log::Level::DEBUG) <<
      get_type_name() << ": error on " << aiocb;
  }
}

template <class AIOCBType> void NBIOQueue::log_retry(AIOCBType& aiocb) {
  if (log != NULL) {
    log->get_stream(Log::Level::DEBUG) <<
      get_type_name() << ": retrying " << aiocb;
  }
}

template <class AIOCBType>
void NBIOQueue::log_wouldblock(AIOCBType& aiocb, RetryStatus retry_status) {
  if (log != NULL) {
    const char* retry_status_str;
    switch (retry_status) {
      case RETRY_STATUS_WANT_READ: retry_status_str = "read"; break;
      case RETRY_STATUS_WANT_WRITE: retry_status_str = "write"; break;
      default: DebugBreak(); retry_status_str = ""; break;
    }

    log->get_stream(Log::Level::DEBUG) << 
      get_type_name() << ": " <<
        aiocb << " would block on " << retry_status_str;
  }
}

NBIOQueue::RetryStatus NBIOQueue::retry(AIOCB& aiocb) {
  switch (aiocb.get_type_id()) {
  case acceptAIOCB::TYPE_ID: return retry(static_cast<acceptAIOCB&>(aiocb));
  case connectAIOCB::TYPE_ID: return retry(static_cast<connectAIOCB&>(aiocb));
  case recvAIOCB::TYPE_ID: return retry(static_cast<recvAIOCB&>(aiocb));
  case sendAIOCB::TYPE_ID: return retry(static_cast<sendAIOCB&>(aiocb));
  case sendfileAIOCB::TYPE_ID: return retry(static_cast<sendfileAIOCB&>(aiocb));
  default: DebugBreak(); return RETRY_STATUS_ERROR;
  }
}

NBIOQueue::RetryStatus NBIOQueue::retry(acceptAIOCB& accept_aiocb) {
  log_retry(accept_aiocb);

  if (accept_aiocb.get_socket().set_blocking_mode(false)) {
    auto_Object<SocketAddress> peername = new SocketAddress;
    StreamSocket* accepted_socket
      = accept_aiocb.get_socket().accept(*peername);

    if (accepted_socket != NULL) {
      accept_aiocb.set_accepted_socket(*accepted_socket);
      accept_aiocb.set_peername(&peername->inc_ref());

      if (accept_aiocb.get_recv_buffer() != NULL) {
        if (accepted_socket->set_blocking_mode(false)) {
          ssize_t recv_ret =
            accepted_socket->recv(*accept_aiocb.get_recv_buffer(), 0);
          if (recv_ret > 0)
            accept_aiocb.set_return(recv_ret);
          else if (recv_ret == 0) {
            accept_aiocb.set_error(0);
            accept_aiocb.set_return(-1);
          } else if (accepted_socket->want_recv())
            accept_aiocb.set_return(0);
        }
      } else
        accept_aiocb.set_return(0);

      log_completion(accept_aiocb);
      return RETRY_STATUS_COMPLETE;
    } else if (accept_aiocb.get_socket().want_accept()) {
      log_wouldblock(accept_aiocb, RETRY_STATUS_WANT_READ);
      return RETRY_STATUS_WANT_READ;
    }
  }

  accept_aiocb.set_error(Exception::get_last_error_code());
  log_error(accept_aiocb);
  return RETRY_STATUS_ERROR;
}

NBIOQueue::RetryStatus NBIOQueue::retry(connectAIOCB& connect_aiocb) {
  log_retry(connect_aiocb);

  if (connect_aiocb.get_socket().set_blocking_mode(false)) {
    if (connect_aiocb.get_socket().connect(connect_aiocb.get_peername())) {
      if (connect_aiocb.get_send_buffer() != NULL) {
        ssize_t send_ret
          = connect_aiocb.get_socket().send(*connect_aiocb.get_send_buffer(), 0);

        if (send_ret >= 0) {
          connect_aiocb.set_return(send_ret);
          log_completion(connect_aiocb);
          return RETRY_STATUS_COMPLETE;
        } else if (connect_aiocb.get_socket().want_send()) {
          log_wouldblock(connect_aiocb, RETRY_STATUS_WANT_WRITE);
          return RETRY_STATUS_WANT_WRITE;
        }
        else if (connect_aiocb.get_socket().want_recv()) {
          log_wouldblock(connect_aiocb, RETRY_STATUS_WANT_READ);
          return RETRY_STATUS_WANT_READ;
        }
        else {
          connect_aiocb.set_error(Exception::get_last_error_code());
          log_error(connect_aiocb);
          return RETRY_STATUS_ERROR;
        }
      } else {
        connect_aiocb.set_return(0);
        log_completion(connect_aiocb);
        return RETRY_STATUS_COMPLETE;
      }
    } else if (connect_aiocb.get_socket().want_connect()) {
      log_wouldblock(connect_aiocb, RETRY_STATUS_WANT_WRITE);
      return RETRY_STATUS_WANT_WRITE;
    }
  }

  connect_aiocb.set_error(Exception::get_last_error_code());
  log_error(connect_aiocb);
  return RETRY_STATUS_ERROR;
}

NBIOQueue::RetryStatus NBIOQueue::retry(recvAIOCB& recv_aiocb) {
  log_retry(recv_aiocb);

  if (recv_aiocb.get_socket().set_blocking_mode(false)) {
    ssize_t recv_ret
      = recv_aiocb.get_socket().recv(
          recv_aiocb.get_buffer(),
          recv_aiocb.get_flags(),
          &recv_aiocb.get_peername()
        );

    if (recv_ret >= 0) {
      recv_aiocb.set_return(recv_ret);
      log_completion(recv_aiocb);
      return RETRY_STATUS_COMPLETE;
    } else if (recv_aiocb.get_socket().want_recv()) {
      log_wouldblock(recv_aiocb, RETRY_STATUS_WANT_READ);
      return RETRY_STATUS_WANT_READ;
    }
    else if (recv_aiocb.get_socket().want_send()) {
      log_wouldblock(recv_aiocb, RETRY_STATUS_WANT_WRITE);
      return RETRY_STATUS_WANT_WRITE;
    }
  }

  recv_aiocb.set_error(Exception::get_last_error_code());
  log_error(recv_aiocb);
  return RETRY_STATUS_ERROR;
}

NBIOQueue::RetryStatus NBIOQueue::retry(sendAIOCB& send_aiocb) {
  log_retry(send_aiocb);

  if (send_aiocb.get_socket().set_blocking_mode(true)) {
    ssize_t send_ret
      = send_aiocb.get_socket().send(
          send_aiocb.get_buffer(),
          send_aiocb.get_flags(),
          send_aiocb.get_peername()
        );

    if (send_ret >= 0) {
      debug_assert_eq(
        static_cast<size_t>(send_ret),
        Buffers::size(send_aiocb.get_buffer())
      );
      send_aiocb.set_return(send_ret);
      log_completion(send_aiocb);
      return RETRY_STATUS_COMPLETE;
    } else if (send_aiocb.get_socket().want_send()) {
      log_wouldblock(send_aiocb, RETRY_STATUS_WANT_WRITE);
      return RETRY_STATUS_WANT_WRITE;
    }
    else if (send_aiocb.get_socket().want_recv()) {
      log_wouldblock(send_aiocb, RETRY_STATUS_WANT_READ);
      return RETRY_STATUS_WANT_READ;
    }
  }

  send_aiocb.set_error(Exception::get_last_error_code());
  log_error(send_aiocb);
  return RETRY_STATUS_ERROR;
}

NBIOQueue::RetryStatus NBIOQueue::retry(sendfileAIOCB& sendfile_aiocb) {
  log_retry(sendfile_aiocb);

  if (sendfile_aiocb.get_socket().set_blocking_mode(true)) {
    ssize_t sendfile_ret
      = sendfile_aiocb.get_socket().sendfile(
          sendfile_aiocb.get_fd(),
          sendfile_aiocb.get_offset(),
          sendfile_aiocb.get_nbytes()
        );

    if (sendfile_ret >= 0) {
      debug_assert_eq(
        static_cast<size_t>(sendfile_ret),
        sendfile_aiocb.get_nbytes()
      );
      sendfile_aiocb.set_return(sendfile_ret);
      log_completion(sendfile_aiocb);
      return RETRY_STATUS_COMPLETE;
    } else if (sendfile_aiocb.get_socket().want_send()) {
      log_wouldblock(sendfile_aiocb, RETRY_STATUS_WANT_WRITE);
      return RETRY_STATUS_WANT_WRITE;
    }
    else if (sendfile_aiocb.get_socket().want_recv()) {
      log_wouldblock(sendfile_aiocb, RETRY_STATUS_WANT_READ);
      return RETRY_STATUS_WANT_READ;
    }
  }

  sendfile_aiocb.set_error(Exception::get_last_error_code());
  log_error(sendfile_aiocb);
  return RETRY_STATUS_ERROR;
}
}
}
}
