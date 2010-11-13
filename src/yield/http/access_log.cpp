// yield/http/access_log.cpp

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


#include "file_access_log.hpp"
#include "ostream_access_log.hpp"
#include "yield/assert.hpp"
#include "yield/http/access_log.hpp"
#include "yield/http/http_request.hpp"
#include "yield/http/http_response.hpp"
#include "yield/net/sockets/socket_address.hpp"

#include <sstream> // for std::ostringstream
#ifdef _WIN32
  #include <Windows.h>
#endif


namespace yield
{
  namespace http
  {
    using yield::net::sockets::SocketAddress;


    AccessLog::AccessLog( Format& format )
      : format( format )
    { }

    AccessLog::~AccessLog()
    {
      delete &format;
    }


    string
    AccessLog::CombinedFormat::operator()
    (
      const HTTPRequest& http_request,
      const HTTPResponse& http_response
    ) const
    {
      string common_entry
        = CommonFormat::operator()( http_request, http_response );

      return common_entry;
    }


    string
    AccessLog::CommonFormat::operator()
    (
      const HTTPRequest& http_request,
      const HTTPResponse& http_response
    ) const
    {
      string client_ip;
      //if ( !SocketAddress::LOOPBACK.getnameinfo( client_ip, true ) )
        client_ip = "-";

      char request_creation_time[32];
      {
        static const char* Months[]
          = {
              "Jan", "Feb", "Mar", "Apr", "May", "Jun",
              "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
            };

    #ifdef _WIN32
        SYSTEMTIME local_system_time = http_request.get_creation_date_time();
        TIME_ZONE_INFORMATION time_zone_information;
        GetTimeZoneInformation( &time_zone_information );

        // 10/Oct/2000:13:55:36 -0700
        _snprintf_s
        (
          request_creation_time,
          32,
          _TRUNCATE,
          "[%02d/%s/%04d:%02d:%02d:%02d %+0.4d]",
          local_system_time.wDay,
          Months[local_system_time.wMonth-1],
          local_system_time.wYear,
          local_system_time.wHour,
          local_system_time.wMinute,
          local_system_time.wSecond,
          ( time_zone_information.Bias / 60 ) * -100
        );
    #else
        tm local_tm = DateTime::now();

        snprintf
        (
          request_creation_time,
          32,
          "[%02d/%s/%04d:%02d:%02d:%02d %d]",
          local_tm.tm_mday,
          Months[local_tm.tm_mon],
          local_tm.tm_year + 1900,
          local_tm.tm_hour,
          local_tm.tm_min,
          local_tm.tm_sec,
          0
        );
    #endif
      }

      const char* request_http_version;
      if ( http_request.get_http_version() == 1.0 )
        request_http_version = "1.0";
      else if ( http_request.get_http_version() == 1.1 )
        request_http_version = "1.1";
      else
        DebugBreak();

      std::ostringstream entry;
      entry << client_ip << " ";
      entry << "- "; // RFC 1413 identity
      entry << "- "; // HTTP auth userid
      entry << request_creation_time << " ";
      entry << "\"";
      entry << http_request.get_method() << " ";
      entry << http_request.get_uri().get_path() << " ";
      entry << "HTTP/" << request_http_version;
      entry << "\" ";
      entry << http_response.get_status_code() << " ";
      if ( http_response.get_body() != NULL )
        entry << http_response.get_content_length();
      else
        entry << "-";
      entry << "\r\n";

      return entry.str();
    }
  }
}
