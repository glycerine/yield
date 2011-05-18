TIMESTAMP=$(shell date +%Y%m%dT%H%M%S)
UNAME := $(shell uname)


CXXFLAGS += -I../../include
ifeq ($(UNAME), FreeBSD)
	CXXFLAGS += -I/usr/local/include
endif
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
else ifneq ($(RELEASE),)
	CXXFLAGS += -O2
else
	CXXFLAGS += -g -D_DEBUG
endif



LDFLAGS += -L../../lib/yield
ifeq ($(UNAME), FreeBSD)
	LDFLAGS += -L/usr/local/lib
endif
ifeq ($(UNAME), MINGW32)
	LDFLAGS += /ignore:4006 /ignore:4221
endif


ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lm -lrt -lstdc++
endif


D_FILE_PATHS := $(shell find ../../build/yield -name "*.d")


O_FILE_PATHS += ../../build/yield/buffer.o ../../build/yield/buffers.o ../../build/yield/channel.o ../../build/yield/date_time.o ../../build/yield/exception.o ../../build/yield/log.o ../../build/yield/time.o


all: ../../lib/yield/libyield.a

clean:
	$(RM) ../../lib/yield/libyield.a $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)

../../lib/yield/libyield.a: $(O_FILE_PATHS)
	-mkdir -p ../../lib/yield 2>/dev/null
	$(AR) -r $@ $(O_FILE_PATHS)

../../build/yield/buffer.o: ../../src/yield/buffer.cpp
	-mkdir -p ../../build/yield 2>/dev/null
	$(CXX) -c -o ../../build/yield/buffer.o -MD $(CXXFLAGS) ../../src/yield/buffer.cpp

../../build/yield/buffers.o: ../../src/yield/buffers.cpp
	-mkdir -p ../../build/yield 2>/dev/null
	$(CXX) -c -o ../../build/yield/buffers.o -MD $(CXXFLAGS) ../../src/yield/buffers.cpp

../../build/yield/channel.o: ../../src/yield/channel.cpp
	-mkdir -p ../../build/yield 2>/dev/null
	$(CXX) -c -o ../../build/yield/channel.o -MD $(CXXFLAGS) ../../src/yield/channel.cpp

../../build/yield/date_time.o: ../../src/yield/date_time.cpp
	-mkdir -p ../../build/yield 2>/dev/null
	$(CXX) -c -o ../../build/yield/date_time.o -MD $(CXXFLAGS) ../../src/yield/date_time.cpp

../../build/yield/exception.o: ../../src/yield/exception.cpp
	-mkdir -p ../../build/yield 2>/dev/null
	$(CXX) -c -o ../../build/yield/exception.o -MD $(CXXFLAGS) ../../src/yield/exception.cpp

../../build/yield/log.o: ../../src/yield/log.cpp
	-mkdir -p ../../build/yield 2>/dev/null
	$(CXX) -c -o ../../build/yield/log.o -MD $(CXXFLAGS) ../../src/yield/log.cpp

../../build/yield/time.o: ../../src/yield/time.cpp
	-mkdir -p ../../build/yield 2>/dev/null
	$(CXX) -c -o ../../build/yield/time.o -MD $(CXXFLAGS) ../../src/yield/time.cpp
