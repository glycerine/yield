// yield/process/process.hpp

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


#ifndef _YIELD_PROCESS_PROCESS_HPP_
#define _YIELD_PROCESS_PROCESS_HPP_


#include "yield/object.hpp"
#include "yield/fs/path.hpp"


namespace yield {
class Channel;


namespace process {
class Process : public Object {
public:
  virtual ~Process();

  static YO_NEW_REF Process*
  create
  (
    const yield::fs::Path& command_line,
    YO_NEW_REF Channel* stderr_ = NULL,
    YO_NEW_REF Channel* stdin_ = NULL,
    YO_NEW_REF Channel* stdout_ = NULL
  );

  static YO_NEW_REF Process*
  create
  (
    int argc,
    yield::i18n::tstring::value_type** argv,
    YO_NEW_REF Channel* stderr_ = NULL,
    YO_NEW_REF Channel* stdin_ = NULL,
    YO_NEW_REF Channel* stdout_ = NULL
  );

  static YO_NEW_REF Process*
  create
  (
    const vector<yield::i18n::tstring::value_type*>& argv,
    YO_NEW_REF Channel* stderr_ = NULL,
    YO_NEW_REF Channel* stdin_ = NULL,
    YO_NEW_REF Channel* stdout_ = NULL
  );

  static YO_NEW_REF Process*
  create
  (
    const yield::fs::Path& executable_file_path,
    const yield::i18n::tstring::value_type** null_terminated_argv,
    YO_NEW_REF Channel* stderr_ = NULL,
    YO_NEW_REF Channel* stdin_ = NULL,
    YO_NEW_REF Channel* stdout_ = NULL
  );

  static yield::fs::Path get_current_executable_file_path();
  static pid_t getpid() {
    return self();
  }
  Channel* get_stderr() const {
    return stderr_;
  }
  Channel* get_stdin() const {
    return stdin_;
  }
  Channel* get_stdout() const {
    return stdout_;
  }
  virtual bool kill() = 0; // SIGKILL
  static YO_NEW_REF Process* open(pid_t pid);
  virtual bool poll(int* out_return_code = 0) = 0;   // waitpid() with WNOHANG
  static pid_t self();
  virtual bool terminate() = 0; // SIGTERM
  virtual int wait() = 0; // waitpid(), blocking

protected:
  Process
  (
    YO_NEW_REF Channel* stderr_,
    YO_NEW_REF Channel* stdin_,
    YO_NEW_REF Channel* stdout_
  );

private:
  Channel* stderr_, *stdin_, *stdout_;
};
}
}


#endif
