# SHELL = /bin/bash
UNAME := $(shell uname)

CXXFLAGS += -I../../include
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


LDFLAGS += -L../../lib/yield
ifeq ($(UNAME), MINGW32)
	LDFLAGS += /ignore:4006 /ignore:4221
endif


ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lm -lrt -lstdc++
endif


DEP_FILE_PATHS := $(shell find ../../build/yield -name "*.d")


OBJECT_FILE_PATHS += ../../build/yield/buffer.o ../../build/yield/date_time.o ../../build/yield/exception.o ../../build/yield/log.o ../../build/yield/page.o ../../build/yield/request.o ../../build/yield/string_buffer.o ../../build/yield/time.o


../../lib/yield/libyield.a: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../lib/yield 2>/dev/null
	$(AR) -r $@ $(OBJECT_FILE_PATHS)

clean:
	$(RM) ../../lib/yield/libyield.a $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../build/yield/buffer.o: ../../src/yield/buffer.cpp
	-mkdir -p ../../build/yield 2>/dev/null
	$(CXX) -c -o ../../build/yield/buffer.o -MD $(CXXFLAGS) ../../src/yield/buffer.cpp

../../build/yield/date_time.o: ../../src/yield/date_time.cpp
	-mkdir -p ../../build/yield 2>/dev/null
	$(CXX) -c -o ../../build/yield/date_time.o -MD $(CXXFLAGS) ../../src/yield/date_time.cpp

../../build/yield/exception.o: ../../src/yield/exception.cpp
	-mkdir -p ../../build/yield 2>/dev/null
	$(CXX) -c -o ../../build/yield/exception.o -MD $(CXXFLAGS) ../../src/yield/exception.cpp

../../build/yield/log.o: ../../src/yield/log.cpp
	-mkdir -p ../../build/yield 2>/dev/null
	$(CXX) -c -o ../../build/yield/log.o -MD $(CXXFLAGS) ../../src/yield/log.cpp

../../build/yield/page.o: ../../src/yield/page.cpp
	-mkdir -p ../../build/yield 2>/dev/null
	$(CXX) -c -o ../../build/yield/page.o -MD $(CXXFLAGS) ../../src/yield/page.cpp

../../build/yield/request.o: ../../src/yield/request.cpp
	-mkdir -p ../../build/yield 2>/dev/null
	$(CXX) -c -o ../../build/yield/request.o -MD $(CXXFLAGS) ../../src/yield/request.cpp

../../build/yield/string_buffer.o: ../../src/yield/string_buffer.cpp
	-mkdir -p ../../build/yield 2>/dev/null
	$(CXX) -c -o ../../build/yield/string_buffer.o -MD $(CXXFLAGS) ../../src/yield/string_buffer.cpp

../../build/yield/time.o: ../../src/yield/time.cpp
	-mkdir -p ../../build/yield 2>/dev/null
	$(CXX) -c -o ../../build/yield/time.o -MD $(CXXFLAGS) ../../src/yield/time.cpp


