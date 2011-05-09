// yield/fs/poll/scanning_directory_watch.cpp

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

#include "scanning_directory_watch.hpp"
#include "yield/assert.hpp"
#include "yield/exception.hpp"
#include "yield/event_handler.hpp"
#include "yield/fs/file_system.hpp"

namespace yield {
namespace fs {
namespace poll {
using std::make_pair;
using std::map;
using std::pair;

ScanningDirectoryWatch::ScanningDirectoryWatch(
  FSEvent::Type fs_event_types,
  const Path& path,
  Log* log
) : ScanningWatch(fs_event_types, log, path) {
  dentries = new map<Path, Stat*>;
  Directory* directory = FileSystem().opendir(this->get_path());
  if (directory != NULL) {
    Directory::Entry* dentry = directory->read();
    if (dentry != NULL) {
      for (;;) {
        if (!dentry->is_special()) {
          Stat* dentry_stat = stat(*dentry);
          if (dentry_stat != NULL) {
            debug_assert_eq(
              dentries->find(dentry->get_name()),
              dentries->end()
            );
            dentries->insert(make_pair(dentry->get_name(), dentry_stat));
          }
        }

        if (!directory->read(*dentry))
          break;
      }
      Directory::Entry::dec_ref(*dentry);
    }
    Directory::dec_ref(*directory);
  } else
    throw Exception();
}

ScanningDirectoryWatch::~ScanningDirectoryWatch() {
  if (dentries != NULL) {
    for (
      map<Path, Stat*>::iterator dentry_i = dentries->begin();
      dentry_i != dentries->end();
      ++dentry_i
    )
      Stat::dec_ref(*dentry_i->second);
    delete dentries;
  }
}

void ScanningDirectoryWatch::scan(EventHandler& fs_event_handler) {
  Directory* directory = FileSystem().opendir(this->get_path());
  if (directory != NULL) {
    map<Path, Stat*>* new_dentries = new map<Path, Stat*>;
    vector< pair<Path, Stat*> > new_new_dentries;

    map<Path, Stat*>* old_dentries = dentries;

    Directory::Entry* new_dentry = directory->read();
    if (new_dentry != NULL) {
      for (;;) {
        if (!new_dentry->is_special()) {
          Stat* new_dentry_stat = stat(*new_dentry);
          if (new_dentry_stat != NULL) {
            debug_assert_eq(
              new_dentries->find(new_dentry->get_name()),
              new_dentries->end()
            );

            new_dentries->insert(make_pair(new_dentry->get_name(), new_dentry_stat));

            map<Path, Stat*>::iterator old_dentry_i
            = old_dentries->find(new_dentry->get_name());
            if (old_dentry_i != old_dentries->end()) {
              Stat* old_dentry_stat = old_dentry_i->second;

              if (new_dentry->get_type() == type(*old_dentry_stat)) {
                if (!equals(*new_dentry_stat, *old_dentry_stat)) {
                  FSEvent* fs_event
                  = new FSEvent(
                    this->get_path() / new_dentry->get_name(),
                    new_dentry_stat->ISDIR() ?
                    FSEvent::TYPE_DIRECTORY_MODIFY :
                    FSEvent::TYPE_FILE_MODIFY
                  );
                  log_fs_event(*fs_event);
                  fs_event_handler.handle(*fs_event);
                }
              } else // dentry type has changed
                debug_break();

              Stat::dec_ref(*old_dentry_i->second);
              old_dentries->erase(old_dentry_i);
            } else { // old_dentry_i == old_dentries->end()
              // Add the new_dentry to new_new_dentries to check for renaming below
              new_new_dentries.push_back(
                make_pair(new_dentry->get_name(), new_dentry_stat)
              );
            }
          }
        }

        if (!directory->read(*new_dentry))
          break;
      }

      // Check the remaining old_dentries against new_new_dentries
      // to match up any renames.
      if (!new_new_dentries.empty()) {
        for (
          map<Path, Stat*>::iterator old_dentry_i = old_dentries->begin();
          old_dentry_i != old_dentries->end();
          ++old_dentry_i
        ) {
            Stat* old_dentry_stat = old_dentry_i->second;

            for (
              vector< pair<Path, Stat*> >::iterator new_dentry_i
              = new_new_dentries.begin();
              new_dentry_i != new_new_dentries.end();
            ) {
              Stat* new_dentry_stat = new_dentry_i->second;

              if (equals(*old_dentry_stat, *new_dentry_stat)) {
                FSEvent* fs_event
                = new FSEvent(
                  this->get_path() / old_dentry_i->first,
                  this->get_path() / new_dentry_i->first,
                  new_dentry_stat->ISDIR() ?
                  FSEvent::TYPE_DIRECTORY_RENAME :
                  FSEvent::TYPE_FILE_RENAME
                );
                log_fs_event(*fs_event);
                fs_event_handler.handle(*fs_event);
                // Don't dec_ref new_dentry_i->second, the Stat;
                // it's owned by new_dentries
                new_dentry_i = new_new_dentries.erase(new_dentry_i);
                Stat::dec_ref(*old_dentry_i->second);
                old_dentry_i->second = NULL;
                break;
              } else
                ++new_dentry_i;
            }
        }

        // Any remaining new_new_dentries here are adds.
        for (
          vector< pair<Path, Stat*> >::iterator new_dentry_i
          = new_new_dentries.begin();
          new_dentry_i != new_new_dentries.end();
          ++new_dentry_i
        ) {
          FSEvent* fs_event
          = new FSEvent(
              this->get_path() / new_dentry_i->first,
              new_dentry_i->second->ISDIR() ?
                FSEvent::TYPE_DIRECTORY_ADD :
                FSEvent::TYPE_FILE_ADD
          );
          log_fs_event(*fs_event);
          fs_event_handler.handle(*fs_event);
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
            FSEvent::TYPE_FILE_REMOVE
          );
          log_fs_event(*fs_event);
          fs_event_handler.handle(*fs_event);
          Stat::dec_ref(*old_dentry_i->second);
        }
      }

      Directory::Entry::dec_ref(*new_dentry);
    }

    delete old_dentries;
    dentries = new_dentries;
  } else { // directory == NULL
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
      log_fs_event(*fs_event);
      fs_event_handler.handle(*fs_event);
      Stat::dec_ref(*dentry_i->second);
    }

    delete dentries;
    dentries = new map<Path, Stat*>;
    Directory::dec_ref(*directory);
  }
}

YO_NEW_REF Stat* ScanningDirectoryWatch::stat(Directory::Entry& dentry) {
#ifdef _WIN32
  return new Stat(dentry);
#else
  return FileSystem().stat(this->get_path() / dentry.get_name());
#endif
}
}
}
}
