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


DEP_FILE_PATHS := $(shell find ../../../../build/yield/marshal/json -name "*.d")


OBJECT_FILE_PATHS += ../../../../build/yield/marshal/json/json_decoder.o ../../../../build/yield/marshal/json/json_encoder.o ../../../../build/yield/marshal/json/json_marshaller.o ../../../../build/yield/marshal/json/json_unmarshaller.o ../../../../build/yield/marshal/json/yajl.o


../../../../lib/yield/libyield_marshal_json.a: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(OBJECT_FILE_PATHS)

clean:
	$(RM) ../../../../lib/yield/libyield_marshal_json.a $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../../build/yield/marshal/json/json_decoder.o: ../../../../src/yield/marshal/json/json_decoder.cpp
	-mkdir -p ../../../../build/yield/marshal/json 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/marshal/json/json_decoder.o -MD $(CXXFLAGS) ../../../../src/yield/marshal/json/json_decoder.cpp

../../../../build/yield/marshal/json/json_encoder.o: ../../../../src/yield/marshal/json/json_encoder.cpp
	-mkdir -p ../../../../build/yield/marshal/json 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/marshal/json/json_encoder.o -MD $(CXXFLAGS) ../../../../src/yield/marshal/json/json_encoder.cpp

../../../../build/yield/marshal/json/json_marshaller.o: ../../../../src/yield/marshal/json/json_marshaller.cpp
	-mkdir -p ../../../../build/yield/marshal/json 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/marshal/json/json_marshaller.o -MD $(CXXFLAGS) ../../../../src/yield/marshal/json/json_marshaller.cpp

../../../../build/yield/marshal/json/json_unmarshaller.o: ../../../../src/yield/marshal/json/json_unmarshaller.cpp
	-mkdir -p ../../../../build/yield/marshal/json 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/marshal/json/json_unmarshaller.o -MD $(CXXFLAGS) ../../../../src/yield/marshal/json/json_unmarshaller.cpp

../../../../build/yield/marshal/json/yajl.o: ../../../../src/yield/marshal/json/yajl.c
	-mkdir -p ../../../../build/yield/marshal/json 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/marshal/json/yajl.o -MD $(CXXFLAGS) ../../../../src/yield/marshal/json/yajl.c


