// queue_test.hpp

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

#ifndef _YIELD_QUEUE_QUEUE_TEST_HPP_
#define _YIELD_QUEUE_QUEUE_TEST_HPP_

#include "yield/assert.hpp"
#include "yield/types.hpp"
#include "yunit.hpp"


namespace yield {
namespace queue {
template <class QueueType>
class QueueTest : public yunit::Test { };


template <class QueueType>
class QueueCreateTest : public QueueTest<QueueType> {
public:
  // yunit::Test
  void run() {
    QueueType();
  }
};


template <class QueueType>
class QueueDequeueTest : public QueueTest<QueueType> {
public:
  // yunit::Test
  void run() {
    QueueType queue;

    uint32_t in_value = 1;
    throw_assert(queue.enqueue(in_value));
    uint32_t* out_value = queue.trydequeue();
    throw_assert_ne(out_value, NULL);
    throw_assert_eq(*out_value, in_value);
    throw_assert_eq(*out_value, 1);
    throw_assert_eq(queue.trydequeue(), NULL);
  }
};


template <class QueueType>
class QueueEnqueueTest : public QueueTest<QueueType> {
public:
  // yunit::Test
  void run() {
    QueueType queue;

    uint32_t in_value = 1;
    throw_assert(queue.enqueue(in_value));
  }
};


template <class QueueType>
class QueueFullTest : public QueueTest<QueueType> {
public:
  // yunit::Test
  void run() {
    QueueType queue;

    uint32_t in_values[] = { 0, 1, 2, 3, 4, 5, 6, 7 };

    for (unsigned char i = 0; i < 8; i++) {
      throw_assert(queue.enqueue(in_values[i]));
    }

    throw_assert_false(queue.enqueue(in_values[0]));

    for (unsigned char i = 0; i < 8; i++) {
      uint32_t* out_value = queue.trydequeue();
      throw_assert(*out_value == in_values[i]);
    }
  }
};


template <class QueueType>
class QueueTestSuite : public yunit::TestSuite {
public:
  QueueTestSuite() {
    add("queue::create", new QueueCreateTest<QueueType>);
    add("queue::dequeue", new QueueDequeueTest<QueueType>);
    add("queue::enqueue", new QueueEnqueueTest<QueueType>);
  }
};
}
}

#endif
