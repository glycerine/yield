// nbio_queue_test.cpp

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

#include "aio_queue_test.hpp"
#include "partial_send_stream_socket.hpp"
#include "yield/sockets/aio/nbio_queue.hpp"

namespace yield {
namespace sockets {
namespace aio {
INSTANTIATE_TYPED_TEST_CASE_P(NBIOQueue, AIOQueueTest, NBIOQueue);
INSTANTIATE_TYPED_TEST_CASE_P(NBIOQueue, EventQueueTest, NBIOQueue);

TEST(NBIOQueue, partial_send) {
  NBIOQueue aio_queue;

  StreamSocketPair sockets;
  auto_Object<PartialSendStreamSocket>
  partial_send_stream_socket = new PartialSendStreamSocket(sockets.first());
  if (!aio_queue.associate(sockets.first())) {
    throw Exception();
  }

  auto_Object<sendAIOCB> aiocb
  = new sendAIOCB(*partial_send_stream_socket, Buffer::copy("test"), 0);

  if (!aio_queue.enqueue(aiocb->inc_ref())) {
    throw Exception();
  }

  auto_Object<sendAIOCB> out_aiocb
  = Object::cast<sendAIOCB>(aio_queue.dequeue());
  ASSERT_EQ(&out_aiocb.get(), &aiocb.get());
  ASSERT_EQ(out_aiocb->get_error(), 0);
  ASSERT_EQ(out_aiocb->get_return(), 4);

  char test[4];
  ssize_t recv_ret = sockets.second().recv(test, 4, 0);
  ASSERT_EQ(recv_ret, 4);
  ASSERT_EQ(memcmp(test, "test", 4), 0);
}

class NBIOQueuePartialSendFileTest : public ::testing::Test {
public:
  void SetUp() {
    TearDown();
    auto_Object<yield::fs::File> file
    = yield::fs::FileSystem().creat("NBIOQueuePartialSendFileTest.txt");
    file->write("test", 4);
    file->close();
  }

  void TearDown() {
    yield::fs::FileSystem().unlink("NBIOQueuePartialSendFileTest.txt");
  }
};

TEST_F(NBIOQueuePartialSendFileTest, partial_sendfile) {
  NBIOQueue aio_queue;

  StreamSocketPair sockets;
  auto_Object<PartialSendStreamSocket>
  partial_send_stream_socket = new PartialSendStreamSocket(sockets.first());
  if (!aio_queue.associate(sockets.first())) {
    throw Exception();
  }

  auto_Object<yield::fs::File> file
  = yield::fs::FileSystem().open("NBIOQueuePartialSendFileTest.txt");
  auto_Object<yield::fs::Stat> stbuf = file->stat();

  auto_Object<sendfileAIOCB> aiocb
  = new sendfileAIOCB(*partial_send_stream_socket, *file);
  ASSERT_EQ(aiocb->get_nbytes(), stbuf->get_size());
  ASSERT_EQ(aiocb->get_offset(), 0);

  if (!aio_queue.enqueue(aiocb->inc_ref())) {
    throw Exception();
  }

  auto_Object<sendfileAIOCB> out_aiocb
  = Object::cast<sendfileAIOCB>(aio_queue.dequeue());
  ASSERT_EQ(&out_aiocb.get(), &aiocb.get());
  ASSERT_EQ(out_aiocb->get_error(), 0);
  ASSERT_EQ(
    out_aiocb->get_return(),
    static_cast<ssize_t>(stbuf->get_size())
  );

  char test[4];
  ssize_t recv_ret = sockets.second().recv(test, 4, 0);
  ASSERT_EQ(recv_ret, 4);
  ASSERT_EQ(memcmp(test, "test", 4), 0);
}

TEST(NBIOQueue, partial_sendmsg) {
  NBIOQueue aio_queue;

  StreamSocketPair sockets;
  auto_Object<PartialSendStreamSocket>
  partial_send_stream_socket = new PartialSendStreamSocket(sockets.first());
  if (!aio_queue.associate(sockets.first())) {
    throw Exception();
  }

  auto_Object<Buffer> buffer = Buffer::copy("test");
  buffer->set_next_buffer(Buffer::copy(" string"));
  auto_Object<sendAIOCB> aiocb
  = new sendAIOCB(*partial_send_stream_socket, buffer->inc_ref(), 0);
  if (!aio_queue.enqueue(aiocb->inc_ref())) {
    throw Exception();
  }

  auto_Object<sendAIOCB> out_aiocb
  = Object::cast<sendAIOCB>(aio_queue.dequeue());
  ASSERT_EQ(&out_aiocb.get(), &aiocb.get());
  ASSERT_EQ(out_aiocb->get_error(), 0);
  ASSERT_EQ(out_aiocb->get_return(), 11);

  char test_string[11];
  ssize_t recv_ret = sockets.second().recv(test_string, 11, 0);
  ASSERT_EQ(recv_ret, 11);
  ASSERT_EQ(memcmp(test_string, "test string", 11), 0);
}
}
}
}
