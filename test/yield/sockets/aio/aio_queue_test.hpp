// aio_queue_test.hpp

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

#ifndef _YIELD_SOCKETS_AIO_AIO_QUEUE_TEST_HPP_
#define _YIELD_SOCKETS_AIO_AIO_QUEUE_TEST_HPP_

#include "../../event_queue_test.hpp"
#include "yield/assert.hpp"
#include "yield/auto_object.hpp"
#include "yield/buffer.hpp"
#include "yield/exception.hpp"
#include "yield/fs/file.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/fs/stat.hpp"
#include "yield/sockets/stream_socket_pair.hpp"
#include "yield/sockets/tcp_socket.hpp"
#include "yield/sockets/aio/recv_aiocb.hpp"
#include "yield/sockets/aio/send_aiocb.hpp"
#include "yield/sockets/aio/sendfile_aiocb.hpp"
#include "yunit.hpp"

namespace yield {
namespace sockets {
namespace aio {
template <class AIOQueueType>
class AIOQueueAssociateTest : public yunit::Test {
public:
  // yunit::Test
  void run() {
    if (!AIOQueueType().associate(StreamSocketPair().first())) {
      throw Exception();
    }
  }
};


template <class AIOQueueType>
class AIOQueueRecvTest : public yunit::Test {
public:
  // yunit::Test
  void run() {
    AIOQueueType aio_queue;

    StreamSocketPair sockets;
    if (!aio_queue.associate(sockets.first())) {
      throw Exception();
    }

    sockets.second().send("m", 1, 0);

    auto_Object<Buffer> buffer = new Buffer(4096);
    auto_Object<recvAIOCB> aiocb
    = new recvAIOCB(sockets.first(), buffer->inc_ref(), 0);
    if (!aio_queue.enqueue(aiocb->inc_ref())) {
      throw Exception();
    }

    auto_Object<recvAIOCB> out_aiocb
    = Object::cast<recvAIOCB>(aio_queue.dequeue());
    throw_assert_eq(&out_aiocb.get(), &aiocb.get());
    throw_assert_eq(out_aiocb->get_error(), 0);
    throw_assert_eq(out_aiocb->get_return(), 1);
    throw_assert_eq(buffer->size(), 1);
    throw_assert_eq((*buffer)[0], 'm');
  }
};


template <class AIOQueueType>
class AIOQueueRecvMsgTest : public yunit::Test {
public:
  // yunit::Test
  void run() {
    AIOQueueType aio_queue;

    StreamSocketPair sockets;
    if (!aio_queue.associate(sockets.first())) {
      throw Exception();
    }

    sockets.second().send("mm", 2, 0);

    auto_Object<Buffer> buffer = new Buffer(1);
    buffer->set_next_buffer(new Buffer(1));
    auto_Object<recvAIOCB> aiocb
    = new recvAIOCB(sockets.first(), buffer->inc_ref(), 0);
    if (!aio_queue.enqueue(aiocb->inc_ref())) {
      throw Exception();
    }

    auto_Object<recvAIOCB> out_aiocb
    = Object::cast<recvAIOCB>(aio_queue.dequeue());
    throw_assert_eq(&out_aiocb.get(), &aiocb.get());
    throw_assert_eq(out_aiocb->get_error(), 0);
    throw_assert_eq(out_aiocb->get_return(), 2);
    throw_assert_eq(buffer->size(), 1);
    throw_assert_eq((*buffer)[0], 'm');
    throw_assert_eq(buffer->get_next_buffer()->size(), 1);
    throw_assert_eq((*buffer->get_next_buffer())[0], 'm');
  }
};


template <class AIOQueueType>
class AIOQueueRecvQueuedTest : public yunit::Test {
  // yunit::Test
  void run() {
    AIOQueueType aio_queue;

    StreamSocketPair sockets;
    if (!aio_queue.associate(sockets.first())) {
      throw Exception();
    }

    for (uint8_t i = 0; i < 2; i++) {
      recvAIOCB* aiocb = new recvAIOCB(sockets.first(), *new Buffer(2), 0);
      if (!aio_queue.enqueue(*aiocb)) {
        throw Exception();
      }
    }

    // For the NBIOQueue: force the first retry
    {
      recvAIOCB* out_aiocb
      = Object::cast<recvAIOCB>(aio_queue.trydequeue());
      throw_assert_eq(out_aiocb, NULL);
    }

    sockets.second().send("te", 2, 0);

    {
      auto_Object<recvAIOCB> out_aiocb
      = Object::cast<recvAIOCB>(aio_queue.dequeue());
      throw_assert_eq(out_aiocb->get_error(), 0);
      throw_assert_eq(out_aiocb->get_return(), 2);
      throw_assert_eq(out_aiocb->get_buffer(), "te");
      throw_assert_eq(out_aiocb->get_buffer().size(), 2);
    }

    {
      recvAIOCB* out_aiocb
      = Object::cast<recvAIOCB>(aio_queue.trydequeue());
      throw_assert_eq(out_aiocb, NULL);
    }

    sockets.second().send("st", 2, 0);

    {
      auto_Object<recvAIOCB> out_aiocb
      = Object::cast<recvAIOCB>(aio_queue.dequeue());
      throw_assert_eq(out_aiocb->get_error(), 0);
      throw_assert_eq(out_aiocb->get_return(), 2);
      throw_assert_eq(out_aiocb->get_buffer(), "st");
      throw_assert_eq(out_aiocb->get_buffer().size(), 2);
    }
  }
};


template <class AIOQueueType>
class AIOQueueRecvSplitTest : public yunit::Test {
public:
  // yunit::Test
  void run() {
    AIOQueueType aio_queue;

    StreamSocketPair sockets;
    if (!aio_queue.associate(sockets.first())) {
      throw Exception();
    }

    for (uint8_t i = 0; i < 2; i++) {
      recvAIOCB* aiocb = new recvAIOCB(sockets.first(), *new Buffer(2), 0);
      if (!aio_queue.enqueue(*aiocb)) {
        throw Exception();
      }
    }

    // For the NBIOQueue: force the first retry
    {
      recvAIOCB* out_aiocb
      = Object::cast<recvAIOCB>(aio_queue.trydequeue());
      throw_assert_eq(out_aiocb, NULL);
    }

    sockets.second().send("test", 4, 0);

    for (uint8_t i = 0; i < 2; i++) {
      auto_Object<recvAIOCB> out_aiocb
      = Object::cast<recvAIOCB>(aio_queue.dequeue());
      throw_assert_eq(out_aiocb->get_error(), 0);
      throw_assert_eq(out_aiocb->get_return(), 2);
      throw_assert_eq(out_aiocb->get_buffer(), i == 0 ? "te" : "st");
      throw_assert_eq(out_aiocb->get_buffer().size(), 2);
    }
  }
};


template <class AIOQueueType>
class AIOQueueSendTest : public yunit::Test {
public:
  // yunit::Test
  void run() {
    AIOQueueType aio_queue;

    StreamSocketPair sockets;
    if (!aio_queue.associate(sockets.first())) {
      throw Exception();
    }

    auto_Object<sendAIOCB> aiocb
    = new sendAIOCB(sockets.first(), Buffer::copy("test"), 0);
    if (!aio_queue.enqueue(aiocb->inc_ref())) {
      throw Exception();
    }

    auto_Object<sendAIOCB> out_aiocb
    = Object::cast<sendAIOCB>(aio_queue.dequeue());
    throw_assert_eq(&out_aiocb.get(), &aiocb.get());
    throw_assert_eq(out_aiocb->get_error(), 0);
    throw_assert_eq(out_aiocb->get_return(), 4);

    char test[4];
    ssize_t recv_ret = sockets.second().recv(test, 4, 0);
    throw_assert_eq(recv_ret, 4);
    throw_assert_eq(memcmp(test, "test", 4), 0);
  }
};


template <class AIOQueueType>
class AIOQueueSendFileTest : public yunit::Test {
public:
  // yunit::Test
  void run() {
    AIOQueueType aio_queue;

    StreamSocketPair sockets;
    if (!aio_queue.associate(sockets.first())) {
      throw Exception();
    }

    auto_Object<yield::fs::File> file
    = yield::fs::FileSystem().open("AIOQueueSendFileTest.txt");
    auto_Object<yield::fs::Stat> stbuf = file->stat();

    auto_Object<sendfileAIOCB> aiocb
    = new sendfileAIOCB(sockets.first(), *file);
    throw_assert_eq(aiocb->get_nbytes(), stbuf->get_size());
    throw_assert_eq(aiocb->get_offset(), 0);

    if (!aio_queue.enqueue(aiocb->inc_ref())) {
      throw Exception();
    }

    auto_Object<sendfileAIOCB> out_aiocb
    = Object::cast<sendfileAIOCB>(aio_queue.dequeue());
    throw_assert_eq(&out_aiocb.get(), &aiocb.get());
    throw_assert_eq(out_aiocb->get_error(), 0);
    throw_assert_eq(
      out_aiocb->get_return(),
      static_cast<ssize_t>(stbuf->get_size())
    );

    char test[4];
    ssize_t recv_ret = sockets.second().recv(test, 4, 0);
    throw_assert_eq(recv_ret, 4);
    throw_assert_eq(memcmp(test, "test", 4), 0);
  }

