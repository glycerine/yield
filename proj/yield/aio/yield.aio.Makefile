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


D_FILE_PATHS := $(shell find ../../../build/yield/aio -name "*.d")


ifeq ($(UNAME), Darwin)
	O_FILE_PATHS += ../../../build/yield/aio/posix/aiocb.o
endif
ifeq ($(UNAME), FreeBSD)
	O_FILE_PATHS += ../../../build/yield/aio/posix/aiocb.o
endif
ifeq ($(UNAME), Linux)
	O_FILE_PATHS += ../../../build/yield/aio/posix/aiocb.o
endif
ifeq ($(UNAME), Solaris)
	O_FILE_PATHS += ../../../build/yield/aio/posix/aiocb.o
endif
ifeq ($(UNAME), MINGW32)
	O_FILE_PATHS += ../../../build/yield/aio/win32/aio_queue.o ../../../build/yield/aio/win32/aiocb.o
endif


all: ../../../lib/yield/libyield_aio.a

clean:
	$(RM) ../../../lib/yield/libyield_aio.a $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)


../../../lib/yield/libyield.a:
	$(MAKE) -C .. yield.Makefile


../../../lib/yield/libyield_aio.a: $(O_FILE_PATHS) ../../../lib/yield/libyield.a
	-mkdir -p ../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(O_FILE_PATHS)

../../../build/yield/aio/posix/aiocb.o: ../../../src/yield/aio/posix/aiocb.cpp
	-mkdir -p ../../../build/yield/aio/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/posix/aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/posix/aiocb.cpp

../../../build/yield/aio/win32/aio_queue.o: ../../../src/yield/aio/win32/aio_queue.cpp
	-mkdir -p ../../../build/yield/aio/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/win32/aio_queue.o -MD $(CXXFLAGS) ../../../src/yield/aio/win32/aio_queue.cpp

../../../build/yield/aio/win32/aiocb.o: ../../../src/yield/aio/win32/aiocb.cpp
	-mkdir -p ../../../build/yield/aio/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/win32/aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/win32/aiocb.cpp
