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
	LIBS += -lm -lrt -lstdc++
endif
LIBS += -lyield


DEP_FILE_PATHS := $(shell find ../../../build/yield/getopt -name "*.d")


OBJECT_FILE_PATHS += ../../../build/yield/getopt/option.o ../../../build/yield/getopt/option_parser.o ../../../build/yield/getopt/options.o ../../../build/yield/getopt/parsed_option.o


../../../lib/yield/libyield_getopt.a: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(OBJECT_FILE_PATHS)

clean:
	$(RM) ../../../lib/yield/libyield_getopt.a $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../build/yield/getopt/option.o: ../../../src/yield/getopt/option.cpp
	-mkdir -p ../../../build/yield/getopt 2>/dev/null
	$(CXX) -c -o ../../../build/yield/getopt/option.o -MD $(CXXFLAGS) ../../../src/yield/getopt/option.cpp

../../../build/yield/getopt/option_parser.o: ../../../src/yield/getopt/option_parser.cpp
	-mkdir -p ../../../build/yield/getopt 2>/dev/null
	$(CXX) -c -o ../../../build/yield/getopt/option_parser.o -MD $(CXXFLAGS) ../../../src/yield/getopt/option_parser.cpp

../../../build/yield/getopt/options.o: ../../../src/yield/getopt/options.cpp
	-mkdir -p ../../../build/yield/getopt 2>/dev/null
	$(CXX) -c -o ../../../build/yield/getopt/options.o -MD $(CXXFLAGS) ../../../src/yield/getopt/options.cpp

../../../build/yield/getopt/parsed_option.o: ../../../src/yield/getopt/parsed_option.cpp
	-mkdir -p ../../../build/yield/getopt 2>/dev/null
	$(CXX) -c -o ../../../build/yield/getopt/parsed_option.o -MD $(CXXFLAGS) ../../../src/yield/getopt/parsed_option.cpp


