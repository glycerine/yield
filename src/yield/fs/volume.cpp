// yield/fs/volume.cpp

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


#include "yield/auto_object.hpp"
#include "yield/fs/directory.hpp"
#include "yield/fs/file.hpp"
#include "yield/fs/memory_mapped_file.hpp"

#if defined(__linux__)
#include "linux/volume.hpp"
#elif defined(_WIN32)
#include "win32/volume.hpp"
#else
#include "posix/volume.hpp"
#endif

#include <fcntl.h> // For O_*
#ifndef _WIN32
#include <sys/mman.h> // For MAP_* and PROT_*
#endif
#include <sys/stat.h> // For S_*


namespace yield {
namespace fs {
using std::pair;


mode_t Volume::FILE_MODE_DEFAULT = S_IREAD | S_IWRITE;
mode_t Volume::DIRECTORY_MODE_DEFAULT = S_IREAD | S_IWRITE | S_IEXEC;
int Volume::MMAP_FLAGS_DEFAULT = MAP_SHARED;
int Volume::MMAP_PROT_DEFAULT = PROT_READ | PROT_WRITE;
uint32_t Volume::OPEN_FLAGS_DEFAULT = O_RDONLY;


class Volume::chmodStat : public Stat {
public:
  chmodStat(mode_t mode) : mode(mode) { }

  // Stat
  mode_t get_mode() const {
    return mode;
  }

private:
  mode_t mode;
};

class Volume::chownStat : public Stat {
public:
  chownStat(uid_t uid, gid_t gid) : uid(uid), gid(gid) { }

  // Stat
  gid_t get_gid() const {
    return gid;
  }
  uid_t get_uid() const {
    return uid;
  }

private:
  uid_t uid;
  gid_t gid;
};

class Volume::utimeStat : public Stat {
public:
  utimeStat
  (
    const DateTime& atime,
    const DateTime& mtime,
    const DateTime& ctime
  )
    : atime(atime),
      mtime(mtime),
      ctime(ctime)
  { }

