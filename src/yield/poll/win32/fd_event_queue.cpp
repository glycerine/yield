// yield/poll/win32/fd_event_queue.cpp

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

#include "yield/poll/fd_event_queue.hpp"

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <Windows.h>

namespace yield {
namespace poll {
class FDEventQueue::Impl : public EventQueue {
public:
  virtual ~Impl() {
  }

  virtual bool associate(fd_t fd, FDEvent::Type fd_event_types) = 0;
  virtual bool dissociate(fd_t fd) = 0;

protected:
  ::yield::queue::BlockingConcurrentQueue<Event> event_queue;
};


class FDEventQueue::FDImpl : public Impl {
public:
  FDImpl() {
    HANDLE hWakeEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (hWakeEvent != NULL) {
      fd_event_types.push_back(FDEvent::TYPE_READ_READY);
      handles.push_back(hWakeEvent);
    }
    else
      throw Exception();
  }

  ~FDImpl() {
    CloseHandle(handles[0]);
  }

public:
  // yield::EventQueue
  bool enqueue(Event& event) {
    if (event_queue.enqueue(event)) {
      SetEvent(handles[0]);
      return true;
    } else
      return false;
  }

  YO_NEW_REF Event* timeddequeue(const Time& timeout) {
    DWORD dwRet
    = WaitForMultipleObjectsEx(
        handles.size(),
        &handles[0],
        FALSE,
        static_cast<DWORD>(timeout.ms()),
        TRUE
      );

    if (dwRet == WAIT_OBJECT_0)
      return event_queue.trydequeue();
    else if (dwRet > WAIT_OBJECT_0 && dwRet < WAIT_OBJECT_0 + handles.size()) {
      return new FDEvent(
                   handles[dwRet - WAIT_OBJECT_0],
                   fd_event_types[dwRet - WAIT_OBJECT_0]
                 );
    }
    else
      return NULL;
  }

public:
  // yield::poll::FDEventQueue::Impl
  bool associate(fd_t fd, FDEvent::Type fd_event_types) {
    if (fd_event_types != 0) {
      if (handles.size() < MAXIMUM_WAIT_OBJECTS) {
        if (
          fd_event_types == FDEvent::TYPE_READ_READY
          ||
          fd_event_types == FDEvent::TYPE_WRITE_READY
        ) {
          for (size_t fd_i = 1; fd_i < handles.size(); ++fd_i) {
            if (handles[fd_i] == fd) {
              this->fd_event_types[fd_i] = fd_event_types;
              return true;
            }
          }

          this->fd_event_types.push_back(fd_event_types);
          handles.push_back(fd);

          return true;
        } else {
          SetLastError(ERROR_INVALID_PARAMETER);
          return false;
        }
      } else {
        SetLastError(ERROR_INVALID_PARAMETER);
        return false;
      }
    } else
      return dissociate(fd);
  }

  bool dissociate(fd_t fd) {
    for (size_t fd_i = 1; fd_i < handles.size(); ++fd_i) {
      if (handles[fd_i] == fd) {
        fd_event_types.erase(fd_event_types.begin() + fd_i);
        handles.erase(handles.begin() + fd_i);
        return true;
      }
    }

    SetLastError(ERROR_INVALID_HANDLE);

    return false;
  }

private:
  vector<FDEvent::Type> fd_event_types;
  vector<HANDLE> handles;
};

class FDEventQueue::SocketImpl : public Impl {
public:
  virtual bool associate(socket_t socket_, FDEvent::Type fd_event_types) = 0;
  virtual bool dissociate(socket_t socket_) = 0;

public:
  // yield::EventQuueue
  bool enqueue(YO_NEW_REF Event& event) {
    if (event_queue.enqueue(event)) {
      send(wake_socket_pair[1], "m", 1, 0);
      return true;
    } else
      return false;      
  }

public:
  // yield::poll::FDEventQueue::Impl
  bool associate(fd_t fd, FDEvent::Type fd_event_types) {
    return associate(reinterpret_cast<socket_t>(fd), fd_event_types);
  }

