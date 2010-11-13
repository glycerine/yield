// yield/page_test.cpp

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


#include "buffer_test.hpp"
#include "yield/page.hpp"


TEST_SUITE( Page );

namespace yield
{
  TEST_EX( Page, capacity, BufferCapacityTest<Page> )
  {
    BufferCapacityTest<Page>::run();
  }

  TEST_EX( Page, copy, BufferCopyTest<Page> )
  {
    BufferCopyTest<Page>::run();
  }

  TEST_EX( Page, data, BufferDataTest<Page> )
  {
    BufferDataTest<Page>::run();
  }

  TEST_EX( Page, empty, BufferEmptyTest<Page> )
  {
    BufferEmptyTest<Page>::run();
  }

  TEST_EX( Page, get_next_buffer, BufferGetNextBufferTest<Page> )
  {
    BufferGetNextBufferTest<Page>::run();
  }

  TEST( Page, getpagesize )
  {
    size_t pagesize = Page::getpagesize();
    throw_assert_gt( pagesize, 0 );
    throw_assert_eq( pagesize % 4096, 0 );
  }

  TEST( Page, init_capacity )
  {
    size_t pagesize = Page::getpagesize();

    {
      auto_Object<Page> page = new Page( 1 );
      throw_assert_eq( page->capacity(), pagesize );
    }

    {
      auto_Object<Page> page = new Page( pagesize );
      throw_assert_eq( page->capacity(), pagesize );
    }

    {
      auto_Object<Page> page = new Page( 0 );
      throw_assert_eq( page->capacity(), 0 );
    }
  }

  TEST( Page, init_capacity_data_size )
  {
    auto_Object<Page> page
      = new Page( Page::getpagesize()*2, "test", 4 );
    throw_assert_eq( page->capacity(), Page::getpagesize()*2 );
    throw_assert_eq( strncmp( *page, "test", 4 ), 0 );
    throw_assert_eq( page->size(), 4 );
  }

  TEST_EX( Page, init_c_string, BufferInitCStringTest<Page> )
  {
    BufferInitCStringTest<Page>::run();
  }

  TEST_EX( Page, init_string, BufferInitStringTest<Page> )
  {
    BufferInitStringTest<Page>::run();
  }

  TEST( Page, is_page_aligned )
  {
    auto_Object<Page> page = new Page( Page::getpagesize() );
    throw_assert( Page::is_page_aligned( page->data() ) );
    iovec iov; iov.iov_base = page->data(); iov.iov_len = page->capacity();
    throw_assert( Page::is_page_aligned( iov ) );
  }

  TEST_EX( Page, operator_array, BufferOperatorArrayTest<Page> )
  {
    BufferOperatorArrayTest<Page>::run();
  }

  TEST_EX( Page, operator_cast, BufferOperatorCastTest<Page> )
  {
    BufferOperatorCastTest<Page>::run();
  }

  TEST_EX( Page, operator_equals, BufferOperatorEqualsTest<Page> )
  {
    BufferOperatorEqualsTest<Page>::run();
  }

  TEST_EX( Page, put_page, BufferPutBufferTest<Page> )
  {
    BufferPutBufferTest<Page>::run();
  }

  TEST_EX( Page, put_char, BufferPutCharTest<Page> )
  {
    BufferPutCharTest<Page>::run();
  }

  TEST_EX( Page, put_c_string, BufferPutCStringTest<Page> )
  {
    BufferPutCStringTest<Page>::run();
  }

  TEST_EX( Page, put_c_string_size, BufferPutCStringSizeTest<Page> )
  {
    BufferPutCStringSizeTest<Page>::run();
  }

  TEST_EX( Page, put_iovec, BufferPutIOVecTest<Page> )
  {
    BufferPutIOVecTest<Page>::run();
  }

  TEST_EX( Page, put_string, BufferPutStringTest<Page> )
  {
    BufferPutStringTest<Page>::run();
  }

  TEST( Page, reserve )
  {
    auto_Object<Page> page = new Page( 0 );
    throw_assert_eq( page->capacity(), 0 );

    page->reserve( 0 );
    throw_assert_eq( page->capacity(), 0 );

    page->reserve( 1 );
    throw_assert_eq( page->capacity(), Page::getpagesize() );

    page->reserve( page->capacity() + 1 );
    throw_assert_eq( page->capacity(), Page::getpagesize()*2 );

    page->reserve( Page::getpagesize() );
    throw_assert_eq( page->capacity(), Page::getpagesize()*2 );
  }

  TEST_EX( Page, resize, BufferResizeTest<Page> )
  {
    BufferResizeTest<Page>::run();
  }

  TEST( Page, rtti )
  {
    throw_assert_eq( Page( Page::getpagesize() ).get_type_id(), Page::TYPE_ID );
    throw_assert_eq
    (
      strcmp( Page( Page::getpagesize() ).get_type_name(), "yield::Page" ),
      0
    );
  }

  TEST_EX( Page, set_next_buffer, BufferSetNextBufferTest<Page> )
  {
    BufferSetNextBufferTest<Page>::run();
  }

  TEST_EX( Page, size, BufferSizeTest<Page> )
  {
    BufferSizeTest<Page>::run();
  }
}
