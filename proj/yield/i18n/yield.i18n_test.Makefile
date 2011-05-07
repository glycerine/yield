TIMESTAMP=$(shell date +%Y%m%dT%H%M%S)
UNAME := $(shell uname)


CXXFLAGS += -I../../../include
ifeq ($(UNAME), FreeBSD)
	CXXFLAGS += -I/usr/local/include
endif
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
ifneq ($(COVERAGE),)
	CXXFLAGS += -fprofile-arcs -ftest-coverage
	LDFLAGS += -fprofile-arcs -ftest-coverage -lgcov
endif
ifneq ($(RELEASE),)
	CXXFLAGS += -O2
else
	CXXFLAGS += -g -D_DEBUG
endif


LDFLAGS += -L../../../lib/yield
ifeq ($(UNAME), FreeBSD)
	LDFLAGS += -L/usr/local/lib
endif
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
	LIBS += -liconv -lm -lrt -lstdc++
endif
LIBS += -lyield_i18n -lyield


D_FILE_PATHS := $(shell find ../../../build/yield/i18n_test -name "*.d")


O_FILE_PATHS += ../../../build/yield/i18n_test/iconv_test.o ../../../build/yield/i18n_test/tstring_test.o ../../../build/yield/i18n_test/yield_i18n_test_main.o


all: ../../../bin/yield/yield_i18n_test

clean:
	$(RM) ../../../bin/yield/yield_i18n_test $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)
			
lcov: ../../../bin/yield/yield_i18n_test
	lcov --directory ../../../build/yield/i18n_test --zerocounters
	-../../../bin/yield/yield_i18n_test
	lcov --base-directory . --directory ../../../build/yield/i18n_test --capture --output-file yield.i18n_test_lcov-$(TIMESTAMP)
	mkdir yield.i18n_test_lcov_html-$(TIMESTAMP)
	genhtml -o yield.i18n_test_lcov_html-$(TIMESTAMP) yield.i18n_test_lcov-$(TIMESTAMP)
	-cp -R yield.i18n_test_lcov_html-$(TIMESTAMP) /mnt/hgfs/minorg/Desktop
	zip -qr yield.i18n_test_lcov_html-$(TIMESTAMP).zip yield.i18n_test_lcov_html-$(TIMESTAMP)/*
	rm -fr yield.i18n_test_lcov_html-$(TIMESTAMP)


../../../lib/yield/libyield_i18n.a:
	$(MAKE) -C . yield.i18n.Makefile


../../../bin/yield/yield_i18n_test: $(O_FILE_PATHS) ../../../lib/yield/libyield_i18n.a
	-mkdir -p ../../../bin/yield 2>/dev/null
	$(LINK.cpp) $(O_FILE_PATHS) -o $@ $(LIBS)

../../../build/yield/i18n_test/iconv_test.o: ../../../test/yield/i18n/iconv_test.cpp
	-mkdir -p ../../../build/yield/i18n_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/i18n_test/iconv_test.o -MD $(CXXFLAGS) ../../../test/yield/i18n/iconv_test.cpp

../../../build/yield/i18n_test/tstring_test.o: ../../../test/yield/i18n/tstring_test.cpp
	-mkdir -p ../../../build/yield/i18n_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/i18n_test/tstring_test.o -MD $(CXXFLAGS) ../../../test/yield/i18n/tstring_test.cpp

../../../build/yield/i18n_test/yield_i18n_test_main.o: ../../../test/yield/i18n/yield_i18n_test_main.cpp
	-mkdir -p ../../../build/yield/i18n_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/i18n_test/yield_i18n_test_main.o -MD $(CXXFLAGS) ../../../test/yield/i18n/yield_i18n_test_main.cpp
