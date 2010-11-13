// yield/thread/thread.hpp

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


#ifndef _YIELD_THREAD_THREAD_HPP_
#define _YIELD_THREAD_THREAD_HPP_


#include "yield/auto_object.hpp"
#include "yield/object.hpp"
#include "yield/thread/runnable.hpp"


namespace yield
{
  class Time;


  namespace thread
  {
    class ProcessorSet;
    class Runnable;

    #if defined(__linux__)
      namespace linux { class Thread; }
    #elif defined(__MACH__)
      namespace darwin { class Thread; }
    #elif defined(__sun)
      namespace sunos { class Thread; }
    #elif defined(_WIN32)
      namespace win32 { class Thread; }
    #else
      namespace posix { class Thread; }
    #endif


    class Thread : public Object
    {
    public:
      Thread( void ( *run )( void* ), void* context = NULL );
      Thread( Runnable& );
      ~Thread();

      bool cancel();
      Runnable* get_runnable() const;
      void* getspecific( uintptr_t key );
      bool is_running() const;
      bool join();
      uintptr_t key_create();
      bool key_delete( uintptr_t key );
      void nanosleep( const Time& );
      static auto_Object<Thread> self();
      void set_name( const char* name );
      bool setaffinity( uint16_t logical_processor_i );
      bool setaffinity( const ProcessorSet& logical_processor_set );
      bool setspecific( uintptr_t key, intptr_t value );
      bool setspecific( uintptr_t key, uintptr_t value );
      bool setspecific( uintptr_t key, void* value );
      void yield();

    private:
      #if defined(__linux__)
        Thread( linux::Thread* pimpl )
      #elif defined(__MACH__)
        Thread( darwin::Thread* pimpl )
      #elif defined(__sun)
        Thread( sunos::Thread* pimpl )
      #elif defined(_WIN32)
        Thread( win32::Thread* pimpl )
      #else
        Thread( posix::Thread* pimpl )
      #endif
          : pimpl( pimpl )
        { }

      void init( Runnable& );

    private:
      #if defined(__linux__)
        linux::Thread* pimpl;
      #elif defined(__MACH__)
        darwin::Thread* pimpl;
      #elif defined(__sun)
        sunos::Thread* pimpl;
      #elif defined(_WIN32)
        win32::Thread* pimpl;
      #else
        posix::Thread* pimpl;
      #endif
    };
  }
}


#endif
