# SHELL = /bin/bash
UNAME := $(shell uname)

CXXFLAGS += -I../../../include
ifeq ($(UNAME), Linux)
	CXXFLAGS += -fno-rtti -Wall -Wold-style-cast -Wunused-macros
endif
ifeq ($(UNAME), FreeBSD)
	CXXFLAGS += -fno-rtti -Wall -Wold-style-cast -Wunused-macros
endif
ifeq ($(UNAME), Darwin)
	CXXFLAGS += -fno-rtti -Wall -Wold-style-cast -Wunused-macros
endif
ifeq ($(UNAME), Solaris)
	CXXFLAGS += -fno-rtti -Wall -Wold-style-cast -Wunused-macros
endif
ifeq ($(UNAME), MINGW32)
	CXXFLAGS += /EHsc /GR- /nologo /wd"4100" /wd"4127" /wd"4290" /wd"4355" /wd"4512"
endif


LDFLAGS += -L../../../lib
ifeq ($(UNAME), MINGW32)
	LDFLAGS += /ignore:4006 /ignore:4221
endif


ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lm -lrt -lstdc++
endif
LIBS += -lyield_common


DEP_FILE_PATHS := $(shell find ../../../build/yield/marshal -name "*.d")


OBJECT_FILE_PATHS += ../../../build/yield/marshal/boolean.o ../../../build/yield/marshal/double.o ../../../build/yield/marshal/integer.o ../../../build/yield/marshal/json/json_decoder.o ../../../build/yield/marshal/json/json_encoder.o ../../../build/yield/marshal/json/json_marshaller.o ../../../build/yield/marshal/json/json_unmarshaller.o ../../../build/yield/marshal/json/yajl.o ../../../build/yield/marshal/marshaller.o ../../../build/yield/marshal/pretty_printer.o ../../../build/yield/marshal/string.o ../../../build/yield/marshal/string_literal.o ../../../build/yield/marshal/unmarshaller.o ../../../build/yield/marshal/xdr/xdr_decoder.o ../../../build/yield/marshal/xdr/xdr_encoder.o ../../../build/yield/marshal/xdr/xdr_marshaller.o ../../../build/yield/marshal/xdr/xdr_unmarshaller.o ../../../build/yield/marshal/xml/charProps.o ../../../build/yield/marshal/xml/genx.o ../../../build/yield/marshal/xml/xml_encoder.o ../../../build/yield/marshal/xml/xml_marshaller.o ../../../build/yield/marshal/xml/xml_unmarshaller.o


../../../lib/libyield_marshal.a: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../lib 2>/dev/null
	$(AR) -r $@ $(OBJECT_FILE_PATHS)

clean:
	$(RM) ../../../lib/libyield_marshal.a $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../build/yield/marshal/boolean.o: ../../../src/yield/marshal/boolean.cpp
	-mkdir -p ../../../build/yield/marshal 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/boolean.o -MD $(CXXFLAGS) ../../../src/yield/marshal/boolean.cpp

../../../build/yield/marshal/double.o: ../../../src/yield/marshal/double.cpp
	-mkdir -p ../../../build/yield/marshal 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/double.o -MD $(CXXFLAGS) ../../../src/yield/marshal/double.cpp

../../../build/yield/marshal/integer.o: ../../../src/yield/marshal/integer.cpp
	-mkdir -p ../../../build/yield/marshal 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/integer.o -MD $(CXXFLAGS) ../../../src/yield/marshal/integer.cpp

../../../build/yield/marshal/json/json_decoder.o: ../../../src/yield/marshal/json/json_decoder.cpp
	-mkdir -p ../../../build/yield/marshal/json 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/json/json_decoder.o -MD $(CXXFLAGS) ../../../src/yield/marshal/json/json_decoder.cpp

../../../build/yield/marshal/json/json_encoder.o: ../../../src/yield/marshal/json/json_encoder.cpp
	-mkdir -p ../../../build/yield/marshal/json 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/json/json_encoder.o -MD $(CXXFLAGS) ../../../src/yield/marshal/json/json_encoder.cpp

../../../build/yield/marshal/json/json_marshaller.o: ../../../src/yield/marshal/json/json_marshaller.cpp
	-mkdir -p ../../../build/yield/marshal/json 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/json/json_marshaller.o -MD $(CXXFLAGS) ../../../src/yield/marshal/json/json_marshaller.cpp

../../../build/yield/marshal/json/json_unmarshaller.o: ../../../src/yield/marshal/json/json_unmarshaller.cpp
	-mkdir -p ../../../build/yield/marshal/json 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/json/json_unmarshaller.o -MD $(CXXFLAGS) ../../../src/yield/marshal/json/json_unmarshaller.cpp

