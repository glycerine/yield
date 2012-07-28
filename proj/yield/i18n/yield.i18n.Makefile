TIMESTAMP=$(shell date +%Y%m%dT%H%M%S)
UNAME := $(shell uname)


CXXFLAGS += -I../../../include
ifeq ($(UNAME), FreeBSD)
	CXXFLAGS += -I/usr/local/include
endif
ifeq ($(UNAME), FreeBSD)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wold-style-cast -Wunused
endif
ifeq ($(UNAME), Linux)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wold-style-cast -Wunused
endif
ifeq ($(UNAME), Darwin)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wold-style-cast -Wunused
endif
ifeq ($(UNAME), Solaris)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wold-style-cast -Wunused
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



LDFLAGS += -L../../../lib/yield
ifeq ($(UNAME), FreeBSD)
	LDFLAGS += -L/usr/local/lib
endif
ifeq ($(UNAME), MINGW32)
	LDFLAGS += /ignore:4006 /ignore:4221
endif


LIBS += -lyield
ifeq ($(UNAME), Darwin)
	LIBS += -liconv
endif
ifeq ($(UNAME), FreeBSD)
	LIBS += -liconv -lintl
endif
ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -liconv -lm -lrt -lstdc++
endif


D_FILE_PATHS := $(shell find ../../../build/yield/i18n -name "*.d")


ifeq ($(UNAME), Darwin)
	O_FILE_PATHS += ../../../build/yield/i18n/posix/code.o ../../../build/yield/i18n/posix/iconv.o ../../../build/yield/i18n/posix/tstring.o
endif
ifeq ($(UNAME), FreeBSD)
	O_FILE_PATHS += ../../../build/yield/i18n/posix/code.o ../../../build/yield/i18n/posix/iconv.o ../../../build/yield/i18n/posix/tstring.o
endif
ifeq ($(UNAME), Linux)
	O_FILE_PATHS += ../../../build/yield/i18n/posix/code.o ../../../build/yield/i18n/posix/iconv.o ../../../build/yield/i18n/posix/tstring.o
endif
ifeq ($(UNAME), Solaris)
	O_FILE_PATHS += ../../../build/yield/i18n/posix/code.o ../../../build/yield/i18n/posix/iconv.o ../../../build/yield/i18n/posix/tstring.o
endif
ifeq ($(UNAME), MINGW32)
	O_FILE_PATHS += ../../../build/yield/i18n/win32/code.o ../../../build/yield/i18n/win32/iconv.o ../../../build/yield/i18n/win32/tstring.o
endif


all: ../../../lib/yield/libyield_i18n.a

clean:
	$(RM) ../../../lib/yield/libyield_i18n.a $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)


../../../lib/yield/libyield.a:
	$(MAKE) -C .. yield.Makefile


../../../lib/yield/libyield_i18n.a: $(O_FILE_PATHS) ../../../lib/yield/libyield.a
	-mkdir -p ../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(O_FILE_PATHS)

../../../build/yield/i18n/posix/code.o: ../../../src/yield/i18n/posix/code.cpp
	-mkdir -p ../../../build/yield/i18n/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/i18n/posix/code.o -MD $(CXXFLAGS) ../../../src/yield/i18n/posix/code.cpp

../../../build/yield/i18n/posix/iconv.o: ../../../src/yield/i18n/posix/iconv.cpp
	-mkdir -p ../../../build/yield/i18n/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/i18n/posix/iconv.o -MD $(CXXFLAGS) ../../../src/yield/i18n/posix/iconv.cpp

../../../build/yield/i18n/posix/tstring.o: ../../../src/yield/i18n/posix/tstring.cpp
	-mkdir -p ../../../build/yield/i18n/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/i18n/posix/tstring.o -MD $(CXXFLAGS) ../../../src/yield/i18n/posix/tstring.cpp

../../../build/yield/i18n/win32/code.o: ../../../src/yield/i18n/win32/code.cpp
	-mkdir -p ../../../build/yield/i18n/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/i18n/win32/code.o -MD $(CXXFLAGS) ../../../src/yield/i18n/win32/code.cpp

../../../build/yield/i18n/win32/iconv.o: ../../../src/yield/i18n/win32/iconv.cpp
	-mkdir -p ../../../build/yield/i18n/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/i18n/win32/iconv.o -MD $(CXXFLAGS) ../../../src/yield/i18n/win32/iconv.cpp

../../../build/yield/i18n/win32/tstring.o: ../../../src/yield/i18n/win32/tstring.cpp
	-mkdir -p ../../../build/yield/i18n/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/i18n/win32/tstring.o -MD $(CXXFLAGS) ../../../src/yield/i18n/win32/tstring.cpp
