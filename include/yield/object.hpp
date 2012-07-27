// yield/object.hpp

// Copyright (c) 2012 Minor Gordon
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

#ifndef _YIELD_OBJECT_HPP_
#define _YIELD_OBJECT_HPP_

#include "yield/atomic.hpp"

namespace yield {
/**
  Base class for atomic reference-counted objects, similar to the TR1 intrusive_ptr.
  Also returns rudimentary run-time type information.
*/
class Object {
public:
  /**
    Cast an Object to a class derived from Object, using Object::get_type_id
      and ObjectType::TYPE_ID to determine whether the Object is of
      the derived type.
    Checks that object != NULL.
    @param object the Object to downcast
    @return an instance of the Object-derived class ObjectType
  */
  template <class ObjectType>
  static ObjectType* cast(Object* object) {
    if (object != NULL && object->get_type_id() == ObjectType::TYPE_ID) {
      return static_cast<ObjectType*>(object);
    } else {
      return NULL;
    }
  }

  /**
    Cast an Object to a class derived from Object, using Object::get_type_id
      and ObjectType::TYPE_ID to determine whether the Object is of
      the derived type.
    @param object the Object to downcast
    @return an instance of the Object-derived class ObjectType
  */
  template <class ObjectType>
  static ObjectType* cast(Object& object) {
    if (object.get_type_id() == ObjectType::TYPE_ID) {
      return static_cast<ObjectType*>(&object);
    } else {
      return NULL;
    }
  }

public:
  /**
    Atomically decrement the reference count of an Object, deleting the
    Object when the count reaches zero.
    @param object the object whose reference count should be decremented.
  */
  static inline void dec_ref(Object& object) {
    if (atomic_dec(&object.refcnt) == 0) {
      delete &object;
    }
  }

  /**
    Atomically decrement the reference count of an Object, deleting the
      Object when the count reaches zero.
    Checks that object != NULL.
    @param object the Object whose reference count should be decremented.
  */
  static inline void dec_ref(Object* object) {
    if (object != 0) {
      Object::dec_ref(*object);
    }
  }

public:
  /**
    Return a numeric run-time type ID.
    @return a numeric run-time type ID
  */
  virtual uint32_t get_type_id() const {
    return 0;
  }

  /**
    Return a human-readable, fully-qualified run-time type name.
    @return a human-readable, fully-qualified run-time type name
  */
  virtual const char* get_type_name() const {
    return "yield::Object";
  }

public:
  /**
    Atomically increment the reference count of an Object and return it.
    @param object the Object whose reference count should be incremented
    @return object
  */
  template <class ObjectType>
  static inline ObjectType& inc_ref(ObjectType& object) {
    atomic_inc(&object.refcnt);
    return object;
  }

  /**
    Atomically increment the reference count of an Object and return it.
    Checks that object != NULL.
    @param object the Object whose reference count should be incremented
    @return object
  */
  template <class ObjectType>
  static inline ObjectType* inc_ref(ObjectType* object) {
    if (object != 0) {
      inc_ref(*object);
    }

    return object;
  }

  /**
    Atomically increment the reference count of this Object and return it.
    @return *this
  */
  inline Object& inc_ref() {
    inc_ref(*this);
    return *this;
  }

protected:
  Object() : refcnt(1) { }
  virtual ~Object() { }

private:
  volatile atomic_t refcnt;
};


/**
  Macro indicating a new Object reference should be passed to a
    function/method or is returned by a function/method.
*/
#define YO_NEW_REF
}

#endif
