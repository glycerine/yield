// yield/aio/net/sockets/recv_aiocb.hpp

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


#ifndef _YIELD_AIO_NET_SOCKETS_RECV_AIOCB_HPP_
#define _YIELD_AIO_NET_SOCKETS_RECV_AIOCB_HPP_


#include "yield/aio/net/sockets/aiocb.hpp"
#include "yield/net/sockets/socket.hpp"
#include "yield/net/sockets/socket_address.hpp"


namespace yield
{
  namespace aio
  {
    namespace net
    {
      namespace sockets
      {
        class recvAIOCB : public AIOCB
        {
        public:
          const static uint32_t TYPE_ID = 3045195539UL;

        public:
          recvAIOCB
          (
            yield::net::sockets::Socket& socket_,
            Buffer& buffer,
            const yield::net::sockets::Socket::MessageFlags& flags
          );

          ~recvAIOCB();

          Buffer& get_buffer() const { return buffer; }
          const yield::net::sockets::Socket::MessageFlags& get_flags() const;
          const yield::net::sockets::SocketAddress& get_peername() const;
          yield::net::sockets::SocketAddress& get_peername() { return peername; }

          // yield::Object
          uint32_t get_type_id() const { return TYPE_ID; }
          const char* get_type_name() const { return "yield::aio::net::sockets::recvAIOCB"; }

          // yield::aio::AIOCB
          #ifdef _WIN32
            bool issue( EventHandler& completion_handler );
            bool issue( yield::aio::win32::AIOQueue& );
          #endif
          RetryStatus retry();
          void set_return( ssize_t return_ );

        private:
          recvAIOCB( recvAIOCB& other );

        private:
          Buffer& buffer;
          yield::net::sockets::Socket::MessageFlags flags;
          yield::net::sockets::SocketAddress peername;
        };
      }
    }
  }
}


#endif
