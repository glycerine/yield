// yield/process/win32/process.cpp

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

#include "process.hpp"
#include "yield/channel.hpp"

#include <Windows.h>


namespace yield {
namespace process {
namespace win32 {
using yield::fs::Path;
using yield::i18n::tstring;


Process::Process
(
  HANDLE hProcess,
  HANDLE hThread,
  Channel* stderr_,
  Channel* stdin_,
  Channel* stdout_
)
  : yield::process::Process(stderr_, stdin_, stdout_),
    hProcess(hProcess), hThread(hThread)
{ }

Process::~Process() {
  CloseHandle(hProcess);
  if (hThread != INVALID_HANDLE_VALUE)
    CloseHandle(hThread);
}

Process*
Process::create
(
  const Path& command_line,
  Channel* stderr_,
  Channel* stdin_,
  Channel* stdout_
) {
  STARTUPINFO startup_info;
  ZeroMemory(&startup_info, sizeof(STARTUPINFO));
  startup_info.cb = sizeof(STARTUPINFO);

  if (stderr_ != NULL) {
    startup_info.hStdError = *stderr_;;
    startup_info.dwFlags = STARTF_USESTDHANDLES;
  }

  if (stdin_ != NULL) {
    startup_info.hStdInput = *stdin_;
    startup_info.dwFlags = STARTF_USESTDHANDLES;
  }

  if (stdout_ != NULL) {
    startup_info.hStdOutput = *stdout_;
    startup_info.dwFlags = STARTF_USESTDHANDLES;
  }

  PROCESS_INFORMATION proc_info;
  ZeroMemory(&proc_info, sizeof(PROCESS_INFORMATION));

  if
  (
    CreateProcess
    (
      NULL,
      const_cast<wchar_t*>(command_line.c_str()),
      NULL,
      NULL,
      TRUE,
      CREATE_NO_WINDOW,
      NULL,
      NULL,
      &startup_info,
      &proc_info
    )
  ) {
    return new Process
           (
             proc_info.hProcess,
             proc_info.hThread,
             stderr_,
             stdin_,
             stdout_
           );
  } else {
    Channel::dec_ref(stderr_);
    Channel::dec_ref(stdin_);
    Channel::dec_ref(stdout_);
    return NULL;
  }
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
  tstring command_line;
  if (executable_file_path.find(L' ') == -1)
    command_line.append(executable_file_path);
  else {
    command_line.append(L"\"", 1);
    command_line.append(executable_file_path);
    command_line.append(L"\"", 1);
  }

  size_t arg_i = 0;
  while (null_terminated_argv[arg_i] != NULL) {
    command_line.append(L" ", 1);
    command_line.append(null_terminated_argv[arg_i]);
    arg_i++;
  }

  return create(command_line, stderr_, stdin_, stdout_);
}

bool Process::kill() {
  return TerminateProcess(hProcess, 0) == TRUE;
}

Path Process::get_current_executable_file_path() {
  wchar_t module_file_name[MAX_PATH];
  DWORD module_file_name_len
  = GetModuleFileName(NULL, module_file_name, MAX_PATH);
  if (module_file_name_len > 0)
    return Path(module_file_name, module_file_name_len);
  else
    return Path();
}

Process* Process::open(pid_t pid) {
  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
  if (hProcess != NULL)
    return new Process(hProcess, INVALID_HANDLE_VALUE);
  else
    return NULL;
}

bool Process::poll(int* out_return_code) {
  if (WaitForSingleObject(hProcess, 0) != WAIT_TIMEOUT) {
    if (out_return_code) {
      DWORD dwChildExitCode;
      GetExitCodeProcess(hProcess, &dwChildExitCode);
      *out_return_code = (int)dwChildExitCode;
    }

    return true;
  } else
    return false;
}

pid_t Process::self() {
  return GetCurrentProcessId();
}

bool Process::terminate() {
  return TerminateProcess(hProcess, 0) == TRUE;
}

int Process::wait() {
  WaitForSingleObject(hProcess, INFINITE);
  DWORD dwChildExitCode;
  GetExitCodeProcess(hProcess, &dwChildExitCode);
  return (int)dwChildExitCode;
}
}
}
}
