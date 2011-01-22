// yield/fs/extended_attributes_test.hpp

// Copyright (c) 2010 Minor Gordon
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


#ifndef _YIELD_FS_EXTENDED_ATTRIBUTES_TEST_HPP_
#define _YIELD_FS_EXTENDED_ATTRIBUTES_TEST_HPP_


#include "yield/exception.hpp"
#include "yield/assert.hpp"
#include "yield/fs/extended_attributes.hpp"
#include "yunit.hpp"


namespace yield {
namespace fs {
class ExtendedAttributesTest {
protected:
  ExtendedAttributesTest()
    : test_xattr_value("test_xattr_value")
  { }

  const char* get_test_xattr_name() const {
    return "user.test_xattr";
  }
  const string& get_test_xattr_value() const {
    return test_xattr_value;
  }

  YO_NEW_REF ExtendedAttributes*
  set_test_xattr
  (
    YO_NEW_REF ExtendedAttributes* xattrs
  ) {
    if (xattrs != NULL) {
      if (xattrs->set(get_test_xattr_name(), get_test_xattr_value()))
        return xattrs;
    }

    if
    (
      Exception::get_last_error_code() == ENOTSUP
      ||
      Exception::get_last_error_code() == EOPNOTSUPP
    ) {
      ExtendedAttributes::dec_ref(xattrs);
      return NULL;
    } else {
      Exception exc(Exception::get_last_error_code());
      ExtendedAttributes::dec_ref(xattrs);
      throw exc;
    }
  }

private:
  string test_xattr_value;
};


class ExtendedAttributesGetTest : public ExtendedAttributesTest {
protected:
  void run(ExtendedAttributes* xattrs) {
    if ((xattrs = set_test_xattr(xattrs)) != NULL) {
      string xattr_value;
      xattrs->get(get_test_xattr_name(), xattr_value);
      ExtendedAttributes::dec_ref(*xattrs);
      throw_assert_eq(xattr_value, get_test_xattr_value());
    }
  }
};


class ExtendedAttributesListTest : public ExtendedAttributesTest {
protected:
  void run(ExtendedAttributes* xattrs) {
    if ((xattrs = set_test_xattr(xattrs)) != NULL) {
      vector<string> names;
      xattrs->list(names);
      ExtendedAttributes::dec_ref(*xattrs);

      throw_assert_ge(names.size(), 1);

      for
      (
        vector<string>::const_iterator name_i = names.begin();
        name_i != names.end();
        name_i++
      ) {
        if (*name_i == get_test_xattr_name())
          return;
      }

      throw_assert(false);
    }
  }
};


class ExtendedAttributesRemoveTest : public ExtendedAttributesTest {
protected:
  void run(ExtendedAttributes* xattrs) {
    if ((xattrs = set_test_xattr(xattrs)) != NULL) {
      if (xattrs->remove(get_test_xattr_name()))
        ExtendedAttributes::dec_ref(*xattrs);
      else {
        ExtendedAttributes::dec_ref(*xattrs);
        throw Exception();
      }
    }
  }
};


class ExtendedAttributesSetTest : public ExtendedAttributesTest {
protected:
  void run(ExtendedAttributes* xattrs) {
    ExtendedAttributes::dec_ref(set_test_xattr(xattrs));
  }
};
}
}


#endif
