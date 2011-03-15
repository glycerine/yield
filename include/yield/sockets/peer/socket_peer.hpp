// yield/sockets/peer/socket_peer.hpp

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

#ifndef _YIELD_SOCKETS_PEER_SOCKET_PEER_HPP_
#define _YIELD_SOCKETS_PEER_SOCKET_PEER_HPP_

#include "yield/log.hpp"
#include "yield/sockets/aio/aio_queue.hpp"
#include "yield/stage/stage.hpp"

namespace yield {
namespace sockets {
namespace peer {
class SocketPeer : public yield::stage::Stage {
protected:
  SocketPeer(Log* error_log = NULL, Log* trace_log = NULL)
    : yield::stage::Stage(*new yield::sockets::aio::AIOQueue),
      error_log(Object::inc_ref(error_log)),
      trace_log(Object::inc_ref(trace_log))
  { }

  virtual ~SocketPeer() {
    Log::dec_ref(error_log);
    Log::dec_ref(trace_log);
  }

public:
  yield::sockets::aio::AIOQueue& get_aio_queue() {
    return static_cast<yield::sockets::aio::AIOQueue&>(get_event_queue());
  }

  Log* get_error_log() const {
    return error_log;
  }

  Log* get_trace_log() const {
    return trace_log;
  }

private:
  Log* error_log;
  Log* trace_log;
};
}
}
}

#endif
