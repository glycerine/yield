// yield/marshal/marshaller.cpp

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


#include "yield/marshal/marshaller.hpp"


namespace yield {
namespace marshal {
void Marshaller::write(const Object& key, Buffer& value) {
  write(key, static_cast<char*>(value), value.size());
}

void Marshaller::write(const Object& key, float value) {
  write(key, static_cast<double>(value));
}

void Marshaller::write(const Object& key, int8_t value) {
  write(key, static_cast<int16_t>(value));
}

void Marshaller::write(const Object& key, int16_t value) {
  write(key, static_cast<int32_t>(value));
}

void Marshaller::write(const Object& key, int32_t value) {
  write(key, static_cast<int64_t>(value));
}

void Marshaller::write(const Object& key, const Map& value) {
  write(key, static_cast<const Object&>(value));
}

void Marshaller::write(const Object& key, const char* value) {
  write(key, value, strnlen(value, UINT16_MAX));
}

void Marshaller::write(const Object& key, const string& value) {
  write(key, const_cast<char*>(value.data()), value.size());
}

void Marshaller::write(const Object& key, uint8_t value) {
  write(key, static_cast<uint16_t>(value));
}

void Marshaller::write(const Object& key, uint16_t value) {
  write(key, static_cast<uint32_t>(value));
}

void Marshaller::write(const Object& key, uint32_t value) {
  write(key, static_cast<uint64_t>(value));
}
}
}
