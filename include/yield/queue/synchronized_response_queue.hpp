// yield/queue/synchronized_response_queue.hpp

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

#ifndef _YIELD_QUEUE_SYNCHRONIZED_RESPONSE_QUEUE_HPP_
#define _YIELD_QUEUE_SYNCHRONIZED_RESPONSE_QUEUE_HPP_

#include "yield/event_handler.hpp"
#include "yield/exception.hpp"
#include "yield/response.hpp"
#include "yield/queue/synchronized_queue.hpp"

namespace yield {
namespace queue {
template <class ResponseType>
class SynchronizedResponseQueue
  : public EventHandler,
    private SynchronizedQueue<Event> {
public:
  YO_NEW_REF ResponseType& dequeue() {
    Event& event = SynchronizedQueue<Event>::dequeue();
    return response_cast(event);
  }

  YO_NEW_REF ResponseType* dequeue(const Time& timeout) {
    Event* event = SynchronizedQueue<Event>::dequeue(timeout);
    if (event != NULL)
      return &response_cast(*event);
    else
      return NULL;
  }

  bool enqueue(YO_NEW_REF Event& event) {
    return SynchronizedQueue<Event>::enqueue(event);
  }

  YO_NEW_REF ResponseType* trydequeue() {
    Event* event = SynchronizedQueue<Event>::trydequeue();
    if (event != NULL)
      return &response_cast(*event);
    else
      return NULL;
  }

public:
  // yield::EventHandler
  void handle(YO_NEW_REF Event& event) {
    enqueue(event);
  }

private:
  YO_NEW_REF ResponseType& response_cast(YO_NEW_REF Event& event) {
    if (event.is_message()) {
      Message& message = static_cast<Message&>(event);

      if (!message.is_request()) {
        Response& response = static_cast<Response&>(message);

        if (response.get_type_id() == ResponseType::TYPE_ID)
          return static_cast<ResponseType&>(response);
        else if (response.is_exception()) {
          try {
            static_cast<Exception&>(response).rethrow();
            // Eliminate compiler warnings about control paths
            return static_cast<ResponseType&>(response);
          } catch (Exception&) {
            Response::dec_ref(response);
            throw;
          }
        }
      }
    }

    throw Exception("SynchronizedResponseQueue: dequeued unexpected response type");
  }
};
};
};

#endif
