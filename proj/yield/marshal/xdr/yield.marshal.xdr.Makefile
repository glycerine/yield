# SHELL = /bin/bash
UNAME := $(shell uname)

CXXFLAGS += -I../../../../include
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
ifneq ($COVERAGE,)
	CXXFLAGS += -fprofile-arcs -ftest-coverage
	LDFLAGS += -fprofile-arcs -ftest-coverage -lgcov
endif
ifneq ($(RELEASE),)
	CXXFLAGS += -O2
else
	CXXFLAGS += -g -D_DEBUG
endif


LDFLAGS += -L../../../../lib/yield
ifeq ($(UNAME), MINGW32)
	LDFLAGS += /ignore:4006 /ignore:4221
endif


ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lm -lrt -lstdc++
endif
LIBS += -lyield_marshal -lyield


DEP_FILE_PATHS := $(shell find ../../../../build/yield/marshal/xdr -name "*.d")


OBJECT_FILE_PATHS += ../../../../build/yield/marshal/xdr/xdr_decoder.o ../../../../build/yield/marshal/xdr/xdr_encoder.o ../../../../build/yield/marshal/xdr/xdr_marshaller.o ../../../../build/yield/marshal/xdr/xdr_unmarshaller.o


../../../../lib/yield/libyield_marshal_xdr.a: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(OBJECT_FILE_PATHS)

clean:
	$(RM) ../../../../lib/yield/libyield_marshal_xdr.a $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../../build/yield/marshal/xdr/xdr_decoder.o: ../../../../src/yield/marshal/xdr/xdr_decoder.cpp
	-mkdir -p ../../../../build/yield/marshal/xdr 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/marshal/xdr/xdr_decoder.o -MD $(CXXFLAGS) ../../../../src/yield/marshal/xdr/xdr_decoder.cpp

../../../../build/yield/marshal/xdr/xdr_encoder.o: ../../../../src/yield/marshal/xdr/xdr_encoder.cpp
	-mkdir -p ../../../../build/yield/marshal/xdr 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/marshal/xdr/xdr_encoder.o -MD $(CXXFLAGS) ../../../../src/yield/marshal/xdr/xdr_encoder.cpp

../../../../build/yield/marshal/xdr/xdr_marshaller.o: ../../../../src/yield/marshal/xdr/xdr_marshaller.cpp
	-mkdir -p ../../../../build/yield/marshal/xdr 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/marshal/xdr/xdr_marshaller.o -MD $(CXXFLAGS) ../../../../src/yield/marshal/xdr/xdr_marshaller.cpp

../../../../build/yield/marshal/xdr/xdr_unmarshaller.o: ../../../../src/yield/marshal/xdr/xdr_unmarshaller.cpp
	-mkdir -p ../../../../build/yield/marshal/xdr 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/marshal/xdr/xdr_unmarshaller.o -MD $(CXXFLAGS) ../../../../src/yield/marshal/xdr/xdr_unmarshaller.cpp


