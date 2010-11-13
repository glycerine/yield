// yield/thread/unix/fiber.hpp

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


#ifndef _YIELD_THREAD_UNIX_FIBER_HPP_
#define _YIELD_THREAD_UNIX_FIBER_HPP_


#include "yield/thread/fiber.hpp"

#ifdef YIELD_HAVE_UNIX_PTH
  #include <pth.h>
#endif


namespace yield
{
  namespace thread
  {
    namespace unix
    {
      class Fiber : public yield::thread::Thread
      {
      public:
        static Fiber* create( Runnable& );
        #ifdef YIELD_HAVE_UNIX_PTH
          operator pth_t() const { return handle; }
        #endif
        static auto_Object<yield::thread::Fiber> self();

        // yiled::process::Thread
        void* getspecific( uintptr_t key );
        uintptr_t key_create();
        bool key_delete( uintptr_t key );
        bool setspecific( uintptr_t key, void* value );
        void yield();
        void yield( Fiber& to_fiber );

      private:
        Fiber( Runnable& runnable );
        #ifdef YIELD_HAVE_UNIX_PTH
          Fiber( pth_t pth );
        #endif
        static void* run( void* );

      private:
        #ifdef YIELD_HAVE_UNIX_PTH
          pth_t pth;
        #endif
      };
    }
  }
}


#endif
