// yield/auto_object.hpp

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

#ifndef _YIELD_AUTO_OBJECT_HPP_
#define _YIELD_AUTO_OBJECT_HPP_

#include "yield/exception.hpp"
#include "yield/object.hpp"


namespace yield {
// Similar to auto_ptr, but using object references instead of delete
// Unlike auto_ptr auto_Object is immutable, so there is no release(),
// reset(), or operator=().
// The class is primarily intended for use in testing, where an object
// should be deleted when it goes out of scope because of an exception.
// The *object constructor throws an Exception if object is NULL.
// That's why this is here and not in yidl.
template <class ObjectType = Object>
class auto_Object {
public:
  auto_Object(YO_NEW_REF ObjectType* object)
    : object(*object) {
    if (object == NULL)
      throw Exception();
  }

  auto_Object(YO_NEW_REF ObjectType& object)
    : object(object)
  { }

  ~auto_Object() {
    Object::dec_ref(object);
  }

  auto_Object(const auto_Object<ObjectType>& other)
    : object(Object::inc_ref(other.object))
  { }

  inline ObjectType& get() const {
    return object;
  }
  inline ObjectType* operator->() const {
    return &get();
  }
  inline ObjectType& operator*() const {
    return get();
  }

  inline bool operator==(const auto_Object<ObjectType>& other) const {
    return &get() == &other.get();
  }

  inline bool operator!=(const auto_Object<ObjectType>& other) const {
    return &get() != &other.get();
  }

private:
  ObjectType& object;
};
}

#endif
