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


OBJECT_FILE_PATHS += ../../../build/yield/process/option_parser.o ../../../build/yield/process/process.o
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
ifeq ($(UNAME), MINGW32)
	OBJECT_FILE_PATHS += ../../../build/yield/process/win32/process.o ../../../build/yield/process/win32/shared_library.o
endif
ifeq ($(UNAME), Solaris)
	OBJECT_FILE_PATHS += ../../../build/yield/process/posix/process.o ../../../build/yield/process/posix/shared_library.o
	OBJECT_FILE_PATHS += ../../../build/yield/process/sunos/process.o
endif


../../../lib/libyield_process.a: $(OBJECT_FILE_PATHS)
	$(AR) -r $@ $(OBJECT_FILE_PATHS)

clean:
	$(RM) ../../../lib/libyield_process.a $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../build/yield/process/darwin/process.o: ../../../src/yield/process/darwin/process.cpp
	$(CXX) -c -o ../../../build/yield/process/darwin/process.o -MD $(CXXFLAGS) ../../../src/yield/process/darwin/process.cpp

../../../build/yield/process/linux/process.o: ../../../src/yield/process/linux/process.cpp
	$(CXX) -c -o ../../../build/yield/process/linux/process.o -MD $(CXXFLAGS) ../../../src/yield/process/linux/process.cpp

../../../build/yield/process/option_parser.o: ../../../src/yield/process/option_parser.cpp
	$(CXX) -c -o ../../../build/yield/process/option_parser.o -MD $(CXXFLAGS) ../../../src/yield/process/option_parser.cpp

../../../build/yield/process/posix/process.o: ../../../src/yield/process/posix/process.cpp
	$(CXX) -c -o ../../../build/yield/process/posix/process.o -MD $(CXXFLAGS) ../../../src/yield/process/posix/process.cpp

../../../build/yield/process/posix/shared_library.o: ../../../src/yield/process/posix/shared_library.cpp
	$(CXX) -c -o ../../../build/yield/process/posix/shared_library.o -MD $(CXXFLAGS) ../../../src/yield/process/posix/shared_library.cpp

../../../build/yield/process/process.o: ../../../src/yield/process/process.cpp
	$(CXX) -c -o ../../../build/yield/process/process.o -MD $(CXXFLAGS) ../../../src/yield/process/process.cpp

../../../build/yield/process/sunos/process.o: ../../../src/yield/process/sunos/process.cpp
	$(CXX) -c -o ../../../build/yield/process/sunos/process.o -MD $(CXXFLAGS) ../../../src/yield/process/sunos/process.cpp

../../../build/yield/process/win32/process.o: ../../../src/yield/process/win32/process.cpp
	$(CXX) -c -o ../../../build/yield/process/win32/process.o -MD $(CXXFLAGS) ../../../src/yield/process/win32/process.cpp

../../../build/yield/process/win32/shared_library.o: ../../../src/yield/process/win32/shared_library.cpp
	$(CXX) -c -o ../../../build/yield/process/win32/shared_library.o -MD $(CXXFLAGS) ../../../src/yield/process/win32/shared_library.cpp