  void setup() {
    teardown();
    auto_Object<yield::fs::File> file
    = yield::fs::FileSystem().creat("AIOQueueSendFileTest.txt");
    file->write("test", 4);
    file->close();
  }

  void teardown() {
    yield::fs::FileSystem().unlink("AIOQueueSendFileTest.txt");
  }
};


template <class AIOQueueType>
class AIOQueueSendMsgTest : public yunit::Test {
public:
  // yunit::Test
  void run() {
    AIOQueueType aio_queue;

    StreamSocketPair sockets;
    if (!aio_queue.associate(sockets.first())) {
      throw Exception();
    }

    auto_Object<Buffer> buffer = Buffer::copy("test");
    buffer->set_next_buffer(Buffer::copy(" string"));
    auto_Object<sendAIOCB> aiocb
    = new sendAIOCB(sockets.first(), buffer->inc_ref(), 0);
    if (!aio_queue.enqueue(aiocb->inc_ref())) {
      throw Exception();
    }

    auto_Object<sendAIOCB> out_aiocb
    = Object::cast<sendAIOCB>(aio_queue.dequeue());
    throw_assert_eq(&out_aiocb.get(), &aiocb.get());
    throw_assert_eq(out_aiocb->get_error(), 0);
    throw_assert_eq(out_aiocb->get_return(), 11);

    char test_string[11];
    ssize_t recv_ret = sockets.second().recv(test_string, 11, 0);
    throw_assert_eq(recv_ret, 11);
    throw_assert_eq(memcmp(test_string, "test string", 11), 0);
  }
};


template <class AIOQueueType>
class AIOQueueTestSuite : public EventQueueTestSuite<AIOQueueType> {
public:
  AIOQueueTestSuite() {
    add("AIOQueue::associate", new AIOQueueAssociateTest<AIOQueueType>);

    add(
      "AIOQueue + recvAIOCB(Buffer)",
      new AIOQueueRecvTest<AIOQueueType>
    );

    add(
      "AIOQueue + recvAIOCB(Buffers)",
      new AIOQueueRecvMsgTest<AIOQueueType>
    );

    add(
      "AIOQueue + recvAIOCB queued",
      new AIOQueueRecvQueuedTest<AIOQueueType>
    );

    add(
      "AIOQueue + recvAIOCB split",
      new AIOQueueRecvSplitTest<AIOQueueType>
    );

    add(
      "AIOQueue + sendAIOCB(Buffer)",
      new AIOQueueSendTest<AIOQueueType>
    );

    add(
      "AIOQueue + sendAIOCB(Buffers)",
      new AIOQueueSendMsgTest<AIOQueueType>
    );

    add(
      "AIOQueue + sendfileAIOCB",
      new AIOQueueSendFileTest<AIOQueueType>
    );
  }
};
}
}
}

#endif
