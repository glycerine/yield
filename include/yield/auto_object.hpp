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

#include "yield/object.hpp"

#include <exception>

namespace yield {
/**
  A scoped pointer for Objects.
  auto_Object is similar to the TR1 intrusive_ptr. It takes an Object reference
    (no NULLs allowed). Instead of delete auto_Object decrements the Object
    on destruction.

 auto_Object is primarily intended for use in testing, where an object
   should be deleted when it goes out of scope because of an exception.
*/
template <class ObjectType = Object>
class auto_Object {
public:
  /**
    Construct an auto_Object from an Object pointer.
    Throws a std::exception if object is NULL.
  */
  auto_Object(YO_NEW_REF ObjectType* object) throw(std::exception)
    : object(*object) {
    if (object == NULL) {
      throw std::exception();
    }
  }

  /**
    Construct an auto_Object from an Object reference.
  */
  auto_Object(YO_NEW_REF ObjectType& object)
    : object(object)
  { }

  /**
    Construct an auto_Object from another auto_Object by creating a new
      reference to the other auto_Object's contained Object.
  */
  auto_Object(const auto_Object<ObjectType>& other)
    : object(Object::inc_ref(other.object))
  { }

  /**
    Destruct the auto_Object, decrementing the reference count of the
      contained Object.
  */
  ~auto_Object() {
    Object::dec_ref(object);
  }

public:
  /**
    Return the contained Object reference.
    @return the contained Object reference
  */
  inline ObjectType& get() const {
    return object;
  }

  /**
    Return the contained Object reference.
    @return the contained Object reference
  */
  inline ObjectType& operator*() const {
    return get();
  }

  /**
    Return the contained Object reference as a pointer.
    @return the contained Object reference as a pointer
  */
  inline ObjectType* operator->() const {
    return &get();
  }

public:
  /**
    Compare this auto_Object's Object reference (a pointer comparison) to another
      auto_Object's Object reference.
    @param other the other auto_Object to compare to.
    @return true if the Object references are the same
  */
  inline bool operator==(const auto_Object<ObjectType>& other) const {
    return &get() == &other.get();
  }

  /**
    Compare this auto_Object's Object reference (a pointer comparison) to another
      auto_Object's Object reference.
    @param other the other auto_Object to compare to.
    @ return false if the Object references are not the same
  */
  inline bool operator!=(const auto_Object<ObjectType>& other) const {
    return &get() != &other.get();
  }

private:
  ObjectType& object;
};
}

#endif
