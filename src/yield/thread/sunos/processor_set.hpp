// yield/thread/sunos/processor_set.hpp

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


#ifndef _YIELD_THREAD_SUNOS_PROCESSOR_SET_HPP_
#define _YIELD_THREAD_SUNOS_PROCESSOR_SET_HPP_


#include "yield/thread/processor_set.hpp"


namespace yield
{
  namespace thread
  {
    class ProcessorSet : public Object
    {
    public:
      ProcessorSet();
      ~ProcessorSet();

      void clear();
      void clear( uint16_t processor_i );

      uint16_t count() const
      {
        uint16_t count = 0;

        for
        (
          uint16_t processor_i = 0;
          processor_i < static_cast<uint16_t>( -1 );
          processor_i++
        )
        {
          if ( isset( processor_i ) )
            count++;
        }

        return count;
      }

      bool empty() const { return count() == 0; }
      bool isset( uint16_t processor_i ) const;
      bool set( uint16_t processor_i );

    private:
      ProcessorSet( const ProcessorSet& ) { } // Prevent copying

    private:
      friend class Process;

      #if defined(_WIN32)
        friend class win32::Thread;
        uintptr_t mask;
      #elif defined(__linux__)
        friend class posix::Thread;
        cpu_set_t cpu_set;
      #elif defined(__sun)
        friend class posix::Thread;
        int psetid;
      #endif
    };
  }
}


#endif
