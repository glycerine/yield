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


ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lm -lrt -lstdc++
endif


DEP_FILE_PATHS := $(shell find ../../../build/yield/common -name "*.d")


OBJECT_FILE_PATHS += ../../../build/yield/common/buffer.o ../../../build/yield/common/date_time.o ../../../build/yield/common/exception.o ../../../build/yield/common/log.o ../../../build/yield/common/page.o ../../../build/yield/common/request.o ../../../build/yield/common/string_buffer.o ../../../build/yield/common/time.o


../../../lib/libyield_common.a: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../lib 2>/dev/null
	$(AR) -r $@ $(OBJECT_FILE_PATHS)

clean:
	$(RM) ../../../lib/libyield_common.a $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../build/yield/common/buffer.o: ../../../src/yield/buffer.cpp
	-mkdir -p ../../../build/yield/common 2>/dev/null
	$(CXX) -c -o ../../../build/yield/common/buffer.o -MD $(CXXFLAGS) ../../../src/yield/buffer.cpp

../../../build/yield/common/date_time.o: ../../../src/yield/date_time.cpp
	-mkdir -p ../../../build/yield/common 2>/dev/null
	$(CXX) -c -o ../../../build/yield/common/date_time.o -MD $(CXXFLAGS) ../../../src/yield/date_time.cpp

../../../build/yield/common/exception.o: ../../../src/yield/exception.cpp
	-mkdir -p ../../../build/yield/common 2>/dev/null
	$(CXX) -c -o ../../../build/yield/common/exception.o -MD $(CXXFLAGS) ../../../src/yield/exception.cpp

../../../build/yield/common/log.o: ../../../src/yield/log.cpp
	-mkdir -p ../../../build/yield/common 2>/dev/null
	$(CXX) -c -o ../../../build/yield/common/log.o -MD $(CXXFLAGS) ../../../src/yield/log.cpp

../../../build/yield/common/page.o: ../../../src/yield/page.cpp
	-mkdir -p ../../../build/yield/common 2>/dev/null
	$(CXX) -c -o ../../../build/yield/common/page.o -MD $(CXXFLAGS) ../../../src/yield/page.cpp

../../../build/yield/common/request.o: ../../../src/yield/request.cpp
	-mkdir -p ../../../build/yield/common 2>/dev/null
	$(CXX) -c -o ../../../build/yield/common/request.o -MD $(CXXFLAGS) ../../../src/yield/request.cpp

../../../build/yield/common/string_buffer.o: ../../../src/yield/string_buffer.cpp
	-mkdir -p ../../../build/yield/common 2>/dev/null
	$(CXX) -c -o ../../../build/yield/common/string_buffer.o -MD $(CXXFLAGS) ../../../src/yield/string_buffer.cpp

../../../build/yield/common/time.o: ../../../src/yield/time.cpp
	-mkdir -p ../../../build/yield/common 2>/dev/null
	$(CXX) -c -o ../../../build/yield/common/time.o -MD $(CXXFLAGS) ../../../src/yield/time.cpp