  // Stat
  const DateTime& get_atime() const {
    return atime;
  }
  const DateTime& get_ctime() const {
    return ctime;
  }
  const DateTime& get_mtime() const {
    return mtime;
  }

private:
  DateTime atime, mtime, ctime;
};


bool Volume::chmod(const Path& path, mode_t mode) {
  return setattr(path, chmodStat(mode));
}

bool Volume::chown(const Path& path, uid_t uid, gid_t gid) {
  return setattr(path, chownStat(uid, gid));
}

File* Volume::creat(const Path& path) {
  return creat(path, FILE_MODE_DEFAULT);
}

File* Volume::creat(const Path& path, mode_t mode) {
  return open(path, O_CREAT | O_WRONLY | O_TRUNC, mode);
}

Volume* Volume::create() {
#if defined(__linux__)
  return new linux::Volume;
#elif defined(_WIN32)
  return new win32::Volume;
#else
  return new posix::Volume;
#endif
}

bool Volume::exists(const Path& path) {
  Stat* stbuf = getattr(path);
  if (stbuf != NULL) {
    Stat::dec_ref(*stbuf);
    return true;
  } else
    return false;
}

bool Volume::isdir(const Path& path) {
  Stat* stbuf = getattr(path);
  if (stbuf != NULL) {
    bool isdir = stbuf->ISDIR();
    Stat::dec_ref(*stbuf);
    return isdir;
  } else
    return false;
}

bool Volume::isfile(const Path& path) {
  Stat* stbuf = getattr(path);
  if (stbuf != NULL) {
    bool isfile = stbuf->ISREG();
    Stat::dec_ref(*stbuf);
    return isfile;
  } else
    return false;
}

bool Volume::mkdir(const Path& path) {
  return mkdir(path, DIRECTORY_MODE_DEFAULT);
}

bool Volume::mktree(const Path& path) {
  return mktree(path, DIRECTORY_MODE_DEFAULT);
}

bool Volume::mktree(const Path& path, mode_t mode) {
  bool ret = true;

  pair<Path, Path> path_parts = path.split();
  if (!path_parts.first.empty())
    ret &= mktree(path_parts.first, mode);

  if (!exists(path) && !mkdir(path, mode))
    return false;

  return ret;
}

bool Volume::rmtree(const Path& path) {
  Directory* test_dir = opendir(path);
  if (test_dir != NULL) {
    auto_Object<Directory> dir(test_dir);
    Directory::Entry* test_dentry = dir->read();
    if (test_dentry != NULL) {
      auto_Object<Directory::Entry> dentry(*test_dentry);

      do {
        if (dentry->is_special())
          continue;

        Path dentry_path(path / dentry->get_name());

        if (dentry->ISDIR()) {
          if (rmtree(dentry_path))
            continue;
          else
            return false;
        } else if (unlink(dentry_path))
          continue;
        else
          return false;
      } while (dir->read(*dentry));

      return rmdir(path);
    }
  }

  return false;
}

bool Volume::touch(const Path& path) {
  return touch(path, FILE_MODE_DEFAULT);
}

bool Volume::touch(const Path& path, mode_t mode) {
  File* file = creat(path, mode);
  if (file != NULL) {
    File::dec_ref(*file);
    return true;
  } else
    return false;
}

//bool
//Volume::trace
//(
//  Log& log,
//  const char* method,
//  const Path& path,
//  bool ret
//)
//{
//  uint32_t last_error_code = Exception::get_last_error_code();

//  {
//    Log::Stream log_stream = log.get_stream( Log::INFO );
//    log_stream << "Volume::" << method << "( " << path << " ) -> ";
//    if ( ret ) log_stream << "succeeded.";
//    else log_stream << "failed: " << Exception( last_error_code ) << ".";
//  }

//  Exception::set_last_error_code( last_error_code );

//  return ret;
//}

//bool
//Volume::trace
//(
//  Log& log,
//  const char* method,
//  const Path& path1,
//  const Path& path2,
//  bool ret
//)
//{
//  uint32_t last_error_code = Exception::get_last_error_code();

//  {
//    Log::Stream log_stream = log.get_stream( Log::INFO );
//    log_stream << "Volume::" << method <<
//      "( " << path1 << ", " << path2 << " ) -> ";
//    if ( ret ) log_stream << "succeeded.";
//    else log_stream << "failed: " << Exception( last_error_code ) << ".";
//  }

//  Exception::set_last_error_code( last_error_code );

//  return ret;
//}

//Stat* Volume::trace_getattr( Log& log, const Path& path, Stat* ret )
//{
//  uint32_t last_error_code = Exception::get_last_error_code();

//  {
//    Log::Stream log_stream = log.get_stream( Log::INFO );
//    log_stream << "Volume::getattr( " << path << " ) -> ";
//    if ( ret != NULL ) log_stream << *ret;
//    else log_stream << "failed: " << Exception( last_error_code ) << ".";
//  }

//  Exception::set_last_error_code( last_error_code );

//  return ret;
//}

//File*
//Volume::trace_open
//(
//  Log& log,
//  const Path& path,
//  uint32_t flags,
//  mode_t mode,
//  uint32_t attributes,
//  File* ret
//)
//{
//  uint32_t last_error_code = Exception::get_last_error_code();

//  {
//    Log::Stream log_stream = log.get_stream( Log::INFO );
//    log_stream << "Volume::open( " <<
//      path << ", " << flags << ", " << mode << ", " << attributes <<
//      " ) -> ";
//    if ( ret != NULL ) log_stream << "succeeded.";
//    else log_stream << "failed.";
//  }

//  Exception::set_last_error_code( last_error_code );

//  return ret;
//}

//Directory*
//Volume::trace_opendir
//(
//  Log& log,
//  const Path& path,
//  Directory* ret
//)
//{
//  uint32_t last_error_code = Exception::get_last_error_code();

//  {
//    Log::Stream log_stream = log.get_stream( Log::INFO );
//    log_stream << "Volume::opendir( " << path << " ) -> ";
//    if ( ret != NULL ) log_stream << "succeeded.";
//    else log_stream << "failed.";
//  }

//  Exception::set_last_error_code( last_error_code );

//  return ret;
//}

//bool
//Volume::trace_setattr
//(
//  Log& log,
//  const Path& path,
//  const Stat& stbuf,
//  bool ret
//)
//{
//  uint32_t last_error_code = Exception::get_last_error_code();

//  {
//    Log::Stream log_stream = log.get_stream( Log::INFO );
//    log_stream << "Volume::setattr( " << path << ", " << stbuf << " ) -> ";
//    if ( ret ) log_stream << "succeeded.";
//    else log_stream << "failed.";
//  }

//  Exception::set_last_error_code( last_error_code );

//  return ret;
//}

//bool
//Volume::trace_truncate
//(
//  Log& log,
//  const Path& path,
//  uint64_t new_size,
//  bool ret
//)
//{
//  uint32_t last_error_code = Exception::get_last_error_code();

//  {
//    Log::Stream log_stream = log.get_stream( Log::INFO );
//    log_stream << "Volume::truncate( " << path << ", " << new_size << " ) -> ";
//    if ( ret ) log_stream << "succeeded.";
//    else log_stream << "failed: " << Exception( last_error_code ) << ".";
//  }

//  Exception::set_last_error_code( last_error_code );

//  return ret;
//}

bool
Volume::utime
(
  const Path& path,
  const DateTime& atime,
  const DateTime& mtime
) {
  return setattr(path, utimeStat(atime, mtime, Stat::INVALID_CTIME));
}

bool
Volume::utime
(
  const Path& path,
  const DateTime& atime,
  const DateTime& mtime,
  const DateTime& ctime
) {
  return setattr(path, utimeStat(atime, mtime, ctime));
}
}
}
