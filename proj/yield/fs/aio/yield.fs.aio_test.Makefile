TIMESTAMP=$(shell date +%Y%m%dT%H%M%S)
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


LDFLAGS += -L../../../../lib/yield
ifeq ($(UNAME), MINGW32)
	LDFLAGS += /ignore:4006 /ignore:4221
endif


ifeq ($(UNAME), Darwin)
	LIBS += -liconv
endif
ifeq ($(UNAME), FreeBSD)
	LIBS += -liconv -lintl -lpthread
endif
ifeq ($(UNAME), Linux)
	LIBS += -lpthread -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -liconv -lcpc -lkstat -lm -lrt -lstdc++
endif
LIBS += -lyield_fs_aio -lyield_aio -lyield_fs -lyield_i18n -lyield_thread -lyield


D_FILE_PATHS := $(shell find ../../../../build/yield/fs/aio -name "*.d")


O_FILE_PATHS += ../../../../build/yield/fs/aio/bio_queue_test.o ../../../../build/yield/fs/aio/yield_fs_aio_test_main.o
ifeq ($(UNAME), Darwin)
	O_FILE_PATHS += ../../../../build/yield/fs/aio/posix/aio_queue_test.o
endif
ifeq ($(UNAME), FreeBSD)
	O_FILE_PATHS += ../../../../build/yield/fs/aio/posix/aio_queue_test.o
endif
ifeq ($(UNAME), Linux)
	O_FILE_PATHS += ../../../../build/yield/fs/aio/posix/aio_queue_test.o
endif
ifeq ($(UNAME), Solaris)
	O_FILE_PATHS += ../../../../build/yield/fs/aio/posix/aio_queue_test.o
endif
ifeq ($(UNAME), MINGW32)
	O_FILE_PATHS += ../../../../build/yield/fs/aio_test/win32/aio_queue_test.o
endif


all: ../../../../bin/yield/yield_fs_aio_test

clean:
	$(RM) ../../../../bin/yield/yield_fs_aio_test $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)
			
lcov: ../../../../bin/yield/yield_fs_aio_test
	lcov --directory ../../../../build/yield/fs/aio --zerocounters
	../../../../bin/yield/yield_fs_aio_test
	lcov --base-directory . --directory ../../../../build/yield/fs/aio --capture --output-file yield.fs.aio_test_lcov-$(TIMESTAMP)
	mkdir yield.fs.aio_test_lcov_html-$(TIMESTAMP)
	genhtml -o yield.fs.aio_test_lcov_html-$(TIMESTAMP) yield.fs.aio_test_lcov-$(TIMESTAMP)
	-cp -R yield.fs.aio_test_lcov_html-$(TIMESTAMP) /mnt/hgfs/minorg/Desktop
	zip -qr yield.fs.aio_test_lcov_html-$(TIMESTAMP).zip yield.fs.aio_test_lcov_html-$(TIMESTAMP)/*
	rm -fr yield.fs.aio_test_lcov_html-$(TIMESTAMP)


../../../../bin/yield/yield_fs_aio_test: $(O_FILE_PATHS) ..\..\..\..\lib\yield\libyield_fs_aio.a
	-mkdir -p ../../../../bin/yield 2>/dev/null
	$(LINK.cpp) $(O_FILE_PATHS) -o $@ $(LIBS)

../../../../build/yield/fs/aio/bio_queue_test.o: ../../../../test/yield/fs/aio/bio_queue_test.cpp
	-mkdir -p ../../../../build/yield/fs/aio 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio/bio_queue_test.o -MD $(CXXFLAGS) ../../../../test/yield/fs/aio/bio_queue_test.cpp

../../../../build/yield/fs/aio/posix/aio_queue_test.o: ../../../../test/yield/fs/aio/posix/aio_queue_test.cpp
	-mkdir -p ../../../../build/yield/fs/aio/posix 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio/posix/aio_queue_test.o -MD $(CXXFLAGS) ../../../../test/yield/fs/aio/posix/aio_queue_test.cpp

../../../../build/yield/fs/aio/yield_fs_aio_test_main.o: ../../../../test/yield/fs/aio/yield_fs_aio_test_main.cpp
	-mkdir -p ../../../../build/yield/fs/aio 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio/yield_fs_aio_test_main.o -MD $(CXXFLAGS) ../../../../test/yield/fs/aio/yield_fs_aio_test_main.cpp

../../../../build/yield/fs/aio_test/win32/aio_queue_test.o: ../../../../test/yield/fs/aio/win32/aio_queue_test.cpp
	-mkdir -p ../../../../build/yield/fs/aio_test/win32 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio_test/win32/aio_queue_test.o -MD $(CXXFLAGS) ../../../../test/yield/fs/aio/win32/aio_queue_test.cpp
