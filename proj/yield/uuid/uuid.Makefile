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


ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lm -lrt -lstdc++ -luuid
endif
LIBS += -lyield_common


DEP_FILE_PATHS := $(shell find ../../../build/yield/uuid -name "*.d")


OBJECT_FILE_PATHS += ../../../build/yield/uuid/uuid.o
ifeq ($(UNAME), Linux)
	OBJECT_FILE_PATHS += ../../../build/yield/uuid/linux/uuid.o
endif
ifeq ($(UNAME), Solaris)
	OBJECT_FILE_PATHS += ../../../build/yield/uuid/sunos/uuid.o
endif
ifeq ($(UNAME), MINGW32)
	OBJECT_FILE_PATHS += ../../../build/yield/uuid/win32/uuid.o
endif


../../../lib/yield/libyield_uuid.a: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(OBJECT_FILE_PATHS)

clean:
	$(RM) ../../../lib/yield/libyield_uuid.a $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../build/yield/uuid/linux/uuid.o: ../../../src/yield/uuid/linux/uuid.cpp
	-mkdir -p ../../../build/yield/uuid/linux 2>/dev/null
	$(CXX) -c -o ../../../build/yield/uuid/linux/uuid.o -MD $(CXXFLAGS) ../../../src/yield/uuid/linux/uuid.cpp

../../../build/yield/uuid/sunos/uuid.o: ../../../src/yield/uuid/sunos/uuid.cpp
	-mkdir -p ../../../build/yield/uuid/sunos 2>/dev/null
	$(CXX) -c -o ../../../build/yield/uuid/sunos/uuid.o -MD $(CXXFLAGS) ../../../src/yield/uuid/sunos/uuid.cpp

../../../build/yield/uuid/uuid.o: ../../../src/yield/uuid/uuid.cpp
	-mkdir -p ../../../build/yield/uuid 2>/dev/null
	$(CXX) -c -o ../../../build/yield/uuid/uuid.o -MD $(CXXFLAGS) ../../../src/yield/uuid/uuid.cpp

../../../build/yield/uuid/win32/uuid.o: ../../../src/yield/uuid/win32/uuid.cpp
	-mkdir -p ../../../build/yield/uuid/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/uuid/win32/uuid.o -MD $(CXXFLAGS) ../../../src/yield/uuid/win32/uuid.cpp


