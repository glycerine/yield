// yield/process/option_parser.hpp

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


#ifndef _YIELD_PROCESS_OPTION_PARSER_HPP_
#define _YIELD_PROCESS_OPTION_PARSER_HPP_


#include "yield/object.hpp"


namespace yield {
namespace process {
// Modelled after Python's optparse.OptionParser class
class OptionParser : public Object {
public:
  class Option {
  public:
    Option
    (
      const string& option,
      const string& help,
      bool require_argument = true
    );

    Option(const string& option, bool require_argument = true);

    const string& get_help() const {
      return help;
    }
    bool get_require_argument() const {
      return require_argument;
    }

    operator const char* () const {
      return option.c_str();
    }
    operator const string& () const {
      return option;
    }
    bool operator==(const string& option) const;
    bool operator==(const char* option) const;
    bool operator==(const Option& other) const;
    bool operator<(const Option& other) const;   // For sorting

  private:
    string help, option;
    bool require_argument;
  };

  class Options : public vector<Option> {
  public:
    void add
    (
      const string& option,
      const string& help,
      bool require_argument = true
    );

    void add(const string& option, bool require_argument = true);
    void add(const Option& option);
    void add(const Options& options);
  };

  class ParsedOption : public Option {
  public:
    ParsedOption(Option& option);
    ParsedOption(Option& option, const string& argument);

    const string& get_argument() const {
      return argument;
    }

  private:
    string argument;
  };

  typedef vector<ParsedOption> ParsedOptions;

public:
  void
  add_option
  (
    const string& option,
    const string& help,
    bool require_argument = true
  );

  void add_option(const string& option, bool require_argument = true);
  void add_option(const Option& option);
  void add_options(const Options& options);

  void
  parse_args
  (
    int argc,
    char** argv,
    ParsedOptions& out_parsed_options,
    vector<string>& out_positional_arguments
  );

  string usage();

private:
  Options options;
};
}
}


#endif
