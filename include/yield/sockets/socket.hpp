// yield/sockets/socket.hpp

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

#ifndef _YIELD_SOCKETS_SOCKET_HPP_
#define _YIELD_SOCKETS_SOCKET_HPP_

#include "yield/auto_object.hpp"
#include "yield/buffer.hpp"
#include "yield/buffers.hpp"
#include "yield/channel.hpp"
#include "yield/exception.hpp"
#include "yield/sockets/socket_address.hpp"

namespace yield {
namespace sockets {
/**
  Socket operations: recv, send, shutdown, et al.
  Wraps a platform-specific socket descriptor.
*/
class Socket : public Channel {
public:
  /**
    The default protocol in a (domain, type, protocol) socket(2) tuple.
  */
  const static int PROTOCOL_DEFAULT = 0;

public:
  /**
    An enum class representing the flags parameter of recv, recvmsg,
      send, sendmsg, et al.
  */
  class MessageFlags {
  public:
    /**
      The message should bypass routing table lookup.
    */
    const static int DONTROUTE = 1;

    /**
      The message should be sent or received out-of-band.
    */
    const static int OOB = 2;

    /**
      Peek for a message, leaving it in the socket buffers.
    */
    const static int PEEK = 4;

  public:
    /**
      Construct a MessageFlags from a platform-specific integer constant.
    */
    MessageFlags(int = 0);

    /**
      Cast this MessageFlags to its original platform-specific integer constant.
    */
    operator int() const {
      return platform_message_flags;
    }

  private:
    int platform_message_flags;
  };

public:
  /**
    An enum class representing options for setsockopt.
  */
  class Option {
  public:
    const static int RCVBUF;
    const static int REUSEADDR;
    const static int SNDBUF;
  };

public:
  /**
    Construct a Socket from a (domain, type, protocol) tuple, e.g.
      (AF_INET, SOCK_STREAM, IPPROTO_TCP) for TCP sockets.
  */
  Socket(int domain, int type, int protocol)
    : domain(domain),
      type(type),
      protocol(protocol) {
    socket_ = create(domain, type, protocol);
    if (socket_ == static_cast<socket_t>(-1))
      throw Exception();
  }

  /**
    Destruct a Socket, closing its socket descriptor.
  */
  virtual ~Socket() {
    close();
  }

public:
  /**
    Bind to the given address.
    @param to_sockaddr socket address to bind 
    @return true on success, false+errno on failure
  */
  virtual bool bind(const SocketAddress& to_sockaddr);

  /**
    Connect to a peer at the given address.
    @param peername address of the peer to connect to
    @return true on success, false+errno on failure
  */
  virtual bool connect(const SocketAddress& peername);

public:
  /**
    Get the domain (e.g., AF_INET) of this socket.
    @return the domain of this socket
  */
  int get_domain() const {
    return domain;
  }

public:
  /**
    Get the fully-qualified domain name (FQDN) of the current host.
    @return the fully-qualified domain name of the current host
  */
  static string getfqdn();

  /**
    Get the unqualified host name of the current host.
    @return the unqaulified host name of the current host.
  */
  static string gethostname();

public:
  /**
    Get the address of the peer this socket is connected to.
    @return the address of the peer this socket is connected to
  */
  auto_Object<SocketAddress> getpeername() const throw(Exception) {
    SocketAddress* peername = new SocketAddress;
    try {
      if (getpeername(*peername))
        return peername;
      else
        throw Exception();
    } catch (Exception&) {
      SocketAddress::dec_ref(*peername);
      throw;
    }
  }

  /**
    Get the address of the peer this socket is connected to.
    @param[out] peername the address of the peer this socket is connected to
    @return true on success, false+errno on failure
  */
  bool getpeername(SocketAddress& peername) const;

public:
  /**
    Get the protocol (e.g. IPPROTO_TCP) of this socket
    @return the protocol of this socket
  */
  int get_protocol() const {
    return protocol;
  }

public:
  /**
    Get the address this socket is bound to.
    @return the address this socket is bound to
  */
  auto_Object<SocketAddress> getsockname() const throw(Exception) {
    SocketAddress* sockname = new SocketAddress;
    try {
      if (getsockname(*sockname))
        return sockname;
      else
        throw Exception();
    } catch (Exception&) {
      SocketAddress::dec_ref(*sockname);
      throw;
    }
  }

  /**
    Get the address this socket is bound to.
    @param[out] sockname the address this socket is bound to
    @return true on success, false+errno on failure
  */
  bool getsockname(SocketAddress& sockname) const;

public:
  /**
    Get the type (e.g., SOCK_STREAM) of this socket.
    @return the type of this socket
  */
  int get_type() const {
    return type;
  }

public:
  /**
    Get the underlying socket descriptor.
    @return the underlying socket descriptor
  */
  virtual operator socket_t() const {
    return socket_;
  }

#ifdef _WIN32
  /**
    Get the underlying socket descriptor as a file descriptor.
    @return the underlying socket descriptor as a file descriptor
  */
  operator fd_t() const {
    return reinterpret_cast<fd_t>(static_cast<socket_t>(*this));
  }
#endif

public:
  /**
    Read from the socket into a Buffer.
    May be a recv(void*, size_t, const MessageFlags&) or a
      recvmsg(const iovec*, int, const MessageFlags&),
      depending on whether buffer is a single buffer or a linked list of them.
    Updates buffer's size depending on how many bytes were read.
    @param[in, out] buffer the buffer to read data into
    @param flags flags altering the behavior of the underlying system call
    @return the number of bytes read on success, -1+errno on failure
  */
  ssize_t recv(Buffer& buffer, const MessageFlags& flags) {
    if (buffer.get_next_buffer() == NULL) {
      iovec iov = buffer.as_read_iovec();
      ssize_t recv_ret = recv(iov.iov_base, iov.iov_len, flags);
      if (recv_ret > 0)
        buffer.put(NULL, static_cast<size_t>(recv_ret));
      return recv_ret;
    } else {
      vector<iovec> iov;
      Buffers::as_read_iovecs(buffer, iov);
      ssize_t recv_ret = recvmsg(&iov[0], iov.size(), flags);
      if (recv_ret > 0)
        Buffers::put(buffer, NULL, static_cast<size_t>(recv_ret));
      return recv_ret;
    }
  }