  bool dissociate(fd_t fd) {
    return dissociate(reinterpret_cast<socket_t>(fd));
  }

protected:
  SocketImpl() {
    wake_socket_pair[0] = wake_socket_pair[1] = INVALID_SOCKET;

    for (;;) {
      SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
      if (listen_socket != INVALID_SOCKET) {
        try {
          SOCKADDR_IN listen_sockname;
          ZeroMemory(&listen_sockname, sizeof(listen_sockname));
          listen_sockname.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);
          listen_sockname.sin_family = AF_INET;
          if (
            bind(
              listen_socket,
              reinterpret_cast<SOCKADDR*>(&listen_sockname),
              sizeof(listen_sockname)
            ) != -1
          ) {
            if (listen(listen_socket, 1) != -1) {
              int listen_socknamelen = sizeof(listen_sockname);
              if (
                getsockname(
                  listen_socket,
                  reinterpret_cast<SOCKADDR*>(&listen_sockname),
                  &listen_socknamelen
                ) != -1
              ) {
                wake_socket_pair[0] = socket(AF_INET, SOCK_STREAM, 0);
                if (wake_socket_pair[0] != INVALID_SOCKET) {
                  try {
                    if (
                      connect(
                        wake_socket_pair[0],
                        reinterpret_cast<SOCKADDR*>(&listen_sockname),
                        listen_socknamelen
                      ) != -1
                    ) {
                      SOCKADDR_IN peername;
                      int peernamelen = sizeof(peername);
                      wake_socket_pair[1]
                        = accept(
                            listen_socket,
                            reinterpret_cast<SOCKADDR*>(&peername),
                            &peernamelen
                          );
                      if (wake_socket_pair[1] != INVALID_SOCKET)
                        return;
                      else
                        throw Exception();
                    } else
                      throw Exception();
                  } catch (Exception&) {
                    closesocket(wake_socket_pair[0]);
                    throw;
                  }
                }
              } else
                throw Exception();
            } else
              throw Exception();
          } else
            throw Exception();
        } catch (Exception&) {
          closesocket(listen_socket);
          throw;
        }
      } else if (WSAGetLastError() == WSANOTINITIALISED) {
        WORD wVersionRequested = MAKEWORD(2, 2);
        WSADATA wsaData;
        WSAStartup(wVersionRequested, &wsaData);
        continue;
      } else
        throw Exception();
    }
  }

  ~SocketImpl() {
    closesocket(wake_socket_pair[0]);
    closesocket(wake_socket_pair[1]);
  }

protected:
  SOCKET wake_socket_pair[2];
};

#if _WIN32_WINNT >= 0x0600
class FDEventQueue::SocketPoller : public SocketImpl {
public:
  SocketPoller() {
    if (!associate(wake_socket_pair[0], FDEvent::TYPE_READ_READY))
      throw Exception();    
  }

public:
  // yield::EventQueue
  YO_NEW_REF Event* timeddequeue(const Time& timeout) {
    // Scan pollfds once for outstanding revents from the last WSAPoll,
    // then call WSAPoll again if necessary.
    int ret = 0;
    do {
      vector<pollfd>::iterator pollfd_i = pollfds.begin();

      do {
        pollfd& pollfd_ = *pollfd_i;

        if (pollfd_.revents != 0) {
          if (pollfd_.fd == wake_socket_pair[0]) {
            pollfd_.revents = 0;
            char m;
            recv(wake_socket_pair[0], &m, 1, 0);
            return event_queue.trydequeue();
          } else {
            fd_t fd = reinterpret_cast<fd_t>(pollfd_.fd);
            uint16_t revents;
            if (pollfd_.revents == (POLLERR | POLLHUP))
              revents = POLLHUP;
            else
              revents = pollfd_.revents;
            pollfd_.revents = 0;
            return new FDEvent(fd, revents);
          }
        }
      } while (++pollfd_i < pollfds.end());

      ret
      = WSAPoll(
          &pollfds[0],
          pollfds.size(),
          static_cast<int>(timeout.ms())
        );
    } while (ret > 0);

    return NULL;
  }

  // yield::poll::SocketImpl
  bool associate(socket_t socket_, uint16_t fd_event_types) {
    if (fd_event_types != 0) {
      for (
        vector<pollfd>::iterator pollfd_i = pollfds.begin();
        pollfd_i != pollfds.end();
        ++pollfd_i
      ) {
        if ((*pollfd_i).fd == socket_) {
          (*pollfd_i).events = fd_event_types;
          return true;
        }
      }

      pollfd pollfd_;
      memset(&pollfd_, 0, sizeof(pollfd_));
      pollfd_.fd = socket_;
      pollfd_.events = fd_event_types;
      pollfds.push_back(pollfd_);
      return true;
    } else
      return dissociate(socket_);
  }

