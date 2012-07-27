// yield/buffers.hpp

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

#ifndef _YIELD_BUFFERS_HPP_
#define _YIELD_BUFFERS_HPP_

#include "yield/types.hpp"

namespace yield {
class Buffer;

/**
  Static methods for manipulating linked lists of <code>Buffer</code>s
    (Buffer::get_next_buffer, Buffer::set_next_buffer).
*/
class Buffers {
public:
  /**
    Construct an array of iovecs for reading into a linked list of Buffers
      (scatter I/O).
    @param buffers a linked list of <code>Buffer</code>s
    @param[out] read_iovecs the resulting array of iovecs for reading
  */
  static void
  as_read_iovecs(
    Buffer& buffers,
    vector<iovec>& read_iovecs
  );

  /**
    Construct an array of iovecs for writing from a linked list of Buffers
      (gather I/O).
    @param buffers a linked list of <code>Buffer</code>s
    @param[out] write_iovecs the resulting array of iovecs for writing
  */
  static size_t
  as_write_iovecs(
    const Buffer& buffers,
    vector<iovec>& write_iovecs
  );

  /**
    Construct an array of iovecs for writing from a linked list of Buffers
      (gather I/O), starting from a byte offset into the [logically contiguous]
      buffer space.
    @param buffers a linked list of <code>Buffer</code>s
    @param offset byte offset into the buffer space
    @param[out] write_iovecs the resulting array of iovecs for writing
  */
  static size_t
  as_write_iovecs(
    const Buffer& buffers,
    size_t offset,
    vector<iovec>& write_iovecs
  );

public:
  /**
    Copy data into a linked list of <code>Buffer</code>s, increasing their sizes.
    @param buffers a linked list of <code>Buffer</code>s
    @param data data to write
    @param size size of data
  */
  static void put(Buffer& buffers, const void* data, size_t size);

public:
  /**
    Return the size of a linked list of <code>Buffer</code>s, defined
      as the sum of the sizes of the constituent <code>Buffer</code>s.
    @param buffers a linked list of <code>Buffer</code>s
    @return the summed size of the buffers
  */
  static size_t size(const Buffer& buffers);
};
}

#endif
