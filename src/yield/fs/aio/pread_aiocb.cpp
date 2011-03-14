// yield/aio/fs/pread_aiocb.cpp

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

#include "yield/exception.hpp"
#include "yield/page.hpp"
#include "yield/aio/fs/pread_aiocb.hpp"
#include "yield/fs/file.hpp"


namespace yield {
namespace aio {
namespace fs {
preadAIOCB::preadAIOCB
(
  yield::fs::File& file,
  YO_NEW_REF Page& page,
  size_t nbytes,
  uint64_t offset
)
  : AIOCB(file, page, nbytes, offset), page(page)
{ }

preadAIOCB::~preadAIOCB() {
  Page::dec_ref(page);
}

void preadAIOCB::set_return(ssize_t return_) {
  if (return_ >= 0) {
    Page* page = &get_page();
    for (;;) {
      size_t page_left = page->capacity() - page->size();

      if (static_cast<size_t>(return_) <= page_left) {
        page->resize(page->size() + return_);
        break;
      } else {
        page->resize(page->capacity());
        return_ -= page_left;
        page = page->get_next_page();
      }
    }
  }

  AIOCB::set_return(return_);
}

preadAIOCB::RetryStatus preadAIOCB::retry() {
  ssize_t return_;

  if (get_page().get_next_page() == NULL) {   // pread
    return_
    = get_file().pread
      (
        static_cast<char*>(get_page())
        + get_page().size(),
        get_page().capacity()
        - get_page().size(),
        get_offset()
      );
  } else { // preadv
    vector<iovec> iov;
    Page* page = &get_page();
    do {
      iovec page_iov;
      page_iov.iov_base
      = static_cast<char*>(*page) + page->size();
      page_iov.iov_len = page->capacity() - page->size();
      iov.push_back(page_iov);
      page = page->get_next_page();
    } while (page != NULL);

    return_
    = get_file().preadv
      (
        &iov[0],
        iov.size(),
        get_offset()
      );
  }

  if (return_ >= 0) {
    set_return(return_);
    return RETRY_STATUS_COMPLETE;
  } else {
    set_error(Exception::get_last_error_code());
    return RETRY_STATUS_ERROR;
  }
}
}
}
}
