// yield/page.cpp

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


#include "yield/page.hpp"

#ifdef _WIN32
#include <Windows.h>
#else
#include <stdlib.h>
#endif


namespace yield {
size_t Page::pagesize = 0;


Page::Page()
  : Buffer(0) {
  data_ = NULL;
  reserve(getpagesize());
}

Page::Page(size_t capacity)
  : Buffer(0) {
  data_ = NULL;
  reserve(capacity);
}

Page::Page(const string& data)
  : Buffer(0) {
  data_ = NULL;
  put(data);
}

Page::Page(const void* data, size_t size)
  : Buffer(0) {
  data_ = NULL;
  put(data, size);
}

Page::Page(size_t capacity, const void* data, size_t size)
  : Buffer(0) {
  data_ = NULL;
  reserve(capacity);
  put(data, size);
}

Page::Page(const Buffer& buffer)
  : Buffer(0) {
  data_ = NULL;
  reserve(buffer.capacity());
  put(buffer);
}

Page::Page(const Page& page)
  : Buffer(0) {
  data_ = NULL;
  reserve(page.capacity());
  put(page);
}

Page::~Page() {
#ifdef _WIN32
  _aligned_free(data_);
#else
  free(data_);
#endif
}

Page* Page::get_next_page() const {
  return static_cast<Page*>(get_next_buffer());
}

size_t Page::getpagesize() {
  if (pagesize != 0)
    return pagesize;
  else {
#ifdef _WIN32
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);
    pagesize = system_info.dwPageSize;
#else
    pagesize = ::getpagesize();
#endif
    return pagesize;
  }
}

bool Page::is_page_aligned(const void* ptr) {
  return (
           reinterpret_cast<const uintptr_t>(ptr)
           &
           (getpagesize() - 1)
         ) == 0;
}

bool Page::is_page_aligned(const iovec& iov) {
  return is_page_aligned(iov.iov_base)
         &&
         (iov.iov_len & (getpagesize() - 1)) == 0;
}

void Page::reserve(size_t new_capacity) {
  if (new_capacity > capacity()) {
    size_t pagesize = getpagesize();
    new_capacity = (new_capacity + pagesize - 1) / pagesize * pagesize;

    void* new_data;
#ifdef _WIN32
    if ((new_data = _aligned_malloc(new_capacity, pagesize)) != NULL)
#else
    if (posix_memalign(&new_data, pagesize, new_capacity) == 0)
#endif
    {
      memcpy_s(new_data, new_capacity, data_, size());
#ifdef WIN32
      _aligned_free(data_);
#else
      free(data_);
#endif
      capacity_ = new_capacity;
      data_ = new_data;
    } else
      throw std::bad_alloc();
  }
}

void Page::set_next_page(YO_NEW_REF Page* page) {
  Buffer::set_next_buffer(page);
}

void Page::set_next_page(YO_NEW_REF Page& page) {
  Buffer::set_next_buffer(page);
}
}