  /**
    Read from the socket into a single buffer.
    @param[in, out] buf pointer to the buffer
    @param buflen the length of the memory region pointed to by buf
    @param flags flags altering the behavior of the underlying system call
    @return the number of bytes read on success, -1+errno on failure
  */
  virtual ssize_t recv(void* buf, size_t buflen, const MessageFlags& flags);

  /**
    Read from the socket into multiple buffers (scatter I/O).
    @param[in, out] iov array of I/O vectors describing the buffers
    @param iovlen length of the I/O vectors array
    @param flags flags altering the behavior of the underlying system call
    @return the number of bytes read on success, -1+errno on failure
  */
  virtual ssize_t recvmsg(
    const iovec* iov,
    int iovlen,
    const MessageFlags& flags
  );

public:
  /**
    Write to the socket from a Buffer.
    May be a send(const void*, size_t, const MessageFlags&) or a
      sendmsg(const iovec*, int, const MessageFlags&),
      depending on whether buffer is a single buffer or a linked list of them.
    @param buffer the buffer to write from
    @param flags flags altering the behavior of the underlying system call
    @return the number of bytes written on success, -1+errno on failure
  */
  ssize_t send(const Buffer& buffer, const MessageFlags& flags) {
    if (buffer.get_next_buffer() == NULL) {
      iovec iov = buffer.as_write_iovec();
      return send(iov.iov_base, iov.iov_len, flags);
    }
    else {
      vector<iovec> iov;
      Buffers::as_write_iovecs(buffer, iov);
      return sendmsg(&iov[0], iov.size(), flags);
    }
  }

  /**
    Write to the socket from a single buffer.
    @param buf pointer to the buffer
    @param buflen the length of the memory region pointed to by buf
    @param flags flags altering the behavior of the underlying system call
    @return the number of bytes written on success, -1+errno on failure
  */
  virtual ssize_t send(const void* buf, size_t buflen, const MessageFlags& flags);

  /**
    Write to the socket from multiple buffers (gather I/O).
    @param iov array of I/O vectors describing the buffers
    @param iovlen length of the I/O vectors array
    @param flags flags altering the behavior of the underlying system call
    @return the number of bytes written on success, -1+errno on failure
  */ 
  virtual ssize_t
  sendmsg(
    const iovec* iov,
    int iovlen,
    const MessageFlags& flags
  );

public:
  /**
    Set the blocking mode of the socket, equivalent to toggling the
      O_NONBLOCK flag on POSIX systems.
    @param blocking_mode true for blocking, false for non-blocking
    @return true on success, false+errno on failure
  */
  virtual bool set_blocking_mode(bool blocking_mode);

public:
  /**
    Set a boolean socket option (e.g., SO_REUSEADDR).
    @param option_name integer constant of the option, usually cast from Option
    @param option_value new value of the option
    @return true on success, false+errno on failure
  */
  bool setsockopt(int option_name, bool option_value) {
    return setsockopt(option_name, option_value ? 1 : 0);
  }

  /**
    Set an integer socket option (e.g., SO_LINGER).
    @param option_name integer constant of the option, usually cast from Option
    @param option_value new value for the option
  */
  virtual bool setsockopt(int option_name, int option_value);

public:
  /**
    Check if the last recv call failed because it would have blocked.
    Equivalent to errno == EWOULDBLOCK on POSIX systems.
    @return true if the last recv call failed because it would have blocked
  */
  virtual bool want_recv() const;

  /**
    Check if the last send call failed because it would have blocked.
    Equivalent to errno == EWOULDBLOCK on POSIX systems.
    @return true if the last send call failed because it would have blocked
  */
  virtual bool want_send() const;

public:
  // yield::Object
  Socket& inc_ref() {
    return Object::inc_ref(*this);
  }

public:
  // yield::Channel
  virtual bool close();

  ssize_t read(Buffer& buffer) {
    return recv(buffer, 0);
  }

  ssize_t read(void* buf, size_t buflen) {
    return recv(buf, buflen, 0);
  }

  ssize_t readv(const iovec* iov, int iovlen) {
    return recvmsg(iov, iovlen, 0);
  }

  ssize_t write(const Buffer& buffer) {
    return send(buffer, 0);
  }

  ssize_t write(const void* buf, size_t buflen) {
    return send(buf, buflen, 0);
  }

  ssize_t writev(const iovec* iov, int iovlen) {
    return sendmsg(iov, iovlen, 0);
  }

protected:
  Socket(int domain, int type, int protocol, socket_t socket_)
    : domain(domain),
      type(type),
      protocol(protocol),
      socket_(socket_)
  { }

  static socket_t create(int domain, int type, int protocol);

private:
  int domain, type, protocol;
  socket_t socket_;
};

/**
  Print a string representation of a Socket to a std::ostream.
  @param os std::ostream to print to
  @param socket_ Socket to print
  @return os
*/
static std::ostream& operator<<(std::ostream& os, Socket& socket_);

/**
  Print a string representation of a Socket to a std::ostream,
    checking if the pointer to the Socket is NULL.
  @param os std::ostream to print to
  @param socket_ Socket to print, checked for NULL
  @return os
*/
static std::ostream& operator<<(std::ostream& os, Socket* socket_);
}
}

#endif
