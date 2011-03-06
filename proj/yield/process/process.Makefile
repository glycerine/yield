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


LDFLAGS += -L../../../lib/yield
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
	LIBS += -lrt -lstdc++ -ldl
endif
ifeq ($(UNAME), Solaris)
	LIBS += -liconv -lm -lrt -lstdc++
endif
LIBS += -lyield_fs -lyield_i18n -lyield_common


DEP_FILE_PATHS := $(shell find ../../../build/yield/process -name "*.d")


OBJECT_FILE_PATHS += ../../../build/yield/process/process.o
ifeq ($(UNAME), Darwin)
	OBJECT_FILE_PATHS += ../../../build/yield/process/darwin/process.o
	OBJECT_FILE_PATHS += ../../../build/yield/process/posix/process.o ../../../build/yield/process/posix/shared_library.o
endif
ifeq ($(UNAME), FreeBSD)
	OBJECT_FILE_PATHS += ../../../build/yield/process/posix/process.o ../../../build/yield/process/posix/shared_library.o
endif
ifeq ($(UNAME), Linux)
	OBJECT_FILE_PATHS += ../../../build/yield/process/linux/process.o
	OBJECT_FILE_PATHS += ../../../build/yield/process/posix/process.o ../../../build/yield/process/posix/shared_library.o
endif
ifeq ($(UNAME), Solaris)
	OBJECT_FILE_PATHS += ../../../build/yield/process/posix/process.o ../../../build/yield/process/posix/shared_library.o
	OBJECT_FILE_PATHS += ../../../build/yield/process/sunos/process.o
endif
ifeq ($(UNAME), MINGW32)
	OBJECT_FILE_PATHS += ../../../build/yield/process/win32/process.o ../../../build/yield/process/win32/shared_library.o
endif


../../../lib/yield/libyield_process.a: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(OBJECT_FILE_PATHS)

clean:
	$(RM) ../../../lib/yield/libyield_process.a $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../build/yield/process/darwin/process.o: ../../../src/yield/process/darwin/process.cpp
	-mkdir -p ../../../build/yield/process/darwin 2>/dev/null
	$(CXX) -c -o ../../../build/yield/process/darwin/process.o -MD $(CXXFLAGS) ../../../src/yield/process/darwin/process.cpp

../../../build/yield/process/linux/process.o: ../../../src/yield/process/linux/process.cpp
	-mkdir -p ../../../build/yield/process/linux 2>/dev/null
	$(CXX) -c -o ../../../build/yield/process/linux/process.o -MD $(CXXFLAGS) ../../../src/yield/process/linux/process.cpp

../../../build/yield/process/posix/process.o: ../../../src/yield/process/posix/process.cpp
	-mkdir -p ../../../build/yield/process/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/process/posix/process.o -MD $(CXXFLAGS) ../../../src/yield/process/posix/process.cpp

../../../build/yield/process/posix/shared_library.o: ../../../src/yield/process/posix/shared_library.cpp
	-mkdir -p ../../../build/yield/process/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/process/posix/shared_library.o -MD $(CXXFLAGS) ../../../src/yield/process/posix/shared_library.cpp

../../../build/yield/process/process.o: ../../../src/yield/process/process.cpp
	-mkdir -p ../../../build/yield/process 2>/dev/null
	$(CXX) -c -o ../../../build/yield/process/process.o -MD $(CXXFLAGS) ../../../src/yield/process/process.cpp

../../../build/yield/process/sunos/process.o: ../../../src/yield/process/sunos/process.cpp
	-mkdir -p ../../../build/yield/process/sunos 2>/dev/null
	$(CXX) -c -o ../../../build/yield/process/sunos/process.o -MD $(CXXFLAGS) ../../../src/yield/process/sunos/process.cpp

../../../build/yield/process/win32/process.o: ../../../src/yield/process/win32/process.cpp
	-mkdir -p ../../../build/yield/process/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/process/win32/process.o -MD $(CXXFLAGS) ../../../src/yield/process/win32/process.cpp

../../../build/yield/process/win32/shared_library.o: ../../../src/yield/process/win32/shared_library.cpp
	-mkdir -p ../../../build/yield/process/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/process/win32/shared_library.o -MD $(CXXFLAGS) ../../../src/yield/process/win32/shared_library.cpp


