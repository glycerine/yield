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
	CXXFLAGS += /EHsc /GR- /MP /nologo /wd"4100" /wd"4127" /wd"4290" /wd"4355" /wd"4512"
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
LIBS += -lyield_uri -lyield


D_FILE_PATHS := $(shell find ../../../build/yield/uri_test -name "*.d")


O_FILE_PATHS += ../../../build/yield/uri_test/uri_test.o ../../../build/yield/uri_test/yield_uri_test_main.o


all: ../../../bin/yield/yield_uri_test

clean:
	$(RM) ../../../bin/yield/yield_uri_test $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)
			
lcov: ..\..\..\bin\yield\yield_uri_test TIMESTAMP=`date +%Y%m%dT%H%M%S`
	lcov --directory ../../../build/yield/uri_test --zerocounters
	..\..\..\bin\yield\yield_uri_test
	lcov --base-directory . --directory ../../../build/yield/uri_test --capture --output-file yield.uri_test_lcov-$TIMESTAMP
	mkdir yield.uri_test_lcov_html-$TIMESTAMP
	genhtml -o yield.uri_test_lcov_html-$TIMESTAMP yield.uri_test_lcov-$TIMESTAMP
	#tar cf yield.uri_test_lcov_html-$TIMESTAMP.tar yield.uri_test_lcov_html-$TIMESTAMP
	#gzip yield.uri_test_lcov_html-$TIMESTAMP.tar
	if [ -d /mnt/hgfs/minorg/Desktop ]; then
	  cp -R yield.uri_test_lcov_html-$TIMESTAMP /mnt/hgfs/minorg/Desktop
	else
	  zip -qr yield.uri_test_lcov_html-$TIMESTAMP.zip yield.uri_test_lcov_html-$TIMESTAMP/*
	fi
	rm -fr yield.uri_test_lcov_html-$TIMESTAMP


../../../bin/yield/yield_uri_test: $(O_FILE_PATHS)
	-mkdir -p ../../../bin/yield 2>/dev/null
	$(LINK.cpp) $(O_FILE_PATHS) -o $@ $(LIBS)

../../../build/yield/uri_test/uri_test.o: ../../../test/yield/uri/uri_test.cpp
	-mkdir -p ../../../build/yield/uri_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/uri_test/uri_test.o -MD $(CXXFLAGS) ../../../test/yield/uri/uri_test.cpp

../../../build/yield/uri_test/yield_uri_test_main.o: ../../../test/yield/uri/yield_uri_test_main.cpp
	-mkdir -p ../../../build/yield/uri_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/uri_test/yield_uri_test_main.o -MD $(CXXFLAGS) ../../../test/yield/uri/yield_uri_test_main.cpp
