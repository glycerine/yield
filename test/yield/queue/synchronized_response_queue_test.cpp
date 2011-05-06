// synchronized_response_queue_test.cpp

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

#include "yield/assert.hpp"
#include "yield/auto_object.hpp"
#include "yield/exception.hpp"
#include "yield/response.hpp"
#include "yield/queue/synchronized_response_queue.hpp"
#include "yunit.hpp"

TEST_SUITE(SynchronizedResponseQueue);

namespace yield {
namespace queue {
TEST(SynchronizedResponseQueue, dequeue) {
  SynchronizedResponseQueue<Response> response_queue;

  {
    response_queue.enqueue(*new Response);
    auto_Object<Response> response = response_queue.dequeue();
  }

  {
    response_queue.enqueue(*new Response);
    auto_Object<Response> response = response_queue.dequeue(1.0);
  }
}

TEST(SynchronizedResponseQueue, enqueue) {
  SynchronizedResponseQueue<Response> response_queue;

  bool enqueue_ret = response_queue.enqueue(*new Response);
  throw_assert(enqueue_ret);
  enqueue_ret = response_queue.enqueue(*new Response);
}
}
}
