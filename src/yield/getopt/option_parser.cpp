// yield/getopt/option_parser.cpp

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

#include "SimpleOpt.h"
#include "yield/assert.hpp"
#include "yield/exception.hpp"
#include "yield/getopt/option_parser.hpp"

#include <algorithm>
#include <sstream>

namespace yield {
namespace getopt {
using std::endl;
using std::sort;
using std::ostringstream;


void
OptionParser::add_option(
  const string& option,
  const string& help,
  bool require_argument
) {
  options.add(option, help, require_argument);
}

void OptionParser::add_option(const string& option, bool require_argument) {
  options.add(option, string(), require_argument);
}

void OptionParser::add_option(const Option& option) {
  options.add(option);
}

void OptionParser::add_options(const Options& options) {
  this->options.add(options);
}

void
OptionParser::parse_args(
  int argc,
  char** argv,
  vector<ParsedOption>& out_parsed_options,
  vector<string>& out_positional_arguments
) {
  vector<CSimpleOpt::SOption> simpleopt_options;

  for
  (
    vector<Option>::size_type option_i = 0;
    option_i < options.size();
    option_i++
  ) {
    CSimpleOpt::SOption simpleopt_option
    = {
      option_i,
      options[option_i],
      options[option_i].get_require_argument() ? SO_REQ_SEP : SO_NONE
    };

    simpleopt_options.push_back(simpleopt_option);
  }

  CSimpleOpt::SOption sentinel_simpleopt_option = SO_END_OF_OPTIONS;
  simpleopt_options.push_back(sentinel_simpleopt_option);

  // Make copies of the strings in argv so that
  // SimpleOpt can punch holes in them
  vector<char*> argvv(argc);
  for (int arg_i = 0; arg_i < argc; arg_i++) {
    size_t arg_len = strnlen(argv[arg_i], SIZE_MAX) + 1;
    argvv[arg_i] = new char[arg_len];
    memcpy_s(argvv[arg_i], arg_len, argv[arg_i], arg_len);
  }

  CSimpleOpt args(argc, &argvv[0], &simpleopt_options[0]);

  while (args.Next()) {
    switch (args.LastError()) {
    case SO_SUCCESS: {
      for
      (
        vector<Option>::iterator option_i = options.begin();
        option_i != options.end();
        ++option_i
      ) {
        Option& option = *option_i;

        if (option == args.OptionText()) {
          if (option.get_require_argument()) {
            out_parsed_options.push_back
            (
              ParsedOption(option, args.OptionArg())
            );
          } else
            out_parsed_options.push_back(ParsedOption(option));
        }
      }
    }
    break;

    case SO_OPT_INVALID: {
      string error_message("unregistered option: ");
      error_message.append(args.OptionText());
      throw Exception(error_message);
    }
    break;

    case SO_OPT_MULTIPLE: {
      string error_message("duplicate option: ");
      error_message.append(args.OptionText());
      throw Exception(error_message);
    }
    break;

    case SO_ARG_INVALID: {
      string error_message("unexpected value to option ");
      error_message.append(args.OptionText());
      throw Exception(error_message);
    }
    break;

    case SO_ARG_MISSING: {
      string error_message("missing value to option ");
      error_message.append(args.OptionText());
      throw Exception(error_message);
    }
    break;

    case SO_ARG_INVALID_DATA: { // Argument looks like another option
      ostringstream error_message;
      error_message << args.OptionText() <<
                    "requires a value, but you appear to have passed another option.";
      throw Exception(error_message.str());
    }
    break;

    default:
      DebugBreak();
      break;
    }
  }

  for (int arg_i = argc - args.FileCount(); arg_i < argc; arg_i++)
    out_positional_arguments.push_back(argv[arg_i]);

  for
  (
    vector<char*>::iterator arg_i = argvv.begin();
    arg_i != argvv.end();
    arg_i++
  )
    delete [] *arg_i;
  argvv.clear();
}

string OptionParser::usage() {
  ostringstream usage;

  usage << "Options:" << endl;

  sort(options.begin(), options.end());
  for
  (
    vector<Option>::const_iterator option_i = options.begin();
    option_i != options.end();
    option_i++
  ) {
    const Option& option = *option_i;
    usage << "  " << option;
    if (!option.get_help().empty())
      usage << "\t" << option.get_help();
    usage << endl;
  }

  usage << endl;

  return usage.str();
}
}
}
