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
ifneq ($COVERAGE,)
	CXXFLAGS += -fprofile-arcs -ftest-coverage
	LDFLAGS += -fprofile-arcs -ftest-coverage -lgcov
endif
ifneq ($(RELEASE),)
	CXXFLAGS += -O2
else
	CXXFLAGS += -g -D_DEBUG
endif


LDFLAGS += -L../../../lib
ifeq ($(UNAME), MINGW32)
	LDFLAGS += /ignore:4006 /ignore:4221
endif


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
	LIBS += -lm -lrt -lstdc++ -liconv
endif
LIBS += -lyield_common


DEP_FILE_PATHS := $(shell find ../../../build/yield/i18n -name "*.d")


ifeq ($(UNAME), Darwin)
	OBJECT_FILE_PATHS += ../../../build/yield/i18n/posix/code.o ../../../build/yield/i18n/posix/iconv.o ../../../build/yield/i18n/posix/tstring.o
endif
ifeq ($(UNAME), FreeBSD)
	OBJECT_FILE_PATHS += ../../../build/yield/i18n/posix/code.o ../../../build/yield/i18n/posix/iconv.o ../../../build/yield/i18n/posix/tstring.o
endif
ifeq ($(UNAME), Linux)
	OBJECT_FILE_PATHS += ../../../build/yield/i18n/posix/code.o ../../../build/yield/i18n/posix/iconv.o ../../../build/yield/i18n/posix/tstring.o
endif
ifeq ($(UNAME), Solaris)
	OBJECT_FILE_PATHS += ../../../build/yield/i18n/posix/code.o ../../../build/yield/i18n/posix/iconv.o ../../../build/yield/i18n/posix/tstring.o
endif
ifeq ($(UNAME), MINGW32)
	OBJECT_FILE_PATHS += ../../../build/yield/i18n/win32/code.o ../../../build/yield/i18n/win32/iconv.o ../../../build/yield/i18n/win32/tstring.o
endif


../../../lib/libyield_i18n.a: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../lib 2>/dev/null
	$(AR) -r $@ $(OBJECT_FILE_PATHS)

clean:
	$(RM) ../../../lib/libyield_i18n.a $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


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


