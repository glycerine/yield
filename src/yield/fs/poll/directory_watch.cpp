// yield/fs/poll/directory_watch.cpp

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

#include "directory_watch.hpp"
#include "yield/assert.hpp"
#include "yield/event_handler.hpp"
#include "yield/fs/directory.hpp"
#include "yield/fs/file_system.hpp"

namespace yield {
namespace fs {
namespace poll {
using std::make_pair;
using std::map;
using std::pair;

DirectoryWatch::DirectoryWatch(
  FSEvent::Type fs_event_types,
  const Path& path,
  Log* log
) : Watch(fs_event_types, path, log) {
  dentries = NULL;
}

DirectoryWatch::~DirectoryWatch() {
  delete dentries;
}

Directory::Entry::Type
DirectoryWatch::get_directory_entry_type(
  const Stat& stbuf
) const {
#ifdef _WIN32
  if (stbuf.ISDEV())
    return Directory::Entry::TYPE_DEV;
  else if (stbuf.ISDIR())
    return Directory::Entry::TYPE_DIR;
  else if (stbuf.ISREG())
    return Directory::Entry::TYPE_REG;
#else
  if (stbuf.ISBLK())
    return Directory::Entry::TYPE_BLK;
  else if (stbuf.ISCHR())
    return Directory::Entry::TYPE_CHR;
  else if (stbuf.ISDIR())
    return Directory::Entry::TYPE_DIR;
  else if (stbuf.ISFIFO())
    return Directory::Entry::TYPE_FIFO;
  else if (stbuf.ISLNK())
    return Directory::Entry::TYPE_LNK;
  else if (stbuf.ISREG())
    return Directory::Entry::TYPE_REG;
  else if (stbuf.ISSOCK())
    return Directory::Entry::TYPE_SOCK;
#endif
  else {
    debug_break();
    return Directory::Entry::TYPE_REG;
  }
}

void DirectoryWatch::read(EventHandler& fs_event_handler) {
  Directory* directory = FileSystem().opendir(this->get_path());
  if (directory != NULL) {
    map<Path, Stat*>* new_dentries = new map<Path, Stat*>;
    map<Path, Stat*>* old_dentries = dentries;
    dentries = NULL;

    vector< pair<Path, Stat*> > new_new_dentries;

    Directory::Entry* dentry = directory->read();
    if (dentry != NULL) {
      for (;;) {
        if (!dentry->is_special()) {
          Path dentry_path = this->get_path() / dentry->get_name();

          Stat* new_dentry_stat = FileSystem().stat(dentry_path);
          debug_assert_ne(new_dentry_stat, NULL);
          new_dentries->insert(make_pair(dentry->get_name(), new_dentry_stat));

          if (old_dentries != NULL) {
            map<Path, Stat*>::iterator old_dentry_i
              = old_dentries->find(dentry->get_name());
            if (old_dentry_i != old_dentries->end()) {
              Stat* old_dentry_stat = old_dentry_i->second;

              if (
                dentry->get_type()
                ==
                get_directory_entry_type(*old_dentry_stat)
              ) {
                if (
                  new_dentry_stat->get_mtime() != old_dentry_stat->get_mtime()
                ) {
                  FSEvent* fs_event
                    = new FSEvent(
                            this->get_path() / dentry->get_name(),
                            new_dentry_stat->ISDIR() ?
                              FSEvent::TYPE_DIRECTORY_MODIFY :
                              FSEvent::TYPE_FILE_MODIFY
                          );
                  fs_event_handler.handle(*fs_event);
                }
              } else // dentry type has changed
                debug_break();

              delete old_dentry_i->second;
              old_dentries->erase(old_dentry_i);
            } else { // old_dentry_i == old_dentries->end()
              // Add the dentry to new_new_dentries to check for renaming below
              new_new_dentries.push_back(
                make_pair(dentry->get_name(), new_dentry_stat)
              );
            }
          }
        }

        Directory::Entry::dec_ref(*dentry);
        if (!directory->read(*dentry)) break;
      }

      if (old_dentries != NULL) {
        // Check the remaining old_dentries against new_new_dentries
        // to match up any renames.
        for (
          map<Path, Stat*>::iterator old_dentry_i = old_dentries->begin();
          old_dentry_i != old_dentries->end();
          ++old_dentry_i
        ) {
          if (!new_new_dentries.empty()) {
            Stat* old_dentry_stat = old_dentry_i->second;

            for (
              vector< pair<Path, Stat*> >::iterator new_dentry_i
                = new_new_dentries.begin();
              new_dentry_i != new_new_dentries.end();
            ) {
              Stat* new_dentry_stat = new_dentry_i->second;

              if (
                get_directory_entry_type(*old_dentry_stat)
                  ==
                  get_directory_entry_type(*new_dentry_stat)
                &&
                old_dentry_stat->get_size() == new_dentry_stat->get_size()
              ) {
                FSEvent* fs_event
                  = new FSEvent(
                          this->get_path() / old_dentry_i->first,
                          this->get_path() / new_dentry_i->first,
                          new_dentry_stat->ISDIR() ?
                            FSEvent::TYPE_DIRECTORY_RENAME :
                            FSEvent::TYPE_FILE_RENAME
                        );
                fs_event_handler.handle(*fs_event);
                // Don't delete new_dentry_i->second, the Stat;
                // it's owned by new_dentries
                new_dentry_i = new_new_dentries.erase(new_dentry_i);
                delete old_dentry_i->second;
                old_dentry_i->second = NULL;
                break;
              } else
                ++new_dentry_i;
            }
          }
        }

        // Any remaining old_dentries here are removes.
        for (
          map<Path, Stat*>::iterator old_dentry_i = old_dentries->begin();
          old_dentry_i != old_dentries->end();
          ++old_dentry_i
        ) {
          if (old_dentry_i->second != NULL) {
            FSEvent* fs_event 
              = new FSEvent(
                      this->get_path() / old_dentry_i->first,
                      old_dentry_i->second->ISDIR() ?
                        FSEvent::TYPE_DIRECTORY_REMOVE :
                        FSEvent::TYPE_FILE_RENAME
                    );
            fs_event_handler.handle(*fs_event);
            delete old_dentry_i->second;
          }
        }

        delete old_dentries;
      }
    }

    dentries = new_dentries;
  } else if (dentries != NULL) { // Could not open directory
    for (
      map<Path, Stat*>::const_iterator dentry_i = dentries->begin();
      dentry_i != dentries->end();
      ++dentry_i
    ) {
      FSEvent* fs_event
        = new FSEvent(
                this->get_path() / dentry_i->first,
                dentry_i->second->ISDIR() ?
                  FSEvent::TYPE_DIRECTORY_REMOVE :
                  FSEvent::TYPE_FILE_REMOVE
              );
      fs_event_handler.handle(*fs_event);
      delete dentry_i->second;
    }

    delete dentries;
  }
}
}
}
}
