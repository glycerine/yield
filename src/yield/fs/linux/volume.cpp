// yield/fs/linux/volume.cpp

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


#include "directory.hpp"
#include "extended_attributes.hpp"
#include "file.hpp"
#include "volume.hpp"

#include <fcntl.h>


namespace yield {
namespace fs {
namespace linux {
yield::fs::File*
Volume::open
(
  const Path& path,
  uint32_t flags,
  mode_t mode,
  uint32_t attributes
) {
  fd_t fd = ::open(path.c_str(), flags, mode);
  if (fd != -1)
    return new File(fd);
  else
    return NULL;
}

YO_NEW_REF yield::fs::Directory* Volume::opendir(const Path& path) {
  DIR* dirp = ::opendir(path.c_str());
  if (dirp != NULL)
    return new Directory(dirp, path);
  else
    return NULL;
}

yield::fs::ExtendedAttributes*
Volume::openxattrs
(
  const Path& path
) {
  return new ExtendedAttributes(path);
}
}
}
}
