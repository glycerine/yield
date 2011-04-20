// yield/thread/pipe.hpp

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

#ifndef _YIELD_THREAD_PIPE_HPP_
#define _YIELD_THREAD_PIPE_HPP_

#include "yield/channel.hpp"
#include "yield/channel_pair.hpp"
#include "yield/exception.hpp"

namespace yield {
namespace thread {
class Pipe : public ChannelPair {
public:
  class End : public Channel {
  public:
    End(fd_t fd) : fd(fd)
    { }

    ~End() {
      close();
    }

  public:
    operator fd_t() const {
      return fd;
    }

  public:
    // yield::Channel
    bool close();
    ssize_t read(void* buf, size_t buflen);
    ssize_t readv(const iovec* iov, int iovlen);
    ssize_t write(const void* buf, size_t buflen);
    ssize_t writev(const iovec* iov, int iovlen);

  private:
    fd_t fd;
  };

public:
  Pipe() {
    fd_t ends[2];
    if (Pipe::pipe(ends)) {
      this->ends[0] = new End(ends[0]);
      this->ends[1] = new End(ends[1]);
    } else
      throw Exception();
  }

  ~Pipe() {
    Channel::dec_ref(*ends[0]);
    Channel::dec_ref(*ends[1]);
  }

  static YO_NEW_REF Pipe* create() {
    fd_t ends[2];
    if (Pipe::pipe(ends))
      return new Pipe(ends);
    else
      return NULL;
  }

  End& get_read_end() {
    return *ends[0];
  }

  End& get_write_end() {
    return *ends[1];
  }

  End& operator[](size_t n) {
    return *ends[n];
  }

  // yield::ChannelPair
  Channel& get_read_channel() {
    return get_read_end();
  }

  Channel& get_write_channel() {
    return get_write_end();
  }

private:
  Pipe(fd_t ends[2]) {
    this->ends[0] = new End(ends[0]);
    this->ends[1] = new End(ends[1]);
  }

  static bool pipe(fd_t ends[2]);

private:
  End* ends[2];
};
}
}

#endif
