// yield/fs/stat.cpp

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


#include "yield/fs/stat.hpp"


namespace yield {
namespace fs {
const DateTime Stat::INVALID_ATIME = DateTime::INVALID_DATE_TIME;
const DateTime Stat::INVALID_CTIME = DateTime::INVALID_DATE_TIME;
const DateTime Stat::INVALID_MTIME = DateTime::INVALID_DATE_TIME;


bool Stat::has_attributes() const {
  return get_attributes() != INVALID_ATTRIBUTES;
}

bool Stat::operator==( const Stat& other ) const {
  return get_attributes() == other.get_attributes() &&
         get_dev() == other.get_dev() &&
         get_ino() == other.get_ino() &&
         get_mode() == other.get_mode() &&
         get_nlink() == other.get_nlink() &&
         get_uid() == other.get_uid() &&
         get_gid() == other.get_gid() &&
         get_rdev() == other.get_rdev() &&
         get_size() == other.get_size() &&
         get_atime() == other.get_atime() &&
         get_mtime() == other.get_mtime() &&
         get_ctime() == other.get_ctime() &&
         get_blksize() == other.get_blksize() &&
         get_blocks() == other.get_blocks();
}

//ostream& operator<<( ostream& os, const Stat& stbuf )
//{
//  os << "{ ";
//  os << "st_dev: " << stbuf.get_dev() << ", ";
//  os << "st_ino: " << stbuf.get_ino() << ", ";
//  #ifdef _WIN32
//    os << "st_mode: " << stbuf.get_mode() << ", ";
//  #else
//    os << "st_mode: " << stbuf.get_mode() << " (";
//    if ( ( stbuf.get_mode() & S_IREAD ) == S_IREAD ) os << "S_IREAD|";
//    if ( ( stbuf.get_mode() & S_IWRITE ) == S_IWRITE ) os << "S_IWRITE|";
//    if ( ( stbuf.get_mode() & S_IEXEC ) == S_IEXEC ) os << "S_IEXEC|";
//    if ( ( stbuf.get_mode() & S_IFDIR ) == S_IFDIR ) os << "S_IFDIR|";
//    if ( ( stbuf.get_mode() & S_IFCHR ) == S_IFCHR ) os << "S_IFCHR|";
//    if ( ( stbuf.get_mode() & S_IFREG ) == S_IFREG ) os << "S_IFREG|";
//    if ( ( stbuf.get_mode() & S_IFBLK ) == S_IFBLK ) os << "S_IFBLK|";
//    if ( ( stbuf.get_mode() & S_IFLNK ) == S_IFLNK ) os << "S_IFLNK|";
//    if ( ( stbuf.get_mode() & S_IRUSR ) == S_IFDIR ) os << "S_IRUSR|";
//    if ( ( stbuf.get_mode() & S_IWUSR ) == S_IWUSR ) os << "S_IWUSR|";
//    if ( ( stbuf.get_mode() & S_IXUSR ) == S_IXUSR ) os << "S_IXUSR|";
//    if ( ( stbuf.get_mode() & S_IRGRP ) == S_IRGRP ) os << "S_IRGRP|";
//    if ( ( stbuf.get_mode() & S_IWGRP ) == S_IWGRP ) os << "S_IWGRP|";
//    if ( ( stbuf.get_mode() & S_IXGRP ) == S_IXGRP ) os << "S_IXGRP|";
//    if ( ( stbuf.get_mode() & S_IROTH ) == S_IROTH ) os << "S_IROTH|";
//    if ( ( stbuf.get_mode() & S_IWOTH ) == S_IWOTH ) os << "S_IWOTH|";
//    if ( ( stbuf.get_mode() & S_IXOTH ) == S_IXOTH ) os << "S_IXOTH|";
//    if ( ( stbuf.get_mode() & S_ISUID ) == S_ISUID ) os << "S_ISUID|";
//    if ( ( stbuf.get_mode() & S_ISGID ) == S_ISGID ) os << "S_ISGID|";
//    if ( ( stbuf.get_mode() & S_ISVTX ) == S_ISVTX ) os << "S_ISVTX|";
//  #endif
//  os << "0), ";
//  os << "st_nlink: " << stbuf.get_nlink() << ", ";
//  os << "st_uid: " << stbuf.get_uid() << ", ";
//  os << "st_gid: " << stbuf.get_gid() << ", ";
//  os << "st_rdev: " << stbuf.get_rdev() << ", ";
//  os << "st_size: " << stbuf.get_size() << ", ";
//  os << "st_atime: " << stbuf.get_atime() << ", ";
//  os << "st_mtime: " << stbuf.get_mtime() << ", ";
//  os << "st_ctime: " << stbuf.get_ctime() << ", ";
//  os << "st_blksize: " << stbuf.get_blksize() << ", ";
//  os << "st_blocks: " << stbuf.get_blocks() << ", ";
//  os << "attributes: " << stbuf.get_attributes() << " }";
//  return os;
//}
}
}
