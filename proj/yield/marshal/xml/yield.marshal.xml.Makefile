# SHELL = /bin/bash
UNAME := $(shell uname)

CXXFLAGS += -I../../../../include
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


LDFLAGS += -L../../../../lib/yield
ifeq ($(UNAME), MINGW32)
	LDFLAGS += /ignore:4006 /ignore:4221
endif


ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lm -lrt -lstdc++
endif
LIBS += -lyield_marshal -lyield


DEP_FILE_PATHS := $(shell find ../../../../build/yield/marshal/xml -name "*.d")


OBJECT_FILE_PATHS += ../../../../build/yield/marshal/xml/charProps.o ../../../../build/yield/marshal/xml/genx.o ../../../../build/yield/marshal/xml/xml_encoder.o ../../../../build/yield/marshal/xml/xml_marshaller.o ../../../../build/yield/marshal/xml/xml_unmarshaller.o


../../../../lib/yield/libyield_marshal_xml.a: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(OBJECT_FILE_PATHS)

clean:
	$(RM) ../../../../lib/yield/libyield_marshal_xml.a $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../../build/yield/marshal/xml/charProps.o: ../../../../src/yield/marshal/xml/charProps.c
	-mkdir -p ../../../../build/yield/marshal/xml 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/marshal/xml/charProps.o -MD $(CXXFLAGS) ../../../../src/yield/marshal/xml/charProps.c

../../../../build/yield/marshal/xml/genx.o: ../../../../src/yield/marshal/xml/genx.c
	-mkdir -p ../../../../build/yield/marshal/xml 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/marshal/xml/genx.o -MD $(CXXFLAGS) ../../../../src/yield/marshal/xml/genx.c

../../../../build/yield/marshal/xml/xml_encoder.o: ../../../../src/yield/marshal/xml/xml_encoder.cpp
	-mkdir -p ../../../../build/yield/marshal/xml 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/marshal/xml/xml_encoder.o -MD $(CXXFLAGS) ../../../../src/yield/marshal/xml/xml_encoder.cpp

../../../../build/yield/marshal/xml/xml_marshaller.o: ../../../../src/yield/marshal/xml/xml_marshaller.cpp
	-mkdir -p ../../../../build/yield/marshal/xml 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/marshal/xml/xml_marshaller.o -MD $(CXXFLAGS) ../../../../src/yield/marshal/xml/xml_marshaller.cpp

../../../../build/yield/marshal/xml/xml_unmarshaller.o: ../../../../src/yield/marshal/xml/xml_unmarshaller.cpp
	-mkdir -p ../../../../build/yield/marshal/xml 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/marshal/xml/xml_unmarshaller.o -MD $(CXXFLAGS) ../../../../src/yield/marshal/xml/xml_unmarshaller.cpp


