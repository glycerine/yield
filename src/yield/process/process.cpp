// yield/process/process.cpp

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


#if defined(__MACH__)
#include "darwin/process.hpp"
#elif defined(__linux__)
#include "linux/process.hpp"
#elif defined(_WIN32)
#include "win32/process.hpp"
#else
#include "posix/process.hpp"
#endif
#include "yield/channel.hpp"
#include "yield/exception.hpp"


namespace yield {
namespace process {
using yield::fs::Path;
using yield::i18n::tstring;


Process::Process( Channel* stderr_, Channel* stdin_, Channel* stdout_ )
  : stderr_( stderr_ ), stdin_( stdin_ ), stdout_( stdout_ )
{ }

Process::~Process() {
  Channel::dec_ref( stderr_ );
  Channel::dec_ref( stdin_ );
  Channel::dec_ref( stdout_ );
}

Process*
Process::create
(
  const Path& command_line,
  Channel* stderr_,
  Channel* stdin_,
  Channel* stdout_
) {
#ifdef _WIN32
  return win32::
#else
  return posix::
#endif
         Process::create
         (
           command_line,
           stderr_,
           stdin_,
           stdout_
         );
}

Process*
Process::create
(
  int argc,
  tstring::value_type** argv,
  Channel* stderr_,
  Channel* stdin_,
  Channel* stdout_
) {
  vector<tstring::value_type*> argv_copy;
  for ( int arg_i = 1; arg_i < argc; arg_i++ )
    argv_copy.push_back( argv[arg_i] );
  argv_copy.push_back( NULL );
  return create
         (
           Path( argv[0] ),
           const_cast<const tstring::value_type**>( &argv_copy[0] ),
           stderr_,
           stdin_,
           stdout_
         );
}

Process*
Process::create
(
  const vector<tstring::value_type*>& argv,
  Channel* stderr_,
  Channel* stdin_,
  Channel* stdout_
) {
  vector<tstring::value_type*> argv_copy( argv );
  argv_copy.push_back( NULL );
  return create
         (
           Path( argv[0] ),
           const_cast<const tstring::value_type**>( &argv_copy[0] ),
           stderr_,
           stdin_,
           stdout_
         );
}

Process*
Process::create
(
  const Path& executable_file_path,
  const tstring::value_type** null_terminated_argv,
  Channel* stderr_,
  Channel* stdin_,
  Channel* stdout_
) {
#ifdef _WIN32
  return win32::
#else
  return posix::
#endif
         Process::create
         (
           executable_file_path,
           null_terminated_argv
         );
}

Path Process::get_current_executable_file_path() {
#if defined(__MACH__)
  return darwin::Process::get_current_executable_file_path();
#elif defined(__linux__)
  return linux::Process::get_current_executable_file_path();
#elif defined(_WIN32)
  return win32::Process::get_current_executable_file_path();
#else
  return tstring();
#endif
}

Process* Process::open( pid_t pid ) {
#ifdef _WIN32
  return win32::Process::open( pid );
#else
  return posix::Process::open( pid );
#endif
}

pid_t Process::self() {
#ifdef _WIN32
  return win32::Process::self();
#else
  return posix::Process::self();
#endif
}
}
}
