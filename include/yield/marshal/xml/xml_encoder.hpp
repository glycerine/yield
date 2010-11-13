// yield/marshal/xml/xml_encoder.hpp

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


#ifndef _YIELD_MARSHAL_XML_XML_ENCODER_HPP_
#define _YIELD_MARSHAL_XML_XML_ENCODER_HPP_


#include "yield/buffer.hpp"


struct genxWriter_rec;
typedef struct genxWriter_rec* genxWriter;


namespace yield
{
  namespace marshal
  {
    namespace xml
    {
      class XMLEncoder
      {
      public:
        XMLEncoder();
        XMLEncoder( Buffer& buffer );
        ~XMLEncoder();

        void add_attribute( const char* name, bool value )
        {
          add_attribute( NULL, name, value );
        }

        void add_attribute( const char* xmlns, const char* name, bool value );

        void add_attribute( const char* name, double value )
        {
          add_attribute( NULL, name, value );
        }

        void
        add_attribute
        (
          const char* xmlns,
          const char* name,
          double value
        );

        void add_attribute( const char* name, int64_t value )
        {
          add_attribute( NULL, name, value );
        }

        void
        add_attribute
        (
          const char* xmlns,
          const char* name,
          int64_t value
        );

        void add_attribute( const char* name, const char* value )
        {
          add_attribute( NULL, name, value );
        }

        void
        add_attribute
        (
          const char* xmlns,
          const char* name,
          const char* value
        );

        void add_attribute( const char* name, const string& value )
        {
          add_attribute( NULL, name, value );
        }

        void
        add_attribute
        (
          const char* xmlns,
          const char* name,
          const string& value
        )
        {
          add_attribute( xmlns, name, value.c_str() );
        }

        void add_attribute( const char* name, uint64_t value )
        {
          add_attribute( NULL, name, value );
        }

        void
        add_attribute
        (
          const char* xmlns,
          const char* name,
          uint64_t value
        );

        void add_text( bool text );
        void add_text( double text );
        void add_text( int64_t text );
        void add_text( const char* text );
        void add_text( const string& text );
        void add_text( const char* text, size_t text_len );
        void add_text( uint64_t text );

        void end_element();

        Buffer& get_buffer();

        void start_element( const char* type ) { start_element( NULL, type ); }
        void start_element( const char* xmlns, const char* type );

      private:
        void finalize();
        void init();

      private:
        Buffer& buffer;
        genxWriter writer;
      };
    }
  }
}


#endif
