// yield/fs/extended_attributes.cpp

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
#include "yield/exception.hpp"
#include "yield/log.hpp"
#include "yield/fs/extended_attributes.hpp"
#include "yield/fs/path.hpp"


namespace yield {
namespace fs {
bool ExtendedAttributes::get(const char* name, string& out_value) {
  ssize_t estimated_value_len = get(name, NULL, 0);
  if (estimated_value_len != -1) {
    char* value = new char[estimated_value_len];
    ssize_t value_len = get(name, value, estimated_value_len);
    debug_assert_eq(value_len, estimated_value_len);
    out_value.assign(value, value_len);
    delete [] value;
    return true;
  } else
    return false;
}

bool ExtendedAttributes::set(const char* name, const char* value, int flags) {
  return set(name, value, strlen(value), flags);
}

bool
ExtendedAttributes::set
(
  const char* name,
  const string& value,
  int flags
) {
  return set(name, value.c_str(), value.size(), flags);
}

//ssize_t
//ExtendedAttributes::trace_get
//(
//  Log& log,
//  const Path& path,
//  const char* name,
//  void* value,
//  size_t size,
//  ssize_t ret
//)
//{
//  uint32_t last_error_code = Exception::get_last_error_code();

//  if ( ret != -1 )
//  {
//    if ( value == NULL )
//    {
//      Log::Stream log_stream = log.get_stream( Log::INFO );
//      log_stream <<
//        "ExtendedAttributes::get( " << path << ", " << name << " ) -> " <<
//        static_cast<size_t>( ret ) << ".";
//    }
//    else
//    {
//      log.get_stream( Log::INFO ) <<
//        "ExtendedAttributes::get( " << path << ", " << name << " ) -> ";
//      log.write( value, size, Log::INFO );
//    }
//  }
//  else
//  {
//    log.get_stream( Log::INFO ) <<
//      "ExtendedAttributes::get( " << path << ", " << name << " ) -> failed: ";
//       Exception( last_error_code );
//  }

//  Exception::set_last_error_code( last_error_code );

//  return ret;
//}

//bool
//ExtendedAttributes::trace_list
//(
//  Log& log,
//  const Path& path,
//  vector<string>& out_names,
//  bool ret
//)
//{
//  uint32_t last_error_code = Exception::get_last_error_code();

//  {
//    Log::Stream log_stream = log.get_stream( Log::INFO );
//    log_stream << "ExtendedAttributes::list( " << path << " ) -> ";
//    if ( ret )
//    {
//      log_stream << "[ ";
//      for
//      (
//        vector<string>::const_iterator name_i = out_names.begin();
//        name_i != out_names.end();
//        ++name_i
//      )
//        log_stream << *name_i << ' ';
//      log_stream << "].";
//    }
//    else
//      log_stream << "failed: " << Exception( last_error_code ) << ".";
//  }

//  Exception::set_last_error_code( last_error_code );

//  return ret;
//}

//bool
//ExtendedAttributes::trace_remove
//(
//  Log& log,
//  const Path& path,
//  const char* name,
//  bool ret
//)
//{
//  uint32_t last_error_code = Exception::get_last_error_code();

//  {
//    Log::Stream log_stream = log.get_stream( Log::INFO );
//    log_stream << "ExtendedAttributes::remove( " <<
//      path << ", " << name << " ) -> ";
//    if ( ret ) log_stream << "succeeded.";
//    else log_stream << "failed: " << Exception( last_error_code ) << ".";
//  }

//  Exception::set_last_error_code( last_error_code );

//  return ret;
//}

//bool
//ExtendedAttributes::trace_set
//(
//  Log& log,
//  const Path& path,
//  const char* name,
//  const void* value,
//  size_t size,
//  int flags,
//  bool ret
//)
//{
//  uint32_t last_error_code = Exception::get_last_error_code();

//  {
//    Log::Stream log_stream = log.get_stream( Log::INFO );
//    log_stream << "ExtendedAttributes::set( " << path << ", " << name
//      << ", value, " << ", " << size << ", " << flags << " ) -> ";
//    if ( ret ) log_stream << "succeeded.";
//    else log_stream << "failed: " << Exception( last_error_code ) << ".";
//  }

//  Exception::set_last_error_code( last_error_code );

//  return ret;
//}
}
}
