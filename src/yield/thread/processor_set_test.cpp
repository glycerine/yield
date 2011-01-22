// yield/thread/processor_set_test.cpp

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


#include "yield/assert.hpp"
#include "yield/auto_object.hpp"
#include "yield/thread/processor_set.hpp"
#include "yunit.hpp"


TEST_SUITE(ProcessorSet)

namespace yield {
namespace thread {
TEST(ProcessorSet, clear_all) {
  auto_Object<ProcessorSet> processor_set = new ProcessorSet;
  processor_set->set(0);
  processor_set->clear();
  throw_assert_eq(processor_set->count(), 0);
}

TEST(ProcessorSet, clear_one) {
  auto_Object<ProcessorSet> processor_set = new ProcessorSet;
  processor_set->set(0);
  processor_set->set(1);
  processor_set->clear(0);
  throw_assert_eq(processor_set->count(), 1);
}

TEST(ProcessorSet, count) {
  auto_Object<ProcessorSet> processor_set = new ProcessorSet;
  processor_set->set(0);
  throw_assert_eq(processor_set->count(), 1);
  processor_set->set(1);
  throw_assert_eq(processor_set->count(), 2);
  processor_set->clear();
  throw_assert_eq(processor_set->count(), 0);
}

TEST(ProcessorSet, empty) {
  auto_Object<ProcessorSet> processor_set = new ProcessorSet;
  throw_assert(processor_set->empty());
  processor_set->set(0);
  throw_assert_false(processor_set->empty());
}

TEST(ProcessorSet, isset) {
  auto_Object<ProcessorSet> processor_set = new ProcessorSet;
  processor_set->set(0);
  throw_assert(processor_set->isset(0));
  throw_assert_false(processor_set->isset(1));
}

TEST(ProcessorSet, set) {
  auto_Object<ProcessorSet> processor_set = new ProcessorSet;
  processor_set->set(0);
  throw_assert(processor_set->isset(0));
  throw_assert_false(processor_set->isset(1));
}
}
}
