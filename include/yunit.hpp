// yunit.hpp

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


#ifndef _YUNIT_HPP_
#define _YUNIT_HPP_


#include <cstdio>
#include <iostream> // For cout, cerr, and endl
#include <string>
#include <vector>
#include <utility> // For pair and make_pair

#ifdef _WIN32
#include <WinError.h>
#ifdef ENOTSUP
#undef ENOTSUP
#endif
#define ENOTSUP ERROR_NOT_SUPPORTED
#ifdef EOPNOTSUPP
#undef EOPNOTSUPP
#endif
#define EOPNOTSUPP ERROR_NOT_SUPPORTED
#else
#include <errno.h>
#endif


#define TEST_EX( TestSuiteName, TestName, TestType )\
extern yunit::TestSuite& TestSuiteName##TestSuite();\
class TestSuiteName##TestName##Test : public TestType\
{\
public:\
  TestSuiteName##TestName##Test()\
  {\
    ::TestSuiteName##TestSuite().add\
    (\
       # TestSuiteName "::" # TestName,\
       this\
    );\
  }\
  void run();\
};\
TestSuiteName##TestName##Test* TestSuiteName##TestName##Test_\
  = new TestSuiteName##TestName##Test;\
void TestSuiteName##TestName##Test::run()

#define TEST( TestSuiteName, TestName )\
  TEST_EX( TestSuiteName, TestName, yunit::Test )


#define TEST_SUITE_EX( TestSuiteName, TestSuiteType )\
yunit::TestSuite& TestSuiteName##TestSuite()\
{\
  static TestSuiteType* test_suite = new TestSuiteType();\
  return *test_suite;\
}\
class TestSuiteName##TestSuiteDest\
{\
public:\
  ~TestSuiteName##TestSuiteDest()\
  {\
    delete &TestSuiteName##TestSuite();\
  }\
};\
TestSuiteName##TestSuiteDest TestSuiteName##TestSuiteDest_;

#define TEST_SUITE( TestSuiteName )\
  TEST_SUITE_EX( TestSuiteName, yunit::TestSuite )



namespace yunit {
class TestSuite;


class Test {
public:
  virtual ~Test() { }

  virtual void setup() { }
  virtual void run() = 0;
  virtual void teardown() { }
};


class TestSuite : private std::vector< std::pair<std::string, Test*> > {
public:
  virtual ~TestSuite() {
    for ( iterator test_i = begin(); test_i != end(); test_i++ )
      delete test_i->second;
  }

  void add( const std::string& name, Test* test ) {
    push_back( std::make_pair( name, test ) );
  }

  bool empty() const {
    return std::vector< std::pair< std::string, Test*> >::empty();
  }

  virtual int run() {
    int failed_test_count = 0;

    for ( iterator test_i = begin(); test_i != end(); ++test_i ) {
      bool called_run = false, called_teardown = false;

      try {
        std::cout << test_i->first;
        test_i->second->setup();
        called_run = true;
        test_i->second->run();
        called_teardown = true;
        test_i->second->teardown();
        std::cout << ": passed";
      } catch ( std::exception& exc ) {
        std::cout << " failed: " << exc.what();
        failed_test_count++;
      }

      std::cout << std::endl;

      if ( called_run && !called_teardown )
        try {
          test_i->second->teardown();
        } catch ( ... ) { }
    }

    return failed_test_count;
  }
};
}


#endif
