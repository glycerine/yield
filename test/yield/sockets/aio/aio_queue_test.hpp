// aio_queue_test.hpp

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

#ifndef _YIELD_SOCKETS_AIO_AIO_QUEUE_TEST_HPP_
#define _YIELD_SOCKETS_AIO_AIO_QUEUE_TEST_HPP_

#include "../../event_queue_test.hpp"
#include "yield/assert.hpp"
#include "yield/auto_object.hpp"
#include "yield/buffer.hpp"
#include "yield/exception.hpp"
#include "yield/sockets/socket_pair.hpp"
#include "yield/sockets/aio/recv_aiocb.hpp"
#include "yield/sockets/aio/send_aiocb.hpp"
#include "yunit.hpp"


namespace yield {
namespace sockets {
namespace aio {
template <class AIOQueueType>
class AIOQueueTest : public yunit::Test {
public:
  // yunit::Test
  void setup() {
    aio_queue = new AIOQueueType;
  }

  void teardown() {
    delete aio_queue;
    aio_queue = NULL;
  }

protected:
  AIOQueueType& get_aio_queue() const {
    return *aio_queue;
  }

private:
  AIOQueueType* aio_queue;
};


template <class AIOQueueType>
class AIOQueueAssociateTest : public AIOQueueTest<AIOQueueType> {
public:
  // yunit::Test
  void run() {
    SocketPair sockets;
    if (!this->get_aio_queue().associate(sockets.first()))
      throw Exception();
  }
};


template <class AIOQueueType>
class AIOQueueRecvTest : public AIOQueueTest<AIOQueueType> {
public:
  // yunit::Test
  void run() {
    SocketPair sockets;
    if (!this->get_aio_queue().associate(sockets.first()))
      throw Exception();

    sockets.second().send("m", 1, 0);
    auto_Object<Buffer> buffer = new Buffer(4096);

    auto_Object<recvAIOCB> aiocb
    = new recvAIOCB(sockets.first(), buffer->inc_ref(), 0);

    if (!this->get_aio_queue().enqueue(aiocb->inc_ref()))
      throw Exception();

    auto_Object<recvAIOCB> out_aiocb
    = object_cast<recvAIOCB>(this->get_aio_queue().dequeue());
    throw_assert_eq(&out_aiocb.get(), &aiocb.get());
    throw_assert_eq(out_aiocb->get_error(), 0);
    throw_assert_eq(out_aiocb->get_return(), 1);
    throw_assert_eq(buffer->size(), 1);
    throw_assert_eq((*buffer)[0], 'm');
  }
};


template <class AIOQueueType>
class AIOQueueRecvQueuedTest : public AIOQueueTest<AIOQueueType> {
  // yunit::Test
  void run() {
    SocketPair sockets;
    if (!this->get_aio_queue().associate(sockets.first()))
      throw Exception();

    for (uint8_t i = 0; i < 2; i++) {
      recvAIOCB* aiocb = new recvAIOCB(sockets.first(), *new Buffer(2), 0);
      if (!this->get_aio_queue().enqueue(*aiocb))
        throw Exception();
    }

    // For the NBIOQueue: force the first retry
    {
      recvAIOCB* out_aiocb
        = object_cast<recvAIOCB>(this->get_aio_queue().trydequeue());
      throw_assert_eq(out_aiocb, NULL);
    }

    sockets.second().send("te", 2, 0);

    {
      auto_Object<recvAIOCB> out_aiocb
        = object_cast<recvAIOCB>(this->get_aio_queue().dequeue());
      throw_assert_eq(out_aiocb->get_error(), 0);
      throw_assert_eq(out_aiocb->get_return(), 2);
      throw_assert_eq(out_aiocb->get_buffer(), "te");
      throw_assert_eq(out_aiocb->get_buffer().size(), 2);
    }
      
    {
      recvAIOCB* out_aiocb
        = object_cast<recvAIOCB>(this->get_aio_queue().trydequeue());
      throw_assert_eq(out_aiocb, NULL);
    }

    sockets.second().send("st", 2, 0);

    {
      auto_Object<recvAIOCB> out_aiocb
        = object_cast<recvAIOCB>(this->get_aio_queue().dequeue());
      throw_assert_eq(out_aiocb->get_error(), 0);
      throw_assert_eq(out_aiocb->get_return(), 2);
      throw_assert_eq(out_aiocb->get_buffer(), "st");
      throw_assert_eq(out_aiocb->get_buffer().size(), 2);
    }
  }
};


template <class AIOQueueType>
class AIOQueueRecvSplitTest : public AIOQueueTest<AIOQueueType> {
public:
  // yunit::Test
  void run() {
    SocketPair sockets;
    if (!this->get_aio_queue().associate(sockets.first()))
      throw Exception();

    for (uint8_t i = 0; i < 2; i++) {
      recvAIOCB* aiocb = new recvAIOCB(sockets.first(), *new Buffer(2), 0);
      if (!this->get_aio_queue().enqueue(*aiocb))
        throw Exception();
    }

    // For the NBIOQueue: force the first retry
    {
      recvAIOCB* out_aiocb
        = object_cast<recvAIOCB>(this->get_aio_queue().trydequeue());
      throw_assert_eq(out_aiocb, NULL);
    }

    sockets.second().send("test", 4, 0);

    for (uint8_t i = 0; i < 2; i++) {
      auto_Object<recvAIOCB> out_aiocb
        = object_cast<recvAIOCB>(this->get_aio_queue().dequeue());
      throw_assert_eq(out_aiocb->get_error(), 0);
      throw_assert_eq(out_aiocb->get_return(), 2);
      throw_assert_eq(out_aiocb->get_buffer(), i == 0 ? "te" : "st");
      throw_assert_eq(out_aiocb->get_buffer().size(), 2);
    }
  }
};


template <class AIOQueueType>
class AIOQueueSendTest : public AIOQueueTest<AIOQueueType> {
public:
  // yunit::Test
  void run() {
    SocketPair sockets;
    if (!this->get_aio_queue().associate(sockets.first()))
      throw Exception();

    auto_Object<Buffer> buffer = new Buffer(4096);
    buffer->put('m', 512);

    auto_Object<sendAIOCB> aiocb
    = new sendAIOCB(sockets.first(), buffer->inc_ref(), 0);

    if (!this->get_aio_queue().enqueue(aiocb->inc_ref()))
      throw Exception();

    auto_Object<sendAIOCB> out_aiocb
    = object_cast<sendAIOCB>(this->get_aio_queue().dequeue());
    throw_assert_eq(&out_aiocb.get(), &aiocb.get());
    throw_assert_eq(out_aiocb->get_error(), 0);
    throw_assert_eq(out_aiocb->get_return(), 512);
  }
};


template <class AIOQueueType>
class AIOQueueTestSuite : public EventQueueTestSuite<AIOQueueType> {
public:
  AIOQueueTestSuite() {
    add("AIOQueue::associate", new AIOQueueAssociateTest<AIOQueueType>);

    add("AIOQueue::dequeue", new EventQueueDequeueTest<AIOQueueType>);

    add("AIOQueue + recv", new AIOQueueRecvTest<AIOQueueType>);
    add("AIOQueue + recv queued", new AIOQueueRecvQueuedTest<AIOQueueType>);
    add("AIOQueue + recv split", new AIOQueueRecvSplitTest<AIOQueueType>);

    add("AIOQueue + send", new AIOQueueSendTest<AIOQueueType>);

    add(
      "AIOQueue::timeddequeue",
      new EventQueueTimedDequeueTest<AIOQueueType>
    );

    add(
      "AIOQueue::trydequeue",
      new EventQueueTryDequeueTest<AIOQueueType>
    );
  }
};
}
}
}

#endif
