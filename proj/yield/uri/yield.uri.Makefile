TIMESTAMP=$(shell date +%Y%m%dT%H%M%S)
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
	CXXFLAGS += /EHsc /GR- /MP /nologo /wd"4100" /wd"4127" /wd"4290" /wd"4355" /wd"4512"
endif
ifneq ($(COVERAGE),)
	CXXFLAGS += -fprofile-arcs -ftest-coverage
	LDFLAGS += -fprofile-arcs -ftest-coverage -lgcov
endif
ifneq ($(RELEASE),)
	CXXFLAGS += -O2
else
	CXXFLAGS += -g -D_DEBUG
endif


LDFLAGS += -L../../../lib/yield
ifeq ($(UNAME), MINGW32)
	LDFLAGS += /ignore:4006 /ignore:4221
endif


ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lm -lrt -lstdc++
endif
LIBS += -lyield


D_FILE_PATHS := $(shell find ../../../build/yield/uri -name "*.d")


O_FILE_PATHS += ../../../build/yield/uri/uri.o ../../../build/yield/uri/uri_parser.o


all: ../../../lib/yield/libyield_uri.a

clean:
	$(RM) ../../../lib/yield/libyield_uri.a $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)


../../../lib/yield/libyield_uri.a: $(O_FILE_PATHS)
	-mkdir -p ../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(O_FILE_PATHS)

../../../build/yield/uri/uri.o: ../../../src/yield/uri/uri.cpp
	-mkdir -p ../../../build/yield/uri 2>/dev/null
	$(CXX) -c -o ../../../build/yield/uri/uri.o -MD $(CXXFLAGS) ../../../src/yield/uri/uri.cpp

../../../build/yield/uri/uri_parser.o: ../../../src/yield/uri/uri_parser.cpp
	-mkdir -p ../../../build/yield/uri 2>/dev/null
	$(CXX) -c -o ../../../build/yield/uri/uri_parser.o -MD $(CXXFLAGS) ../../../src/yield/uri/uri_parser.cpp
