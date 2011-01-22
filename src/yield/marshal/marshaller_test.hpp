// yield/marshal/marshaller_test.hpp

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


#ifndef _YIELD_MARSHAL_MARSHALLER_TEST_HPP_
#define _YIELD_MARSHAL_MARSHALLER_TEST_HPP_


#include "marshaller_test_types.hpp"
#include "yunit.hpp"

#include <float.h>


namespace yield {
namespace marshal {
template <class ObjectType>
class MarshallerTest : public yunit::Test {
protected:
  MarshallerTest( YO_NEW_REF ObjectType* object )
    : object( object ) {
    empty_object = new ObjectType;
  }

  virtual ~MarshallerTest() {
    ObjectType::dec_ref( *object );
    ObjectType::dec_ref( *empty_object );
  }

  ObjectType* object;
  ObjectType* empty_object;
};


template <template <class> class MarshallerTestType>
class MarshallerTestSuite : public yunit::TestSuite {
public:
  MarshallerTestSuite() {
    // Boolean
    add
    (
      "MarshallerTest( BooleanStruct( true ) )",
      new MarshallerTestType<BooleanStruct>( new BooleanStruct( true ) )
    );

    add
    (
      "MarshallerTest( BooleanStruct( false ) )",
      new MarshallerTestType<BooleanStruct>( new BooleanStruct( false ) )
    );


    // Double
    //add
    //(
    //  "MarshallerTest( DBL_MIN )",
    //  new MarshallerTestType<DoubleStruct>( new DoubleStruct( DBL_MIN ) )
    //);

    add
    (
      "MarshallerTest( -1.0 )",
      new MarshallerTestType<DoubleStruct>( new DoubleStruct( -1 ) )
    );

    add
    (
      "MarshallerTest( 0.0 )",
      new MarshallerTestType<DoubleStruct>( new DoubleStruct( 0 ) )
    );

    add
    (
      "MarshallerTest( 1.0 )",
      new MarshallerTestType<DoubleStruct>( new DoubleStruct( 1 ) )
    );

    //add
    //(
    //  "MarshallerTest( DBL_MAX )",
    //  new MarshallerTestType<DoubleStruct>( new DoubleStruct( DBL_MAX ) )
    //);


    // DoubleMap
    add
    (
      "MarshallerTest( DoubleMap() )",
      new MarshallerTestType<DoubleMapStruct>( new DoubleMapStruct() )
    );

    {
      DoubleMap double_map;
      double_map[1.1] = "test";
      add
      (
        "MarshallerTest( DoubleMap( { 1.1 : \"test\" } ) )",
        new MarshallerTestType<DoubleMapStruct>
        (
          new DoubleMapStruct( double_map )
        )
      );
    }

    {
      DoubleMap double_map;
      double_map[1.1] = "test";
      double_map[2.2] = "test";
      add
      (
        "MarshallerTest( DoubleMap( { 1.1 : \"test\", 2.2 : \"test\" } ) )",
        new MarshallerTestType<DoubleMapStruct>
        (
          new DoubleMapStruct( double_map )
        )
      );
    }

    // DoubleSet
    add
    (
      "MarshallerTest( DoubleSet() )",
      new MarshallerTestType<DoubleSetStruct>( new DoubleSetStruct() )
    );

    add
    (
      "MarshallerTest( DoubleSet( [1.1] ) )",
      new MarshallerTestType<DoubleSetStruct>
      (
        new DoubleSetStruct( DoubleSet( 1.1 ) )
      )
    );

    {
      DoubleSet double_set( 1.1 );
      double_set.push_back( 2.2 );
      add
      (
        "MarshallerTest( DoubleSet( [1.1, 2.2] ) )",
        new MarshallerTestType<DoubleSetStruct>
        (
          new DoubleSetStruct( double_set )
        )
      );
    }


    // EmptyStruct
    add
    (
      "MarshallerTest( EmptyStruct() )",
      new MarshallerTestType<EmptyStruct>( new EmptyStruct() )
    );


    // Float
    //add
    //(
    //  "MarshallerTest( FLT_MIN )",
    //  new MarshallerTestType<FloatStruct>( new FloatStruct( FLT_MIN ) )
    //);

    add
    (
      "MarshallerTest( -1.0F )",
      new MarshallerTestType<FloatStruct>( new FloatStruct( -1 ) )
    );

    add
    (
      "MarshallerTest( 0.0F )",
      new MarshallerTestType<FloatStruct>( new FloatStruct( 0 ) )
    );

    add
    (
      "MarshallerTest( 1.0F )",
      new MarshallerTestType<FloatStruct>( new FloatStruct( 1 ) )
    );

    //add
    //(
    //  "MarshallerTest( FLT_MAX )",
    //  new MarshallerTestType<FloatStruct>( new FloatStruct( FLT_MAX ) )
    //);


    // Int8
    add
    (
      "MarshallerTest( INT8_MIN )",
      new MarshallerTestType<Int8Struct>( new Int8Struct( INT8_MIN ) )
    );

    add
    (
      "MarshallerTest( -1c )",
      new MarshallerTestType<Int8Struct>( new Int8Struct( -1 ) )
    );

    add
    (
      "MarshallerTest( 0c )",
      new MarshallerTestType<Int8Struct>( new Int8Struct( 0 ) )
    );

    add
    (
      "MarshallerTest( 1c )",
      new MarshallerTestType<Int8Struct>( new Int8Struct( 1 ) )
    );

    add
    (
      "MarshallerTest( INT8_MAX )",
      new MarshallerTestType<Int8Struct>( new Int8Struct( INT8_MAX ) )
    );


    // Int16
    add
    (
      "MarshallerTest( INT16_MIN )",
      new MarshallerTestType<Int16Struct>( new Int16Struct( INT16_MIN ) )
    );

    add
    (
      "MarshallerTest( -1s )",
      new MarshallerTestType<Int16Struct>( new Int16Struct( -1 ) )
    );

    add
    (
      "MarshallerTest( 0s )",
      new MarshallerTestType<Int16Struct>( new Int16Struct( 0 ) )
    );


    add
    (
      "MarshallerTest( 1s )",
      new MarshallerTestType<Int16Struct>( new Int16Struct( 1 ) )
    );

    add
    (
      "MarshallerTest( INT16_MAX )",
      new MarshallerTestType<Int16Struct>( new Int16Struct( INT16_MAX ) )
    );


    // Int32
    add
    (
      "MarshallerTest( INT32_MIN )",
      new MarshallerTestType<Int32Struct>( new Int32Struct( INT32_MIN ) )
    );

    add
    (
      "MarshallerTest( -1l )",
      new MarshallerTestType<Int32Struct>( new Int32Struct( -1 ) )
    );

    add
    (
      "MarshallerTest( 0l )",
      new MarshallerTestType<Int32Struct>( new Int32Struct( 0 ) )
    );

    add
    (
      "MarshallerTest( 1l )",
      new MarshallerTestType<Int32Struct>( new Int32Struct( 1 ) )
    );

    add
    (
      "MarshallerTest( INT32_MAX )",
      new MarshallerTestType<Int32Struct>( new Int32Struct( INT32_MAX ) )
    );


    // Int64
    add
    (
      "MarshallerTest( INT64_MIN )",
      new MarshallerTestType<Int64Struct>( new Int64Struct( INT64_MIN ) )
    );

    add
    (
      "MarshallerTest( -1ll )",
      new MarshallerTestType<Int64Struct>( new Int64Struct( -1 ) )
    );

    add
    (
      "MarshallerTest( 0ll )",
      new MarshallerTestType<Int64Struct>( new Int64Struct( 0 ) )
    );

    add
    (
      "MarshallerTest( 1ll )",
      new MarshallerTestType<Int64Struct>( new Int64Struct( 1 ) )
    );

    add
    (
      "MarshallerTest( INT64_MAX )",
      new MarshallerTestType<Int64Struct>( new Int64Struct( INT64_MAX ) )
    );


    // String
    add
    (
      "MarshallerTest( string() )",
      new MarshallerTestType<StringStruct>( new StringStruct() )
    );

    add
    (
      "MarshallerTest( \"\" )",
      new MarshallerTestType<StringStruct>( new StringStruct( "" ) )
    );

    add
    (
      "MarshallerTest( \"test\" )",
      new MarshallerTestType<StringStruct>( new StringStruct( "test" ) )
    );

    // To get padding in XDR
    add
    (
      "MarshallerTest( \"test1\" )",
      new MarshallerTestType<StringStruct>( new StringStruct( "test1" ) )
    );


    // StringMap
    add
    (
      "MarshallerTest( StringMap() )",
      new MarshallerTestType<StringMapStruct>( new StringMapStruct() )
    );

    {
      StringMap string_map;
      string_map[String( "test" )] = "test";
      add
      (
        "MarshallerTest( StringMap( { \"test\" : \"test\" } ) )",
        new MarshallerTestType<StringMapStruct>
        (
          new StringMapStruct( string_map )
        )
      );
    }

    {
      StringMap string_map;
      string_map[String( "test1" )] = "test";
      string_map[String( "test2" )] = "test";
      add
      (
        "MarshallerTest( StringMap( { \"test1\" : \"test\", \"test2\" : \"test\" } ) )",
        new MarshallerTestType<StringMapStruct>
        (
          new StringMapStruct( string_map )
        )
      );
    }


    // StringSet
    add
    (
      "MarshallerTest( StringSet() )",
      new MarshallerTestType<StringSetStruct>( new StringSetStruct() )
    );

    add
    (
      "MarshallerTest( StringSet( [\"test\"] )",
      new MarshallerTestType<StringSetStruct>
      (
        new StringSetStruct( StringSet( "test" ) )
      )
    );

    {
      StringSet string_set( "test1" );
      string_set.push_back( "test2" );
      add
      (
        "MarshallerTest( StringSet( [\"test1\", \"test2\"] )",
        new MarshallerTestType<StringSetStruct>
        (
          new StringSetStruct( string_set )
        )
      );
    }


    // Uint8
    add
    (
      "MarshallerTest( 0uc )",
      new MarshallerTestType<Uint8Struct>( new Uint8Struct( 0 ) )
    );

    add
    (
      "MarshallerTest( 1uc )",
      new MarshallerTestType<Uint8Struct>( new Uint8Struct( 1 ) )
    );

    add
    (
      "MarshallerTest( UINT8_MAXuc )",
      new MarshallerTestType<Uint8Struct>( new Uint8Struct( UINT8_MAX ) )
    );


    // Uint16
    add
    (
      "MarshallerTest( 0us )",
      new MarshallerTestType<Uint16Struct>( new Uint16Struct( 0 ) )
    );

    add
    (
      "MarshallerTest( 1us )",
      new MarshallerTestType<Uint16Struct>( new Uint16Struct( 1 ) )
    );

    add
    (
      "MarshallerTest( UINT16_MAX )",
      new MarshallerTestType<Uint16Struct>( new Uint16Struct( UINT16_MAX ) )
    );


    // Uint32
    add
    (
      "MarshallerTest( 0ul )",
      new MarshallerTestType<Uint32Struct>( new Uint32Struct( 0 ) )
    );

    add
    (
      "MarshallerTest( 1ul )",
      new MarshallerTestType<Uint32Struct>( new Uint32Struct( 1 ) )
    );

    add
    (
      "MarshallerTest( UINT32_MAX )",
      new MarshallerTestType<Uint32Struct>( new Uint32Struct( UINT32_MAX ) )
    );


    // Uint64
    add
    (
      "MarshallerTest( 0ull )",
      new MarshallerTestType<Uint64Struct>( new Uint64Struct( 0 ) )
    );

    add
    (
      "MarshallerTest( 1ull )",
      new MarshallerTestType<Uint64Struct>( new Uint64Struct( 1 ) )
    );

    add
    (
      "MarshallerTest( UINT64_MAX )",
      new MarshallerTestType<Uint64Struct>( new Uint64Struct( UINT64_MAX ) )
    );
  }
};
};
};


#endif
