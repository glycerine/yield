// yield/sockets/aio/sendfile_aiocb.hpp

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

#ifndef _YIELD_SOCKETS_AIO_SENDFILE_AIOCB_HPP_
#define _YIELD_SOCKETS_AIO_SENDFILE_AIOCB_HPP_

#include "yield/sockets/aio/aiocb.hpp"

namespace yield {
namespace sockets {
class streamSocket;

namespace aio {
class sendfileAIOCB : public AIOCB {
public:
  const static uint32_t TYPE_ID = 2151573397UL;

public:
  sendfileAIOCB(StreamSocket&, fd_t fd);
  sendfileAIOCB(StreamSocket&, fd_t fd, off_t offset, size_t nbytes);
  ~sendfileAIOCB();

public:
  fd_t get_fd() {
    return fd;
  }

  size_t get_nbytes() const {
    return nbytes;
  }

  StreamSocket& get_socket();

public:
  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }

  const char* get_type_name() const {
    return "yield::sockets::aio::sendfileAIOCB";
  }

private:
  void init(fd_t fd);

private:
  fd_t fd;
  size_t nbytes;
};

std::ostream& operator<<(std::ostream&, sendfileAIOCB&);
}
}
}

#endif