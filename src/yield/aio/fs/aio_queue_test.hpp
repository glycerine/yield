// yield/aio/fs/aio_queue_test.hpp

// Copyright (c) 2010 Minor Gordon
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


#ifndef _YIELD_AIO_FS_AIO_QUEUE_TEST_HPP_
#define _YIELD_AIO_FS_AIO_QUEUE_TEST_HPP_


#include "../../event_queue_test.hpp"
#include "yield/assert.hpp"
#include "yield/auto_object.hpp"
#include "yield/page.hpp"
#include "yield/exception.hpp"
#include "yield/aio/fs/pread_aiocb.hpp"
#include "yield/aio/fs/pwrite_aiocb.hpp"
#include "yield/fs/file.hpp"
#include "yield/fs/path.hpp"
#include "yield/fs/volume.hpp"
#include "yunit.hpp"

#include <fcntl.h>


namespace yield {
namespace aio {
namespace fs {
using yield::fs::File;
using yield::fs::Path;
using yield::fs::Volume;


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
    volume->unlink( get_test_file_name() );
  }

protected:
  AIOQueueTest()
    : test_buffer( "aio_queue_test" ),
      test_file_name( "aio_queue_test.txt" ) {
    volume = Volume::create();
  }

  ~AIOQueueTest() {
    delete volume;
  }

  AIOQueueType& get_aio_queue() const {
    return *aio_queue;
  }
  Page& get_test_buffer() {
    return test_buffer;
  }
  const Path& get_test_file_name() const {
    return test_file_name;
  }
  Volume& get_volume() const {
    return *volume;
  }

private:
  AIOQueueType* aio_queue;
  Page test_buffer;
  Path test_file_name;
  Volume* volume;
};


template <class AIOQueueType>
class AIOQueuePReadTest : public AIOQueueTest<AIOQueueType> {
public:
  // yunit::Test
  void run() {
    Event* null_event = this->get_aio_queue().trydequeue();
    throw_assert_eq( null_event, NULL );

    {
      auto_Object<File> file
      = this->get_volume().creat( this->get_test_file_name() );

      file->write
      (
        this->get_test_buffer(),
        this->get_test_buffer().size()
      );
    }

    auto_Object<File> file
    = this->get_volume().open
      (
        this->get_test_file_name(),
        O_ASYNC|O_RDONLY
      );

    if ( !this->get_aio_queue().associate( *file ) )
      throw Exception();

    auto_Object<Page> page = new Page;

    auto_Object<preadAIOCB> aiocb
    = new preadAIOCB( *file, page->inc_ref(), page->capacity(), 0 );

    if ( !this->get_aio_queue().enqueue( aiocb->inc_ref() ) )
      throw Exception();

    auto_Object<preadAIOCB> out_aiocb
    = object_cast<preadAIOCB>( this->get_aio_queue().dequeue() );
    throw_assert_eq( &out_aiocb.get(), &aiocb.get() );
    throw_assert_eq( out_aiocb->get_error(), 0 );
    throw_assert_eq
    (
      static_cast<size_t>( out_aiocb->get_return() ),
      this->get_test_buffer().size()
    );
    throw_assert_eq( *page, this->get_test_buffer() );
  }
};


template <class AIOQueueType>
class AIOQueueWriteTest : public AIOQueueTest<AIOQueueType> {
public:
  // yunit::Test
  void run() {
    {
      auto_Object<File> file
      = this->get_volume().open
        (
          this->get_test_file_name(),
          O_ASYNC|O_CREAT|O_TRUNC|O_WRONLY
        );

      if ( !this->get_aio_queue().associate( *file ) )
        throw Exception();

      auto_Object<pwriteAIOCB> aiocb
      = new pwriteAIOCB
      (
        *file,
        this->get_test_buffer().inc_ref(),
        this->get_test_buffer().size(),
        0
      );

      if ( !this->get_aio_queue().enqueue( aiocb->inc_ref() ) )
        throw Exception();

      auto_Object<pwriteAIOCB> out_aiocb
      = object_cast<pwriteAIOCB>( this->get_aio_queue().dequeue() );
      throw_assert_eq( &out_aiocb.get(), &aiocb.get() );
      throw_assert_eq( out_aiocb->get_error(), 0 );
      throw_assert_eq
      (
        static_cast<size_t>( out_aiocb->get_return() ),
        this->get_test_buffer().size()
      );
    }

    {
      auto_Object<File> file
      = this->get_volume().open( this->get_test_file_name() );
      auto_Object<Page> page = new Page;
      ssize_t read_ret = file->read( *page, page->capacity() );
      throw_assert_gt( read_ret, 0 );
      throw_assert_eq
      (
        static_cast<size_t>( read_ret ),
        this->get_test_buffer().size()
      );
      page->resize( static_cast<size_t>( read_ret ) );
      throw_assert_eq( *page, this->get_test_buffer() );
    }
  }
};


template <class AIOQueueType>
class AIOQueueTestSuite : public EventQueueTestSuite<AIOQueueType> {
public:
  AIOQueueTestSuite() {
    add( "AIOQueue::dequeue", new EventQueueDequeueTest<AIOQueueType> );
    add( "AIOQueue + pread", new AIOQueuePReadTest<AIOQueueType> );
    add( "AIOQueue + pwrite", new AIOQueueWriteTest<AIOQueueType> );

    add
    (
      "AIOQueue::timeddequeue",
      new EventQueueTimedDequeueTest<AIOQueueType>
    );

    add
    (
      "AIOQueue::trydequeue",
      new EventQueueTryDequeueTest<AIOQueueType>
    );
  }
};
}
}
}


#endif
