// yield/aio/fs/pwrite_aiocb.cpp

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
#include "yield/aio/fs/pwrite_aiocb.hpp"
#include "yield/fs/file.hpp"


namespace yield {
namespace aio {
namespace fs {
pwriteAIOCB::pwriteAIOCB
(
  yield::fs::File& file,
  YO_NEW_REF Page& page,
  size_t nbytes,
  uint64_t offset
)
  : AIOCB(file, page, nbytes, offset),
    page(page)
{ }

pwriteAIOCB::~pwriteAIOCB() {
  Page::dec_ref(page);
}

pwriteAIOCB::RetryStatus pwriteAIOCB::retry() {
  ssize_t return_;

  if (get_page().get_next_page() == NULL) {   // pwrite
    return_
    = get_file().pwrite
      (
        get_page(),
        get_page().size(),
        get_offset()
      );
  } else { // pwritev
    vector<iovec> iov;
    Page* page = &get_page();
    do {
      iovec page_iov;
      page_iov.iov_base = *page;
      page_iov.iov_len = page->size();
      iov.push_back(page_iov);
      page = page->get_next_page();
    } while (page != NULL);

    return_
    = get_file().pwritev
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
