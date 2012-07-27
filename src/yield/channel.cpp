// yield/channel.cpp

// Copyright (c) 2012 Minor Gordon
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

#include "yield/buffer.hpp"
#include "yield/buffers.hpp"
#include "yield/channel.hpp"

namespace yield {
ssize_t Channel::read(Buffer& buffer) {
  if (buffer.get_next_buffer() == NULL) {
    ssize_t read_ret = read(buffer, buffer.capacity() - buffer.size());
    if (read_ret > 0) {
      buffer.put(NULL, static_cast<size_t>(read_ret));
    }
    return read_ret;
  } else {
    vector<iovec> iov;
    Buffers::as_read_iovecs(buffer, iov);
    ssize_t readv_ret = readv(&iov[0], iov.size());
    if (readv_ret > 0) {
      Buffers::put(buffer, NULL, static_cast<size_t>(readv_ret));
    }
    return readv_ret;
  }
}

ssize_t Channel::write(const Buffer& buffer) {
  if (buffer.get_next_buffer() == NULL) {
    return write(buffer, buffer.size());
  } else {
    vector<iovec> iov;
    Buffers::as_write_iovecs(buffer, iov);
    return writev(&iov[0], iov.size());
  }
}

ssize_t Channel::writev(const iovec* iov, int iovlen) {
  string buf;
  for (int iov_i = 0; iov_i < iovlen; ++iov_i) {
    buf.append(static_cast<char*>(iov[iov_i].iov_base), iov[iov_i].iov_len);
  }
  return write(buf.data(), buf.size());
}
}
