// yield/thread/linux/processor_set.hpp

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

#ifndef _YIELD_THREAD_LINUX_PROCESSOR_SET_HPP_
#define _YIELD_THREAD_LINUX_PROCESSOR_SET_HPP_

#include "yield/types.hpp"

#include <sched.h> // For cpu_set_t

namespace yield {
namespace thread {
namespace linux {
/**
  Describe a set of processors as a contiguous array of bits.
  Used for setting thread or process affinity.
*/
class ProcessorSet {
public:
  ProcessorSet();

public:
  /**
    Clear all bits in the processor set.
  */
  void clear();

  /**
    Clear a specific bit in the processor set.
    @param processor_i index of the bit to clear
  */
  void clear(uint16_t processor_i);

public:
  /**
    Count the number of set bits.
    @return the number of set bits
  */
  uint16_t count() const;

  /**
    Check whether the processor set is empty.
    @return true if the processor set is empty
  */
  bool empty() const {
    return count() == 0;
  }

public:
  /**
    Query the system for the number of logical processors currently online.
    @return the number of logical processors currently online
  */
  static uint16_t get_online_logical_processor_count();

  /**
    Query the system for the number of logical processors currently online.
    @return the number of physical processors currently online
  */
  static uint16_t get_online_physical_processor_count();

public:
  /**
    Check if a specific bit is set.
    @param processor_i index of the bit to check
    @return true if the bit is set
  */
  bool isset(uint16_t processor_i) const;

public:
  /**
    Get the underlying Linux cpu_set_t.
    @return the underlying Linux cpu_set_t
  */
  operator const cpu_set_t* () const {
    return &cpu_set;
  }

public:
  /**
    Set a specific bit in the processor set.
    @param processor_i index of the bit to set
    @return true if the set succeeded
  */
  bool set(uint16_t processor_i);

private:
  cpu_set_t cpu_set;
};
}
}
}

#endif
