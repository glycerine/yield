// yield/fs/file.cpp

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
#include "yield/fs/file.hpp"


namespace yield {
namespace fs {
size_t File::getpagesize() {
  return Page::getpagesize();
}

//bool
//File::trace
//(
//  Log& log,
//  const char* method,
//  const Path& path,
//  bool ret
//)
//{
//  return FileSystem::trace( log, method, path, ret );
//}

//Stat* File::trace_getattr( Log& log, const Path& path, Stat* ret )
//{
//  return FileSystem::trace_getattr( log, path, ret );
//}

//File::Lock*
//File::trace_getlk
//(
//  Log& log,
//  const Path& path,
//  const Lock& lock,
//  Lock* ret
//)
//{
//  uint32_t last_error_code = Exception::get_last_error_code();

//  {
//    Log::Stream log_stream = log.get_stream( Log::INFO );
//    log_stream << "File::getlk( " << path << ", " << lock << " ) -> ";
//    if ( ret != NULL ) log_stream << *ret << ".";
//    else log_stream << "failed: " << Exception( last_error_code ) << ".";
//  }

//  Exception::set_last_error_code( last_error_code );

//  return ret;
//}

//ssize_t
//File::trace_read
//(
//  Log& log,
//  const Path& path,
//  void* buf,
//  size_t buflen,
//  ssize_t ret
//)
//{
//  uint32_t last_error_code = Exception::get_last_error_code();

//  {
//    Log::Stream log_stream = log.get_stream( Log::INFO );
//    log_stream << "File::read( " << path << ", buf, " << buflen << " ) -> ";
//    if ( ret >= 0 ) log_stream << static_cast<size_t>( ret ) << ".";
//    else log_stream << "failed: " << Exception( last_error_code );
//  }

//  Exception::set_last_error_code( last_error_code );

//  return ret;
//}

//bool
//File::trace_setlk
//(
//  Log& log,
//  const Path& path,
//  const Lock& lock,
//  bool ret
//)
//{
//  uint32_t last_error_code = Exception::get_last_error_code();

//  {
//    Log::Stream log_stream = log.get_stream( Log::INFO );
//    log_stream << "File::setlk( " << path << ", " << lock << " ) -> ";
//    if ( ret ) log_stream << "succeeded.";
//    else log_stream << "failed: " << Exception( last_error_code ) << ".";
//  }

//  Exception::set_last_error_code( last_error_code );

//  return ret;
//}

//bool
//File::trace_setlkw
//(
//  Log& log,
//  const Path& path,
//  const Lock& lock,
//  bool ret
//)
//{
//  uint32_t last_error_code = Exception::get_last_error_code();

//  {
//    Log::Stream log_stream = log.get_stream( Log::INFO );
//    log_stream << "File::setlkw( " << path << ", " << lock << " ) -> ";
//    if ( ret ) log_stream << "succeeded.";
//    else log_stream << "failed: " << Exception( last_error_code ) << ".";
//  }

//  Exception::set_last_error_code( last_error_code );

//  return ret;
//}

//bool
//File::trace_truncate
//(
//  Log& log,
//  const Path& path,
//  uint64_t new_size,
//  bool ret
//)
//{
//  return FileSystem::trace_truncate( log, path, new_size, ret );
//}

//ssize_t
//File::trace_write
//(
//  Log& log,
//  const Path& path,
//  const void* buf,
//  size_t buflen,
//  ssize_t ret
//)
//{
//  uint32_t last_error_code = Exception::get_last_error_code();

//  {
//    Log::Stream log_stream = log.get_stream( Log::INFO );
//    log_stream << "File::write( " << path << ", buf, " << buflen << " ) -> ";
//    if ( ret >= 0 ) log_stream << static_cast<size_t>( ret ) << ".";
//    else log_stream << "failed: " << Exception( last_error_code );
//  }

//  Exception::set_last_error_code( last_error_code );

//  return ret;
//}

//ssize_t
//File::trace_writev
//(
//  Log& log,
//  const Path& path,
//  const iovec* iov,
//  int iovlen,
//  ssize_t ret
//)
//{
//  uint32_t last_error_code = Exception::get_last_error_code();

//  {
//    Log::Stream log_stream = log.get_stream( Log::INFO );
//    log_stream << "File::writev( " << path << ", iov, " << iovlen << " ) -> ";
//    if ( ret >= 0 ) log_stream << static_cast<size_t>( ret ) << ".";
//    else log_stream << "failed: " << Exception( last_error_code );
//  }

//  Exception::set_last_error_code( last_error_code );

//  return ret;
//}


//std::ostream& operator<<( std::ostream& os, const File::Lock& lock )
//{
//  os << "File::Lock{ ";

//  os << "start: " << lock.get_start() << ", ";
//  os << "len: " << lock.get_len() << ", ";
//  os << "pid: " << lock.get_pid() << ", ";

//  os << "type: " << ( lock.is_exclusive() ? "write" : "read" );

//  os << "whence: ";
//  switch ( lock.get_whence() )
//  {
//    case SEEK_SET: os << "SEEK_SET"; break;
//    case SEEK_CUR: os << "SEEK_CUR"; break;
//    case SEEK_END: os << "SEEK_END"; break;
//  }

//  os << " }";

//  return os;
//}
}
}
