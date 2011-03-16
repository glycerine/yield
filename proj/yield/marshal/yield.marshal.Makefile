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


DEP_FILE_PATHS := $(shell find ../../../build/yield/marshal -name "*.d")


OBJECT_FILE_PATHS += ../../../build/yield/marshal/boolean.o ../../../build/yield/marshal/double.o ../../../build/yield/marshal/integer.o ../../../build/yield/marshal/marshaller.o ../../../build/yield/marshal/pretty_printer.o ../../../build/yield/marshal/string.o ../../../build/yield/marshal/string_literal.o ../../../build/yield/marshal/unmarshaller.o


../../../lib/yield/libyield_marshal.a: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(OBJECT_FILE_PATHS)

clean:
	$(RM) ../../../lib/yield/libyield_marshal.a $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../build/yield/marshal/boolean.o: ../../../src/yield/marshal/boolean.cpp
	-mkdir -p ../../../build/yield/marshal 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/boolean.o -MD $(CXXFLAGS) ../../../src/yield/marshal/boolean.cpp

../../../build/yield/marshal/double.o: ../../../src/yield/marshal/double.cpp
	-mkdir -p ../../../build/yield/marshal 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/double.o -MD $(CXXFLAGS) ../../../src/yield/marshal/double.cpp

../../../build/yield/marshal/integer.o: ../../../src/yield/marshal/integer.cpp
	-mkdir -p ../../../build/yield/marshal 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/integer.o -MD $(CXXFLAGS) ../../../src/yield/marshal/integer.cpp

../../../build/yield/marshal/marshaller.o: ../../../src/yield/marshal/marshaller.cpp
	-mkdir -p ../../../build/yield/marshal 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/marshaller.o -MD $(CXXFLAGS) ../../../src/yield/marshal/marshaller.cpp

../../../build/yield/marshal/pretty_printer.o: ../../../src/yield/marshal/pretty_printer.cpp
	-mkdir -p ../../../build/yield/marshal 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/pretty_printer.o -MD $(CXXFLAGS) ../../../src/yield/marshal/pretty_printer.cpp

../../../build/yield/marshal/string.o: ../../../src/yield/marshal/string.cpp
	-mkdir -p ../../../build/yield/marshal 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/string.o -MD $(CXXFLAGS) ../../../src/yield/marshal/string.cpp

../../../build/yield/marshal/string_literal.o: ../../../src/yield/marshal/string_literal.cpp
	-mkdir -p ../../../build/yield/marshal 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/string_literal.o -MD $(CXXFLAGS) ../../../src/yield/marshal/string_literal.cpp

../../../build/yield/marshal/unmarshaller.o: ../../../src/yield/marshal/unmarshaller.cpp
	-mkdir -p ../../../build/yield/marshal 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal/unmarshaller.o -MD $(CXXFLAGS) ../../../src/yield/marshal/unmarshaller.cpp


