// yield/buffer.hpp

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

#ifndef _YIELD_BUFFER_HPP_
#define _YIELD_BUFFER_HPP_

#include "yield/object.hpp"


namespace yield {
/**
  Generic buffer class, mainly used for I/O with aligned buffers.
*/
class Buffer : public Object {
public:
  /**
    Default alignment boundary.
  */
  const static size_t ALIGNMENT_DEFAULT = sizeof(void*);

  /**
    Run-time type ID.
  */
  const static uint32_t TYPE_ID = 4078143893UL;

public:
  /**
    Construct a buffer of capacity with the default alignment.
    @param capacity capacity of the new buffer
  */
  Buffer(size_t capacity);

  /**
    Construct a buffer of capacity with the specified alignment.
    @param alignment alignment boundary of the new buffer
    @param capacity capacity of the new buffer
  */
  Buffer(size_t alignment, size_t capacity);

  /**
    Free the buffer.
  */
  virtual ~Buffer();

public:
  /**
    Return an iovec suitable for reading data into the buffer.
    <code>iovec.iov_base = data() + size(),
      iovec.iov_len = capacity() - size()</code>.
    @return an iovec for reading data into the buffer
  */
  iovec as_read_iovec();

  /**
    Return an iovec suitable for writing the data from the buffer.
    <code>iovec.iov_base = data(), iovec.iov_len = size()</code>
    @return an iovec for writing data from the buffer
  */
  iovec as_write_iovec() const;

public:
  /**
    Return the current capacity of the buffer.
    @return the current capacity of the buffer
  */
  size_t capacity() const {
    return capacity_;
  }

public:
  /**
    Copy the contents of the buffer (data(), size()) into a new Buffer.
    @return the new <code>Buffer</code> copy
  */
  Buffer& copy() const {
    return copy(*this);
  }

  /**
    Copy the contents of the buffer (data.data(), data.size())
      into a new Buffer.
    @param data the data to copy
    @return the new <code>Buffer</code> copy
  */
  static Buffer& copy(const Buffer& data) {
    return copy(data, data.capacity());
  }

  /**
    Copy the contents of the buffer (data.data(), data.size())
      into a new Buffer.
    @param data the data to copy
    @return the new <code>Buffer</code> copy
  */
  static Buffer& copy(const string& data) {
    return copy(data.data(), data.size());
  }

  /**
    Copy the contents of the buffer (data, strlen(data)) into a new Buffer.
    @param data the data to copy
    @return the new <code>Buffer</code> copy
  */
  static Buffer& copy(const char* data) {
    return copy(data, strlen(data));
  }

  /**
    Copy the contents of the buffer (data, size) into a new Buffer.
    @param data the data to copy
    @return the new <code>Buffer</code> copy
  */
  static Buffer& copy(const void* data, size_t size) {
    return copy(size, data, size);
  }

  /**
    Copy the contents of the buffer (data, size) into a new Buffer,
      possibly with a larger capacity than size.
    @param capacity capacity of the new <code>Buffer</code> copy
    @param data the data to copy
    @return the new <code>Buffer</code> copy
  */
  static Buffer& copy(size_t capacity, const void* data, size_t size) {
    return copy(ALIGNMENT_DEFAULT, capacity, data, size);
  }

  /**
    Copy the contents of the buffer (data, size) into a new Buffer,
      possibly with a larger capacity than size and a different alignment.
    @param alignment alignment of the new <code>Buffer</code> copy
    @param capacity capacity of the new <code>Buffer</code> copy
    @param data the data to copy
    @return the new <code>Buffer</code> copy
  */
  static Buffer&
  copy(
    size_t alignment,
    size_t capacity,
    const void* data,
    size_t size
  );

public:
  /**
    Return a void pointer to the data.
    @return a void pointer to the data.
  */
  void* data() {
    return data_;
  }

  /**
    Return a const void pointer to the data.
    @return a const void pointer to the data.
  */
  const void* data() const {
    return data_;
  }

public:
  /**
    Check if the buffer is empty (size == 0).
    @return true if the buffer is empty.
  */
  bool empty() const {
    return size() == 0;
  }

public:
  /**
    Get the next Buffer in a linked list of <code>Buffer</code>s.
    @return the next Buffer in a linked list of <code>Buffer</code>s
  */
  Buffer* get_next_buffer() const {
    return next_buffer;
  }

public:
  /**
    Get the system page size in bytes (usually a multiple of 4096).
    @return the system page size in bytes
  */
  static size_t getpagesize();

  /**
    Check if the current buffer is system page size-aligned.
    @return true if the current buffer is system page size-aligned
  */
  bool is_page_aligned() const {
    return is_page_aligned(data());
  }

  /**
    Check if the buffer described by iov is system page size-aligned.
    @param iov iovec describing a buffer
    @return true if the buffer described by iov is system page size-aligned
  */
  static bool is_page_aligned(const iovec& iov) {
    return is_page_aligned(iov.iov_base);
  }

  /**
    Check if the buffer pointed to by data is system page size-aligned.
    @param data pointer to a buffer
    @return true if data is system page size-aligned
  */
  static bool is_page_aligned(const void* data);

public:
  /**
    Return a char pointer to the data.
    @return a char pointer to the data.
  */
  operator char* () {
    return static_cast<char*>(data());
  }

