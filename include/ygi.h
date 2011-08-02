/* ygi.h */

/* Copyright (c) 2011 Minor Gordon */
/* All rights reserved */

/* This source file is part of the Yield project. */

/* Redistribution and use in source and binary forms, with or without */
/* modification, are permitted provided that the following conditions are met: */
/* * Redistributions of source code must retain the above copyright */
/* notice, this list of conditions and the following disclaimer. */
/* * Redistributions in binary form must reproduce the above copyright */
/* notice, this list of conditions and the following disclaimer in the */
/* documentation and/or other materials provided with the distribution. */
/* * Neither the name of the Yield project nor the */
/* names of its contributors may be used to endorse or promote products */
/* derived from this software without specific prior written permission. */

/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" */
/* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE */
/* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE */
/* ARE DISCLAIMED. IN NO EVENT SHALL Minor Gordon BE LIABLE FOR ANY DIRECT, */
/* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES */
/* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; */
/* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND */
/* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT */
/* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF */
/* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#ifndef _YGI_H_
#define _YGI_H_

#define __STDC_LIMIT_MACROS 1
#include <stdint.h>

#ifdef _WIN32
#ifndef _STRUCT_IOVEC_DEFINED
struct iovec {
  size_t iov_len;
  void* iov_base;
};
#define _STRUCT_IOVEC_DEFINED 1
#endif
#else
#include <sys/types.h>
#include <sys/uio.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct ygi_response_t {
    void
    (*set_body)(
      struct ygi_response_t* this_,
      const struct iovec* body
    );

    void
    (*set_chunked_body_callback)(
      struct ygi_response_t* this_,
      struct iovec(*chunked_body_callback)()
    );

    void
    (*set_field)(
      struct ygi_response_t* this_,
      const char* field_name,
      const struct iovec* field_value
    );
  } ygi_response_t;

  typedef struct ygi_request_t {
    size_t (*CONTENT_LENGTH)(struct ygi_request_t* this_);
    struct iovec(*DOCUMENT_ROOT)(struct ygi_request_t* this_);
    struct iovec(*HTTP_)(struct ygi_request_t* this_, const char* field_name);
    struct iovec(*HTTP_REFERER)(struct ygi_request_t* this_);
    struct iovec(*HTTP_USER_AGENT)(struct ygi_request_t* this_);
    struct iovec(*PATH_INFO)(struct ygi_request_t* this_);
    struct iovec(*PATH_TRANSLATED)(struct ygi_request_t* this_);
    struct iovec(*QUERY_STRING)(struct ygi_request_t* this_);
    struct iovec(*REMOTE_ADDR)(struct ygi_request_t* this_);
    struct iovec(*REMOTE_HOST)(struct ygi_request_t* this_);
    struct iovec(*REQUEST_METHOD)(struct ygi_request_t* this_);
    struct iovec(*SCRIPT_NAME)(struct ygi_request_t* this_);
    struct iovec(*SERVER_NAME)(struct ygi_request_t* this_);
    uint16_t (*SERVER_PORT)(struct ygi_request_t* this_);

    ygi_response_t*
    (*respond)(
      struct ygi_request_t* this_,
      uint16_t status_code
    );
  } ygi_request_t;

  typedef ygi_response_t* (*ygi_request_handler_t)(ygi_request_t* ygi_request);

#ifdef __cplusplus
}
#endif

#endif
