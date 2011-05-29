// yield/poll/fd_event.hpp

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

#ifndef _YIELD_POLL_FD_EVENT_HPP_
#define _YIELD_POLL_FD_EVENT_HPP_

#include "yield/event.hpp"

namespace yield {
namespace poll {
/**
  Event subclass describing file descriptor events
    (read and write readiness, errors).
*/
class FDEvent : public Event {
public:
  const static uint32_t TYPE_ID = 117149474;

public:
  typedef uint16_t Type;
  const static Type TYPE_ERROR;
  const static Type TYPE_HUP;
  const static Type TYPE_READ_READY;
  const static Type TYPE_WRITE_READY;

public:
  /**
    Construct an FDEvent from a file descriptor and a Type.
  */
  FDEvent(fd_t fd, Type type);

public:
  /**
    Get the file descriptor associated with this FDEvent.
    @return the file descriptor associated with this FDEvent
  */
  fd_t get_fd() const {
    return fd;
  }

  /**
    Get the Type of this FDEvent.
    @return the Type of this FDEvent
  */
  Type get_type() const {
    return type;
  }

public:
  // yield::Object
  virtual uint32_t get_type_id() const {
    return TYPE_ID;
  }

  virtual const char* get_type_name() const {
    return "yield::poll::FDEvent";
  }

private:
  fd_t fd;
  Type type;
};
}
}

#endif