  bool dissociate(socket_t socket_) {
    for (
      vector<pollfd>::iterator pollfd_i = pollfds.begin();
      pollfd_i != pollfds.end();
      ++pollfd_i
    ) {
      if ((*pollfd_i).fd == socket_) {
        pollfds.erase(pollfd_i);
        return true;
      }
    }

    return false;
  }

private:
  vector<pollfd> pollfds;
};
#endif

class FDEventQueue::SocketSelector : public SocketImpl {
public:
  SocketSelector() {
    FD_ZERO(&except_fd_set);
    FD_ZERO(&read_fd_set);
    FD_ZERO(&write_fd_set);

    if (!associate(wake_socket_pair[0], POLLIN))
      throw Exception();
  }

public:
  // yield::EventQueue
  YO_NEW_REF Event* timeddequeue(const Time& timeout) {
    fd_set except_fd_set_copy, read_fd_set_copy, write_fd_set_copy;

    memcpy_s(
      &except_fd_set_copy,
      sizeof(except_fd_set_copy),
      &except_fd_set,
      sizeof(except_fd_set)
    );

    memcpy_s(
      &read_fd_set_copy,
      sizeof(read_fd_set_copy),
      &read_fd_set,
      sizeof(read_fd_set)
    );

    memcpy_s(
      &write_fd_set_copy,
      sizeof(write_fd_set_copy),
      &write_fd_set,
      sizeof(write_fd_set)
    );

    timeval timeout_tv;
    timeout_tv.tv_sec = static_cast<long>(timeout.ns() / Time::NS_IN_S);
    timeout_tv.tv_usec = (timeout.ns() % Time::NS_IN_S) / Time::NS_IN_US;

    int ret
    = select(
        0,
        &read_fd_set_copy,
        &write_fd_set_copy,
        &except_fd_set_copy,
        &timeout_tv
      );

    if (ret > 0) {
      vector<SOCKET>::const_iterator socket_i = sockets.begin();

      while (ret > 0 && socket_i != sockets.end()) {
        socket_t socket_ = *socket_i;

        uint16_t fd_event_types = 0;

        if (FD_ISSET(socket_, &except_fd_set_copy)) {
          fd_event_types |= POLLERR;
          ret--; // one for every socket event,
          // not every socket
        }

        if (
          ret > 0
          &&
          FD_ISSET(socket_, &read_fd_set_copy)
        ) {
          fd_event_types |= POLLIN;
          ret--;
        }

        if (
          ret > 0
          &&
          FD_ISSET(socket_, &write_fd_set_copy)
        ) {
          fd_event_types |= POLLOUT;
          ret--;
        }

        if (fd_event_types != 0) {
          if (socket_ == wake_socket_pair[0]) {
            char m;
            recv(wake_socket_pair[0], &m, 1, 0);
            return event_queue.trydequeue();
          }
          else {
            return new FDEvent(
                         reinterpret_cast<fd_t>(socket_),
                         fd_event_types
                       );
          }
        }

        ++socket_i;
      }
    }

    return NULL;
  }

public:
  // yield::poll::FDEventQueue::SocketImpl
  bool associate(socket_t socket_, uint16_t fd_event_types) {
    if (fd_event_types != 0) {
      bool added_socket_to_except_fd_set = false;

      if (fd_event_types & FDEvent::TYPE_READ_READY) {
        FD_SET(socket_, &read_fd_set);
        FD_SET(socket_, &except_fd_set);
        added_socket_to_except_fd_set = true;
      }
      else
        FD_CLR(socket_, &read_fd_set);

      if (fd_event_types & FDEvent::TYPE_WRITE_READY) {
        FD_SET(socket_, &write_fd_set);
        if (!added_socket_to_except_fd_set)
          FD_SET(socket_, &except_fd_set);
      }
      else
        FD_CLR(socket_, &write_fd_set);

      if (!added_socket_to_except_fd_set)
        FD_CLR(socket_, &except_fd_set);

      for (
        vector<SOCKET>::iterator socket_i = sockets.begin();
        socket_i != sockets.end();
        ++socket_i
      ) {
        if (*socket_i == socket_)
          return true;
      }

      sockets.push_back(socket_);
      return true;
    } else
      return dissociate(socket_);
  }

  bool dissociate(socket_t socket_) {
    for (
      vector<SOCKET>::iterator socket_i = sockets.begin();
      socket_i != sockets.end();
      ++socket_i
    ) {
      if (*socket_i == socket_) {
        sockets.erase(socket_i);
        FD_CLR(socket_, &except_fd_set);
        FD_CLR(socket_, &read_fd_set);
        FD_CLR(socket_, &write_fd_set);
        return true;
      }
    }

    return false;
  }

private:
  fd_set except_fd_set, read_fd_set, write_fd_set;
  vector<SOCKET> sockets;
};


FDEventQueue::FDEventQueue(bool for_sockets_only) throw (Exception) {
  if (for_sockets_only) {
#if _WIN32_WINNT >= 0x0600
    pimpl = new SocketPoller;
#else
    pimpl = new SocketSelector;
#endif
  } else
    pimpl = new FDImpl;
}

FDEventQueue::~FDEventQueue() {
  delete pimpl;
}

bool FDEventQueue::associate(fd_t fd, FDEvent::Type fd_event_types) {
  return pimpl->associate(fd, fd_event_types);
}

bool FDEventQueue::dissociate(fd_t fd) {
  return pimpl->dissociate(fd);
}

bool FDEventQueue::enqueue(Event& event) {
  return pimpl->enqueue(event);
}

YO_NEW_REF Event* FDEventQueue::timeddequeue(const Time& timeout) {
  return pimpl->timeddequeue(timeout);
}
}
}
