// yield/channel.hpp

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

#ifndef _YIELD_CHANNEL_HPP_
#define _YIELD_CHANNEL_HPP_

#include "yield/object.hpp"

namespace yield {
class Buffer;

/**
  An abstract class for read-write channels (files, sockets, etc.).
*/
class Channel : public Object {
public:
  /**
    Empty virtual destructor.
  */
  virtual ~Channel() { }

public:
  /**
    Close the channel.
    @return true if the close was successful
  */
  virtual bool close() = 0;

public:
  /**
    Read from the channel into a Buffer.
    May be a read(void*, size_t) or a readv(const iovec*, int),
      depending on whether buffer is a single buffer or a linked list of them.
    Updates buffer's size depending on how many bytes were read.
    @param[in, out] buffer the buffer to read data into
    @return the number of bytes read on success, -1+errno on failure
  */
  virtual ssize_t read(Buffer& buffer);

  /**
    Read from the channel into a single buffer.
    @param[in, out] buf pointer to the buffer
    @param buflen the length of the memory region pointed to by buf
    @return the number of bytes read on success, -1+errno on failure
  */
  virtual ssize_t read(void* buf, size_t buflen) = 0;

  /**
    Read from the channel into multiple buffers (scatter I/O).
    @param[in, out] iov array of I/O vectors describing the buffers
    @param iovlen length of the I/O vectors array
    @return the number of bytes read on success, -1+errno on failure
  */
  virtual ssize_t readv(const iovec* iov, int iovlen) = 0;

public:
  /**
    Write to the channel from a Buffer.
    May be a write(const void*, size_t) or a writev(const iovec*, int),
      depending on whether buffer is a single buffer or a linked list of them.
    @param buffer the buffer to write from
    @return the number of bytes written on success, -1+errno on failure
  */
  virtual ssize_t write(const Buffer& buffer);

  /**
    Write to the channel from a single buffer.
    @param buf pointer to the buffer
    @param buflen the length of the memory region pointed to by buf
    @return the number of bytes written on success, -1+errno on failure
  */
  virtual ssize_t write(const void* buf, size_t buflen) = 0;

  /**
    Write to the channel from multiple buffers (gather I/O).
    @param iov array of I/O vectors describing the buffers
    @param iovlen length of the I/O vectors array
    @return the number of bytes written on success, -1+errno on failure
  */
  virtual ssize_t writev(const iovec* iov, int iovlen);

public:
  // yield::Object
  Channel& inc_ref() {
    return Object::inc_ref(*this);
  }
};
}

#endif