  /**
    Return a const char pointer to the data.
    @return a const char pointer to the data.
  */
  operator const char* () const {
    return static_cast<const char*>(data());
  }

  /**
    Return a uint8_t pointer to the data.
    @return a uint8_t pointer to the data.
  */
  operator uint8_t* () {
    return static_cast<uint8_t*>(data());
  }

  /**
    Return a const uint8_t pointer to the data.
    @return a const uint8_t pointer to the data.
  */
  operator const uint8_t* () const {
    return static_cast<const uint8_t*>(data());
  }

  /**
    Return a void pointer to the data.
    @return a void pointer to the data.
  */
  operator void* () {
    return data();
  }

  /**
    Return a const void pointer to the data.
    @return a const void pointer to the data.
  */
  operator const void* () const {
    return data();
  }

  /**
    Return an iovec suitable for writing the data from the buffer.
    <code>iovec.iov_base = data(), iovec.iov_len = size()</code>
    @return an iovec for writing data from the buffer
  */
  operator iovec() const {
    return as_write_iovec();
  }

  /**
    Return an individual byte from the buffer as a char.
    @param n index of the byte to return
    @return byte at the specified index
  */
  char operator[](int n) {
    return static_cast<char*>(*this)[n];
  }

  /**
    Compare this buffer to another buffer.
    Returns true if if size() == other.size() and
      memcmp(data(), other.data(), size()) == 0.
    @param other the other buffer.
    @return true if the buffers are the same
  */
  bool operator==(const Buffer& other) const;

  /**
    Compare this buffer to another buffer.
    Returns true if if size() == strlen(other) and
      memcmp(data(), other, size()) == 0.
    @param other the other buffer.
    @return true if the buffers are the same
  */
  bool operator==(const char* other) const;

  /**
    Compare this buffer to another buffer.
    Returns true if if size() == other.size() and
      memcmp(data(), other.data(), size()) == 0.
    @param other the other buffer.
    @return true if the buffers are the same
  */
  bool operator==(const string& other) const;

  /**
    Contrast this buffer to another buffer.
    Returns true if if size() != other.size() or
      memcmp(data(), other.data(), size()) != 0.
    @param other the other buffer.
    @return true if the buffers are not the same
  */
  bool operator!=(const Buffer& other) const {
    return !operator==(other);
  }

  /**
    Contrast this buffer to another buffer.
    Returns true if if size() != strlen(other) or
      memcmp(data(), other, size()) != 0.
    @param other the other buffer.
    @return true if the buffers are not the same
  */
  bool operator!=(const char* other) const {
    return !operator==(other);
  }

  /**
    Contrast this buffer to another buffer.
    Returns true if if size() != other.size() or
      memcmp(data(), other.data(), size()) != 0.
    @param other the other buffer.
    @return true if the buffers are not the same
  */
  bool operator!=(const string& other) const {
    return !operator==(other);
  }

public:
  /**
    Copy data into this buffer, increasing size.
    @param data data to copy into this buffer
  */
  void put(const Buffer& data) {
    put(data, data.size());
  }

  /**
    Copy data into this buffer, increasing size.
    @param data data to copy into this buffer
  */
  void put(char data) {
    put(&data, sizeof(data));
  }

  /**
    Copy data into this buffer, increasing size.
    @param data data to copy into this buffer
  */
  void put(const char* data) {
    put(data, strlen(data));
  }

  /**
    Copy data into this buffer, increasing size.
    @param data data to copy into this buffer
  */
  void put(const string& data) {
    put(data.c_str(), data.size());
  }

  /**
    Copy data into this buffer, increasing size.
    @param data data to copy into this buffer
  */
  void put(const iovec& data) {
    put(data.iov_base, data.iov_len);
  }

  /**
    Copy data into this buffer, increasing size.
    @param data data to copy into this buffer
    @param size size of data
  */
  void put(const void* data, size_t size);

public:
  /**
    Set the next Buffer in a linked list of <code>Buffer</code>s.
    @param next_buffer the next Buffer in a linked list of <code>Buffer</code>s
  */
  void set_next_buffer(YO_NEW_REF Buffer* next_buffer);

  /**
    Set the next Buffer in a linked list of <code>Buffer</code>s.
    @param next_buffer the next Buffer in a linked list of <code>Buffer</code>s
  */
  void set_next_buffer(YO_NEW_REF Buffer& next_buffer);

public:
  /**
    Return the size of the buffer e.g., how many bytes have been <code>put</code>
      into the buffer.
    @return size of the buffer
  */
  size_t size() const {
    return size_;
  }

public:
  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }

  const char* get_type_name() const {
    return "yield::Buffer";
  }

  Buffer& inc_ref() {
    return Object::inc_ref(*this);
  }

protected:
  /**
    Construct a <code>Buffer</code> by stealing data.
    data_ must be freed in the derived class's destructor before ~Buffer is called!
    @param capacity capacity of data, assumes size = 0
    @param data to steal for the new <code>Buffer</code>
  */
  Buffer(size_t capacity, void* data);

protected:
  size_t capacity_;
  void* data_;

private:
  void alloc(size_t alignment, size_t capacity);

private:
  static size_t pagesize;
  Buffer* next_buffer;
  size_t size_;
};

std::ostream& operator<<(std::ostream&, const Buffer&);
std::ostream& operator<<(std::ostream&, const Buffer*);
}

#endif
