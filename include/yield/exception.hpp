// yield/exception.hpp

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


#ifndef _YIELD_EXCEPTION_HPP_
#define _YIELD_EXCEPTION_HPP_


#include "yield/response.hpp"

#include <exception>


namespace yield {
class Exception : public Response, public std::exception {
public:
  const static uint32_t INVALID_ERROR_CODE = 0;
  const static uint32_t LAST_ERROR_CODE = static_cast<uint32_t>(-1);

public:
  // error_message is always copied
  Exception(uint32_t error_code = LAST_ERROR_CODE);
  Exception(const char* error_message);
  Exception(const string& error_message);
  Exception(uint32_t error_code, const char* error_message);
  Exception(uint32_t error_code, const string& error_message);
  Exception(const Exception& other);
  virtual ~Exception() throw();

  virtual Exception& clone() const {
    return *new Exception(*this);
  }

  virtual uint32_t get_error_code() const {
    return error_code;
  }
  virtual const char* get_error_message() const throw();
  static uint32_t get_last_error_code();
  operator const char* () const throw() {
    return get_error_message();
  }

  virtual void rethrow() const {
    throw Exception(*this);
  }

  void set_error_code(uint32_t error_code);
  void set_error_message(const char* error_message);
  void set_error_message(const string& error_message);
  static void set_last_error_code(uint32_t error_code);

  // Response
  bool is_exception() const {
    return true;
  }

  // std::exception
  const char* what() const throw();

private:
  uint32_t error_code;
  char* error_message;
};
}


#endif