../../../build/yield/marshal/json/yajl.o: ../../../src/yield/marshal/json/yajl.c
	-mkdir -p ../../../build/yield/marshal/json 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/json/yajl.o -MD $(CXXFLAGS) ../../../src/yield/marshal/json/yajl.c

../../../build/yield/marshal/marshaller.o: ../../../src/yield/marshal/marshaller.cpp
	-mkdir -p ../../../build/yield/marshal 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/marshaller.o -MD $(CXXFLAGS) ../../../src/yield/marshal/marshaller.cpp

../../../build/yield/marshal/pretty_printer.o: ../../../src/yield/marshal/pretty_printer.cpp
	-mkdir -p ../../../build/yield/marshal 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/pretty_printer.o -MD $(CXXFLAGS) ../../../src/yield/marshal/pretty_printer.cpp

../../../build/yield/marshal/string.o: ../../../src/yield/marshal/string.cpp
	-mkdir -p ../../../build/yield/marshal 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/string.o -MD $(CXXFLAGS) ../../../src/yield/marshal/string.cpp

../../../build/yield/marshal/string_literal.o: ../../../src/yield/marshal/string_literal.cpp
	-mkdir -p ../../../build/yield/marshal 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/string_literal.o -MD $(CXXFLAGS) ../../../src/yield/marshal/string_literal.cpp

../../../build/yield/marshal/unmarshaller.o: ../../../src/yield/marshal/unmarshaller.cpp
	-mkdir -p ../../../build/yield/marshal 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/unmarshaller.o -MD $(CXXFLAGS) ../../../src/yield/marshal/unmarshaller.cpp

../../../build/yield/marshal/xdr/xdr_decoder.o: ../../../src/yield/marshal/xdr/xdr_decoder.cpp
	-mkdir -p ../../../build/yield/marshal/xdr 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/xdr/xdr_decoder.o -MD $(CXXFLAGS) ../../../src/yield/marshal/xdr/xdr_decoder.cpp

../../../build/yield/marshal/xdr/xdr_encoder.o: ../../../src/yield/marshal/xdr/xdr_encoder.cpp
	-mkdir -p ../../../build/yield/marshal/xdr 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/xdr/xdr_encoder.o -MD $(CXXFLAGS) ../../../src/yield/marshal/xdr/xdr_encoder.cpp

../../../build/yield/marshal/xdr/xdr_marshaller.o: ../../../src/yield/marshal/xdr/xdr_marshaller.cpp
	-mkdir -p ../../../build/yield/marshal/xdr 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/xdr/xdr_marshaller.o -MD $(CXXFLAGS) ../../../src/yield/marshal/xdr/xdr_marshaller.cpp

../../../build/yield/marshal/xdr/xdr_unmarshaller.o: ../../../src/yield/marshal/xdr/xdr_unmarshaller.cpp
	-mkdir -p ../../../build/yield/marshal/xdr 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/xdr/xdr_unmarshaller.o -MD $(CXXFLAGS) ../../../src/yield/marshal/xdr/xdr_unmarshaller.cpp

../../../build/yield/marshal/xml/charProps.o: ../../../src/yield/marshal/xml/charProps.c
	-mkdir -p ../../../build/yield/marshal/xml 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/xml/charProps.o -MD $(CXXFLAGS) ../../../src/yield/marshal/xml/charProps.c

../../../build/yield/marshal/xml/genx.o: ../../../src/yield/marshal/xml/genx.c
	-mkdir -p ../../../build/yield/marshal/xml 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/xml/genx.o -MD $(CXXFLAGS) ../../../src/yield/marshal/xml/genx.c

../../../build/yield/marshal/xml/xml_encoder.o: ../../../src/yield/marshal/xml/xml_encoder.cpp
	-mkdir -p ../../../build/yield/marshal/xml 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/xml/xml_encoder.o -MD $(CXXFLAGS) ../../../src/yield/marshal/xml/xml_encoder.cpp

../../../build/yield/marshal/xml/xml_marshaller.o: ../../../src/yield/marshal/xml/xml_marshaller.cpp
	-mkdir -p ../../../build/yield/marshal/xml 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/xml/xml_marshaller.o -MD $(CXXFLAGS) ../../../src/yield/marshal/xml/xml_marshaller.cpp

../../../build/yield/marshal/xml/xml_unmarshaller.o: ../../../src/yield/marshal/xml/xml_unmarshaller.cpp
	-mkdir -p ../../../build/yield/marshal/xml 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/xml/xml_unmarshaller.o -MD $(CXXFLAGS) ../../../src/yield/marshal/xml/xml_unmarshaller.cpp


