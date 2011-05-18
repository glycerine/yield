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

#include "yield/event.hpp"

#include <exception>

namespace yield {
/**
  Generic exceptions, also used in the event-driven concurrency subsystem.
*/
class Exception : public Event, public std::exception {    
public:
  /**
    Run-time type ID.
  */
  const static uint32_t TYPE_ID = 3282856065UL;

public:
  /**
    Construct an Exception using get_last_error_code and
      the associated system error message.
  */
  Exception();

  /**
    Construct an Exception using error_code and
      the associated system error message.
    @param error_code the system error code
  */
  Exception(uint32_t error_code);

  /**
    Construct an Exception using a custom error message.
    Copies error_message.
  */
  Exception(const char* error_message);

  /**
    Construct an Exception using a custom error message.
    Copies error_message.
  */
  Exception(const string& error_message);

  /**
    Construct an Exception using a custom error code and message.
    Copies error_message.
  */
  Exception(uint32_t error_code, const char* error_message);

  /**
    Construct an Exception using a custom error code and message.
    Copies error_message.
  */
  Exception(uint32_t error_code, const string& error_message);

  /**
    Copy constructor.
  */
  Exception(const Exception& other);

  /**
    Virtual destructor that deallocates the error_message copy.
  */
  virtual ~Exception() throw();

public:
  /**
    Clone this Exception on the heap.
    @return a new'd copy of this Exception
  */
  virtual YO_NEW_REF Exception& clone() const {
    return *new Exception(*this);
  }

public:
  /**
    Get the error code associated with this Exception.
    @return the error code
  */
  virtual uint32_t get_error_code() const {
    return error_code;
  }

  /**
    Get the error message associated with this Exception.
    @return the error message
  */
  virtual const char* get_error_message() const throw();

  /**
    Get the last system error code (errno or GetLastError).
    @return the last system error code
  */
  static uint32_t get_last_error_code();

  /**
    Cast this Exception to a C string.
    @return the error message
  */
  operator const char* () const throw() {
    return get_error_message();
  }

public:
  /**
    Rethrow this Exception on the stack.
  */
  virtual void rethrow() const {
    throw Exception(*this);
  }

public:
  /**
    Set the error code associated with this Exception.
    May change error_message accordingly.
    @param error_code the new error code
  */
  void set_error_code(uint32_t error_code);

  /**
    Set the error message associated with this Exception.
    Copies error_message.
    @param error_message the new error message
  */
  void set_error_message(const char* error_message);

  /**
    Set the error message associated with this Exception.
    Copies error_message.
    @param error_message the new error message
  */
  void set_error_message(const string& error_message);

  /**
    Set the last system error code (errno or SetLastError)
    @param error_code the new system error code
  */
  static void set_last_error_code(uint32_t error_code);

public:
  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }

  const char* get_type_name() const {
    return "yield::Exception";
  }

public:
  // yield::Response
  bool is_exception() const {
    return true;
  }

public:
  // std::exception
  const char* what() const throw();

private:
  uint32_t error_code;
  char* error_message;
};
}

#endif
