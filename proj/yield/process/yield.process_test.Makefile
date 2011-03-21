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


ifeq ($(UNAME), Darwin)
	LIBS += -liconv
endif
ifeq ($(UNAME), FreeBSD)
	LIBS += -liconv -lintl
endif
ifeq ($(UNAME), Linux)
	LIBS += -ldl -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -liconv -lm -lrt -lstdc++
endif
LIBS += -lyield_process -lyield_fs -lyield_i18n -lyield


D_FILE_PATHS := $(shell find ../../../build/yield/process -name "*.d")


O_FILE_PATHS += ../../../build/yield/process/process_test.o ../../../build/yield/process/shared_library_test.o ../../../build/yield/process/yield_process_test_main.o


all: ../../../bin/yield/yield_process_test

clean:
	$(RM) ../../../bin/yield/yield_process_test $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)
			
lcov: ../../../bin/yield/yield_process_test
	lcov --directory ../../../build/yield/process --zerocounters
	../../../bin/yield/yield_process_test
	lcov --base-directory . --directory ../../../build/yield/process --capture --output-file yield.process_test_lcov-$(TIMESTAMP)
	mkdir yield.process_test_lcov_html-$(TIMESTAMP)
	genhtml -o yield.process_test_lcov_html-$(TIMESTAMP) yield.process_test_lcov-$(TIMESTAMP)
	-cp -R yield.process_test_lcov_html-$(TIMESTAMP) /mnt/hgfs/minorg/Desktop
	zip -qr yield.process_test_lcov_html-$(TIMESTAMP).zip yield.process_test_lcov_html-$(TIMESTAMP)/*
	rm -fr yield.process_test_lcov_html-$(TIMESTAMP)


../../../bin/yield/yield_process_test: $(O_FILE_PATHS)
	-mkdir -p ../../../bin/yield 2>/dev/null
	$(LINK.cpp) $(O_FILE_PATHS) -o $@ $(LIBS)

../../../build/yield/process/process_test.o: ../../../test/yield/process/process_test.cpp
	-mkdir -p ../../../build/yield/process 2>/dev/null
	$(CXX) -c -o ../../../build/yield/process/process_test.o -MD $(CXXFLAGS) ../../../test/yield/process/process_test.cpp

../../../build/yield/process/shared_library_test.o: ../../../test/yield/process/shared_library_test.cpp
	-mkdir -p ../../../build/yield/process 2>/dev/null
	$(CXX) -c -o ../../../build/yield/process/shared_library_test.o -MD $(CXXFLAGS) ../../../test/yield/process/shared_library_test.cpp

../../../build/yield/process/yield_process_test_main.o: ../../../test/yield/process/yield_process_test_main.cpp
	-mkdir -p ../../../build/yield/process 2>/dev/null
	$(CXX) -c -o ../../../build/yield/process/yield_process_test_main.o -MD $(CXXFLAGS) ../../../test/yield/process/yield_process_test_main.cpp
