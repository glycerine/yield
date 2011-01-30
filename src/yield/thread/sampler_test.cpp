// yield/thread/sampler_test.cpp

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
#include "yield/thread/sampler.hpp"
#include "yunit.hpp"


TEST_SUITE(Sampler)

namespace yield {
namespace thread {
TEST(Sampler, all_zeros) {
  Sampler<uint32_t, 10> sampler;
  for (unsigned char i = 0; i < 10; i++)
    sampler.set_next_sample(0);
  throw_assert_eq(sampler.get_samples_count(), 10);
  throw_assert_eq(sampler.get_min(), 0);
  throw_assert_eq(sampler.get_max(), 0);
  throw_assert_eq(sampler.get_mean(), 0);
  throw_assert_eq(sampler.get_median(), 0);
  throw_assert_eq(sampler.get_percentile(0.9), 0);
}

TEST(Sampler, averaged_median) {
  Sampler<uint32_t, 10> sampler;
  for (unsigned char i = 0; i < 10; i++)
    sampler.set_next_sample(i);
  throw_assert_eq(sampler.get_samples_count(), 10);
  throw_assert_eq(sampler.get_min(), 0);
  throw_assert_eq(sampler.get_max(), 9);
  throw_assert_eq(sampler.get_mean(), 4);
  throw_assert_eq(sampler.get_median(), 4);
  throw_assert_eq(sampler.get_percentile(0.9), 9);
}

TEST(Sampler, no_samples) {
  Sampler<uint32_t, 10> sampler;
  throw_assert_eq(sampler.get_samples_count(), 0);
  throw_assert_eq(sampler.get_max(), 0);
  throw_assert_eq(sampler.get_mean(), 0);
  throw_assert_eq(sampler.get_median(), 0);
  throw_assert_eq(sampler.get_percentile(0.9), 0);
}

TEST(Sampler, one_sample) {
  Sampler<uint32_t, 10> sampler;
  sampler.set_next_sample(42);
  throw_assert_eq(sampler.get_samples_count(), 1);
  throw_assert_eq(sampler.get_min(), 42);
  throw_assert_eq(sampler.get_max(), 42);
  throw_assert_eq(sampler.get_mean(), 42);
  throw_assert_eq(sampler.get_median(), 42);
  throw_assert_eq(sampler.get_percentile(0.9), 42);
}

TEST(Sampler, single_median) {
  Sampler<double, 9> sampler;
  for (unsigned char i = 0; i < 9; i++)
    sampler.set_next_sample(i);
  throw_assert_eq(sampler.get_samples_count(), 9);
  throw_assert_eq(sampler.get_min(), 0);
  throw_assert_eq(sampler.get_max(), 8);
  throw_assert_eq(sampler.get_mean(), 4);
  throw_assert_eq(sampler.get_median(), 4);
  throw_assert_eq(sampler.get_percentile(0.9), 8);
}

TEST(Sampler, too_many_samples) {
  Sampler<uint32_t, 10> sampler;
  for (unsigned char i = 0; i < 15; i++)
    sampler.set_next_sample(i);
  throw_assert_eq(sampler.get_samples_count(), 10);
}
}
}
