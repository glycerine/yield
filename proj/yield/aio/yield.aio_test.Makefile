TIMESTAMP=$(shell date +%Y%m%dT%H%M%S)
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
ifeq ($(UNAME), MINGW32)
	LDFLAGS += /ignore:4006 /ignore:4221
endif


ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lm -lrt -lstdc++
endif
LIBS += -lyield_aio -lyield


D_FILE_PATHS := $(shell find ../../../build/yield/aio -name "*.d")


O_FILE_PATHS += ../../../build/yield/aio/yield_aio_test_main.o


all: ../../../bin/yield/yield_aio_test

clean:
	$(RM) ../../../bin/yield/yield_aio_test $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)
			
lcov: ../../../bin/yield/yield_aio_test
	lcov --directory ../../../build/yield/aio --zerocounters
	../../../bin/yield/yield_aio_test
	lcov --base-directory . --directory ../../../build/yield/aio --capture --output-file yield.aio_test_lcov-$(TIMESTAMP)
	mkdir yield.aio_test_lcov_html-$(TIMESTAMP)
	genhtml -o yield.aio_test_lcov_html-$(TIMESTAMP) yield.aio_test_lcov-$(TIMESTAMP)
	-cp -R yield.aio_test_lcov_html-$(TIMESTAMP) /mnt/hgfs/minorg/Desktop
	zip -qr yield.aio_test_lcov_html-$(TIMESTAMP).zip yield.aio_test_lcov_html-$(TIMESTAMP)/*
	rm -fr yield.aio_test_lcov_html-$(TIMESTAMP)


../../../lib/yield/libyield_aio.a:
	$(MAKE) -C . yield.aio.Makefile


../../../bin/yield/yield_aio_test: $(O_FILE_PATHS) ../../../lib/yield/libyield_aio.a
	-mkdir -p ../../../bin/yield 2>/dev/null
	$(LINK.cpp) $(O_FILE_PATHS) -o $@ $(LIBS)

../../../build/yield/aio/yield_aio_test_main.o: ../../../test/yield/aio/yield_aio_test_main.cpp
	-mkdir -p ../../../build/yield/aio 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/yield_aio_test_main.o -MD $(CXXFLAGS) ../../../test/yield/aio/yield_aio_test_main.cpp
