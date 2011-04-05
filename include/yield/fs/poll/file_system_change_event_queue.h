/* yield/fs/poll/file_system_change_event_queue.h */

/* Copyright (c) 2011 Minor Gordon */
/* All rights reserved */

/* This source file is part of the Yield project. */

/* Redistribution and use in source and binary forms, with or without */
/* modification, are permitted provided that the following conditions are met: */
/* * Redistributions of source code must retain the above copyright */
/* notice, this list of conditions and the following disclaimer. */
/* * Redistributions in binary form must reproduce the above copyright */
/* notice, this list of conditions and the following disclaimer in the */
/* documentation and/or other materials provided with the distribution. */
/* * Neither the name of the Yield project nor the */
/* names of its contributors may be used to endorse or promote products */
/* derived from this software without specific prior written permission. */

/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" */
/* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE */
/* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE */
/* ARE DISCLAIMED. IN NO EVENT SHALL Minor Gordon BE LIABLE FOR ANY DIRECT, */
/* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES */
/* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; */
/* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND */
/* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT */
/* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF */
/* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#ifndef _YIELD_FS_POLL_FILE_SYSTEM_CHANGE_EVENT_QUEUE_H_
#define _YIELD_FS_POLL_FILE_SYSTEM_CHANGE_EVENT_QUEUE_H_

#ifndef _YIELD_FS_POLL_FILE_SYSTEM_CHANGE_EVENT_QUEUE_H_
#define _YIELD_FS_POLL_FILE_SYSTEM_CHANGE_EVENT_QUEUE_H_

#include <map>
#include <queue>

#include "yield/object.h"
#include "yield/time.h"
#include "yield/fs/poll/file_system_change_event.h"

namespace yield {
namespace fs {
namespace poll {
using std::map;
using std::queue;

class VolumeChangeEventQueue : public Object {
public:
  typedef uint8_t Flag;
  const static Flag FLAG_DONT_FOLLOW = 1; // Don't follow symbolic links
  const static Flag FLAG_ONESHOT     = 2;
  const static Flag FLAG_ONLYDIR     = 4;
  const static Flag FLAG_RECURSIVE   = 8;
  const static Flag FLAGS_DEFAULT = FLAG_RECURSIVE;

public:
  virtual ~VolumeChangeEventQueue() { }

  virtual bool
  associate
  (
    const Path& path,
    VolumeChangeEvent::Type events = VolumeChangeEvent::TYPE_ALL,
    Flag flags = FLAGS_DEFAULT
  ) = 0;

  static YO_NEW_REF VolumeChangeEventQueue& create();
  virtual void dissociate( const Path& path ) = 0;

  bool
  poll
  (
    VolumeChangeEvent& volume_change_event,
    const Time& timeout = Time::FOREVER
  );

  virtual int
  poll
  (
    VolumeChangeEvent* volume_change_events,
    int volume_change_events_len,
    const Time& timeout
  ) = 0;

protected:
  class Watch
  {
  public:
    virtual ~Watch() { }

    VolumeChangeEvent::Type get_events() const { return events; }
    Flag get_flags() const { return flags; }
    const Path& get_path() const { return path; }
    bool isdir() const { return ( flags & FLAG_ONLYDIR ) == FLAG_ONLYDIR; }

    virtual bool read( VolumeChangeEvent& volume_change_event ) = 0;

  protected:
    Watch( VolumeChangeEvent::Type events, Flag flags, const Path& path )
      : events( events ), flags( flags ), path( path )
    { }

  private:
    VolumeChangeEvent::Type events;
    Flag flags;
    Path path;
  };


  template <class WatchType>
  class WatchMap : public map<Path, WatchType*>
  {
  public:
    WatchType* find( const Path& path )
    {
      typename map<Path, WatchType*>::const_iterator watch_i
        = map<Path, WatchType*>::find( path );

      if ( watch_i != this->end() )
        return watch_i->second;
      else
        return NULL;
    }

    WatchType* erase( const Path& path )
    {
      typename map<Path, WatchType*>::iterator watch_i
        = map<Path, WatchType*>::find( path );

      if ( watch_i != this->end() )
      {
        WatchType* watch = watch_i->second;
        map<Path, WatchType*>::erase( watch_i );
        return watch;
      }
      else
        return NULL;
    }

    void insert( const Path& path, WatchType& watch )
    {
      ( *this )[path] = &watch;
    }
  };

protected:
  VolumeChangeEventQueue() { }

  int get_leftover_volume_change_events( VolumeChangeEvent*, int );
  int read( VolumeChangeEvent*, int, Watch& watch );

private:
  queue<VolumeChangeEvent*> leftover_volume_change_events;
};
}
}
}

#endif
