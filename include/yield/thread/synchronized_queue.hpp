// yield/thread/synchronized_queue.hpp

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

#ifndef _YIELD_THREAD_SYNCHRONIZED_QUEUE_HPP_
#define _YIELD_THREAD_SYNCHRONIZED_QUEUE_HPP_

#include "yield/time.hpp"
#include "yield/thread/condition_variable.hpp"

#include <queue>


namespace yield {
namespace thread {
template <class ElementType>
class SynchronizedQueue : private std::queue<ElementType*> {
public:
  ElementType& dequeue() {
    cond.lock_mutex();

    while (std::queue<ElementType*>::empty())
      cond.wait();

    ElementType* element = std::queue<ElementType*>::front();
    std::queue<ElementType*>::pop();

    cond.unlock_mutex();

    return *element;
  }

  ElementType* dequeue(const Time& timeout) {
    Time timeout_left(timeout);

    cond.lock_mutex();

    if (!std::queue<ElementType*>::empty()) {
      ElementType* element = std::queue<ElementType*>::front();
      std::queue<ElementType*>::pop();
      cond.unlock_mutex();
      return element;
    } else {
      for (;;) {
        Time start_time = Time::now();

        cond.wait(timeout_left);

        if (!std::queue<ElementType*>::empty()) {
          ElementType* element = std::queue<ElementType*>::front();
          std::queue<ElementType*>::pop();
          cond.unlock_mutex();
          return element;
        } else {
          Time elapsed_time(Time::now() - start_time);
          if (elapsed_time < timeout_left)
            timeout_left -= elapsed_time;
          else {
            cond.unlock_mutex();
            return NULL;
          }
        }
      }
    }
  }

  bool enqueue(ElementType& element) {
    cond.lock_mutex();
    std::queue<ElementType*>::push(&element);
    cond.signal();
    cond.unlock_mutex();
    return true;
  }

  ElementType* trydequeue() {
    if (cond.trylock_mutex()) {
      if (!std::queue<ElementType*>::empty()) {
        ElementType* element = std::queue<ElementType*>::front();
        std::queue<ElementType*>::pop();
        cond.unlock_mutex();
        return element;
      } else
        cond.unlock_mutex();
    }

    return NULL;
  }

private:
  yield::thread::ConditionVariable cond;
};
}
}

#endif
