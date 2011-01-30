// yield/thread/sampler.hpp

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

#ifndef _YIELD_THREAD_SAMPLER_HPP_
#define _YIELD_THREAD_SAMPLER_HPP_

#include "yield/types.hpp"

#include <algorithm> // for std::sort


namespace yield {
namespace thread {
template <typename SampleType, size_t ArraySize>
class Sampler {
public:
  Sampler() {
    memset(samples, 0, sizeof(samples));
    samples_pos = samples_count = 0;
    min = static_cast<SampleType>(UINT64_MAX);
    max = 0;
    total = 0;
  }

  void clear() {
    //lock.acquire();
    samples_count = 0;
    //lock.release();
  }

  SampleType get_max() const {
    return max;
  }

  SampleType get_mean() {
    //lock.acquire();
    SampleType mean;

    if (samples_count > 0)
      mean = static_cast<SampleType>
             (
               static_cast<double>(total) /
               static_cast<double>(samples_count)
             );
    else
      mean = 0;

    //lock.release();
    return mean;
  }

  SampleType get_median() {
    //lock.acquire();

    SampleType median;

    if (samples_count > 0) {
      std::sort(samples, samples + samples_count);
      size_t sc_div_2 = samples_count / 2;
      if (samples_count % 2 == 1)
        median = samples[sc_div_2];
      else {
        SampleType median_temp = samples[sc_div_2] + samples[sc_div_2 - 1];
        if (median_temp > 0)
          median = static_cast<SampleType>
                   (
                     static_cast<double>(median_temp) / 2.0
                   );
        else
          median = 0;
      }
    } else
      median = 0;

    //lock.release();

    return median;
  }

  SampleType get_min() const {
    return min;
  }

  SampleType get_percentile(double percentile) {
    if (percentile > 0 && percentile < 100) {
      //lock.acquire();

      SampleType value;

      if (samples_count > 0) {
        std::sort(samples, samples + samples_count);
        value =
          samples[static_cast<size_t>(percentile *
                                      static_cast<double>(samples_count))];
      } else
        value = 0;

      //lock.release();

      return value;
    } else
      return 0;
  }

  uint32_t get_samples_count() const {
    return samples_count;
  }

  void set_next_sample(SampleType sample) {
    //if ( lock.try_acquire() )
    //{
    samples[samples_pos] = sample;
    samples_pos = (samples_pos + 1) % ArraySize;
    if (samples_count < ArraySize) samples_count++;

    if (sample < min)
      min = sample;
    if (sample > max)
      max = sample;
    total += sample;

    //  lock.release();
    //}
  }

protected:
  SampleType samples[ArraySize + 1], min, max;
  SampleType total;
  uint32_t samples_pos, samples_count;
  // LockType lock;
};
};
};

#endif
