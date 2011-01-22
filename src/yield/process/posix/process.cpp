// yield/process/posix/process.cpp

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


#include "process.hpp"
#include "yield/assert.hpp"
#include "yield/channel.hpp"

#include <signal.h>
#include <sys/wait.h> // For waitpid


namespace yield {
namespace process {
namespace posix {
using yield::fs::Path;


Process::Process
(
  pid_t pid,
  Channel* stderr_,
  Channel* stdin_,
  Channel* stdout_
)
  : yield::process::Process(stderr_, stdin_, stdout_),
    pid(pid)
{ }

Process*
Process::create
(
  const Path& command_line,
  Channel* stderr_,
  Channel* stdin_,
  Channel* stdout_
) {
  const char* argv[] = { static_cast<const char*>(NULL) };
  return create(command_line, argv, stderr_, stdin_, stdout_);
}

Process*
Process::create
(
  const Path& executable_file_path,
  const char** null_terminated_argv,
  Channel* stderr_,
  Channel* stdin_,
  Channel* stdout_
) {
  pid_t pid = fork();
  if (pid == -1)
    return NULL;
  else if (pid == 0) {   // Child
    //close( STDIN_FILENO );
    // Set stdin to read end of stdin pipe
    //dup2( *child_stdin->get_input_stream()->get_file(), STDIN_FILENO );

    //close( STDOUT_FILENO );
    // Set stdout to write end of stdout pipe
    //dup2( *child_stdout->get_output_stream()->get_file(), STDOUT_FILENO );

    //close( STDERR_FILENO );
    // Set stderr to write end of stderr pipe
    //dup2( *child_stderr->get_output_stream()->get_file(), STDERR_FILENO );

    vector<char*> argv_with_executable_file_path;
    argv_with_executable_file_path.push_back
    (
      const_cast<char*>(executable_file_path.c_str())
    );
    size_t arg_i = 0;
    while (null_terminated_argv[arg_i] != NULL) {
      argv_with_executable_file_path.push_back
      (
        const_cast<char*>(null_terminated_argv[arg_i])
      );
      arg_i++;
    }
    argv_with_executable_file_path.push_back(NULL);

    execv(executable_file_path.c_str(), &argv_with_executable_file_path[0]);

    DebugBreak();
    return NULL;
  } else // Parent
    return new Process(pid, stderr_, stdin_, stdout_);
}

pid_t Process::self() {
  return ::getpid();
}

bool Process::kill() {
  return ::kill(pid, SIGKILL) == 0;
}

YO_NEW_REF Process* Process::open(pid_t pid) {
  return new Process(pid);
}

bool Process::poll(int* out_return_code) {
  if (waitpid(pid, out_return_code, WNOHANG) > 0) {
    // "waitpid() was successful. The value returned indicates the process ID
    // of the child process whose status information was recorded in the
    // storage pointed to by stat_loc."
#if defined(__FreeBSD__) || defined(__sun)
    if (WIFEXITED(*out_return_code)) {     // Child exited normally
      *out_return_code = WEXITSTATUS(*out_return_code);
#else
    if (WIFEXITED(out_return_code)) {     // Child exited normally
      *out_return_code = WEXITSTATUS(out_return_code);
#endif
      return true;
    } else
      return false;
  }
  // 0 = WNOHANG was specified on the options parameter, but no child process
  // was immediately available.
  // -1 = waitpid() was not successful. The errno value is set
  // to indicate the error.
  else
    return false;
}

bool Process::terminate() {
  return ::kill(pid, SIGTERM) == 0;
}

int Process::wait() {
  int stat_loc;
  if (waitpid(pid, &stat_loc, 0) >= 0)
    return stat_loc;
  else
    return -1;
}
}
}
}
