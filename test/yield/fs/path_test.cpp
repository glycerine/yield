// path_test.cpp

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

#include "yield/config.hpp"
#include "yield/assert.hpp"
#include "yield/fs/path.hpp"
#include "yunit.hpp"

#include <utility>
#ifdef _WIN32
#include <Windows.h>
#endif


TEST_SUITE(Path);

namespace yield {
namespace fs {
using std::pair;


const static Path TEST_DIR_NAME("path_test");
const static Path TEST_FILE_NAME("path_test.txt");


TEST(Path, copy) {
  Path path_copy(TEST_FILE_NAME);
  throw_assert_eq(path_copy, TEST_FILE_NAME);
  throw_assert_eq(TEST_FILE_NAME, path_copy);
}

//TEST( Path, cast )
//{
//  Path path( TEST_FILE_NAME );
//  string path_str = path;
//  throw_assert_eq( path, path_str );
//#ifdef _WIN32
//  wstring path_wstr = ( wstring )path;
//  throw_assert_eq( path, path_wstr );
//#endif
//}

TEST(Path, empty) {
  throw_assert_false(TEST_FILE_NAME.empty());
  Path empty_path;
  throw_assert(empty_path.empty());
}

//TEST( Path, ends_with_separator )
//{
//#ifdef _WIN32
//  {
//    Path path( L"test_dir\\" );
//    const string& narrow_path = path;
//    throw_assert_ne
//    (
//      narrow_path[narrow_path.size()-1],
//      Path::SEPARATOR
//    );
//    const wstring& wide_path = path;
//    throw_assert_ne( wide_path[wide_path.size()-1], Path::SEPARATOR );
//  }
//#endif
//
//  {
//    Path path( TEST_FILE_NAME );
//    path = path + Path::SEPARATOR_STRING;
//    const string& narrow_path = path;
//    throw_assert_ne
//    (
//      narrow_path[narrow_path.size()-1],
//      Path::SEPARATOR
//    );
//#ifdef _WIN32
//    const wstring& wide_path = path;
//    throw_assert_ne( wide_path[wide_path.size()-1], Path::SEPARATOR );
//#endif
//  }
//}

TEST(Path, equals) {
  Path path(TEST_FILE_NAME);
  throw_assert_eq(path, TEST_FILE_NAME);
}

TEST(Path, join) {
  // Empty left, non-empty right
  {
    Path joined = Path() / TEST_FILE_NAME;
    throw_assert_eq(joined, TEST_FILE_NAME);
  }

  // Non-empty left, empty right
  {
    Path joined = TEST_DIR_NAME / Path();
    throw_assert_eq(joined, TEST_DIR_NAME);
  }

  // Non-empty left, non-empty right
  {
    Path joined = TEST_DIR_NAME / TEST_FILE_NAME;
#ifdef _WIN32
    throw_assert_eq(joined, L"path_test\\path_test.txt");
#else
    throw_assert_eq(joined, "path_test/path_test.txt");
#endif
  }
}

TEST(Path, split_head_tail) {
  {
    Path path("head");
    pair<Path, Path> split_path = path.split();
    throw_assert(split_path.first.empty());
    throw_assert_eq(split_path.second, Path("head"));
  }

  {
    Path path = Path("head") / Path("tail");
    pair<Path, Path> split_path = path.split();
    throw_assert_eq(split_path.first, Path("head"));
    throw_assert_eq(split_path.second, Path("tail"));
  }

  {
    Path path = Path("head1") / Path("head2") / Path("tail");
    pair<Path, Path> split_path = path.split();
    throw_assert_eq(split_path.first, (Path("head1") / Path("head2")));
    throw_assert_eq(split_path.second, Path("tail"));
  }
}

TEST(Path, split_parts) {
  Path path = Path("path1") / Path("path2") / Path("path3");
  vector<Path> path_parts;
  path.split(path_parts);
  throw_assert_eq(path_parts.size(), 3);
  throw_assert_eq(path_parts[0], Path("path1"));
  throw_assert_eq(path_parts[1], Path("path2"));
  throw_assert_eq(path_parts[2], Path("path3"));
}

TEST(Path, splitext) {
  {
    Path path("test");
    pair<Path, Path> splitext_path = path.splitext();
    throw_assert_eq(splitext_path.first, Path("test"));
    throw_assert(splitext_path.second.empty());
  }

  {
    Path path("test.txt");
    pair<Path, Path> splitext_path = path.splitext();
    throw_assert_eq(splitext_path.first, Path("test"));
    throw_assert_eq(splitext_path.second, Path(".txt"));
  }

  {
    Path path("test.txt.txt");
    pair<Path, Path> splitext_path = path.splitext();
    throw_assert_eq(splitext_path.first, Path("test.txt"));
    throw_assert_eq(splitext_path.second, Path(".txt"));
  }
}
}
}
