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


ifeq ($(UNAME), FreeBSD)
	LIBS += -lpthread
endif
ifeq ($(UNAME), Linux)
	LIBS += -lpthread -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lcpc -lkstat -lm -lrt -lstdc++
endif
LIBS += -lyield_stage -lyield_thread -lyield


D_FILE_PATHS := $(shell find ../../../build/yield/stage_test -name "*.d")


O_FILE_PATHS += ../../../build/yield/stage_test/seda_stage_scheduler_test.o ../../../build/yield/stage_test/stage_test.o ../../../build/yield/stage_test/wavefront_stage_scheduler_test.o ../../../build/yield/stage_test/yield_stage_test_main.o


all: ../../../bin/yield/yield_stage_test

clean:
	$(RM) ../../../bin/yield/yield_stage_test $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)
			
lcov: ..\..\..\bin\yield\yield_stage_test TIMESTAMP=`date +%Y%m%dT%H%M%S`
	lcov --directory ../../../build/yield/stage_test --zerocounters
	..\..\..\bin\yield\yield_stage_test
	lcov --base-directory . --directory ../../../build/yield/stage_test --capture --output-file yield.stage_test_lcov-$TIMESTAMP
	mkdir yield.stage_test_lcov_html-$TIMESTAMP
	genhtml -o yield.stage_test_lcov_html-$TIMESTAMP yield.stage_test_lcov-$TIMESTAMP
	#tar cf yield.stage_test_lcov_html-$TIMESTAMP.tar yield.stage_test_lcov_html-$TIMESTAMP
	#gzip yield.stage_test_lcov_html-$TIMESTAMP.tar
	if [ -d /mnt/hgfs/minorg/Desktop ]; then
	  cp -R yield.stage_test_lcov_html-$TIMESTAMP /mnt/hgfs/minorg/Desktop
	else
	  zip -qr yield.stage_test_lcov_html-$TIMESTAMP.zip yield.stage_test_lcov_html-$TIMESTAMP/*
	fi
	rm -fr yield.stage_test_lcov_html-$TIMESTAMP


../../../bin/yield/yield_stage_test: $(O_FILE_PATHS)
	-mkdir -p ../../../bin/yield 2>/dev/null
	$(LINK.cpp) $(O_FILE_PATHS) -o $@ $(LIBS)

../../../build/yield/stage_test/seda_stage_scheduler_test.o: ../../../test/yield/stage/seda_stage_scheduler_test.cpp
	-mkdir -p ../../../build/yield/stage_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/stage_test/seda_stage_scheduler_test.o -MD $(CXXFLAGS) ../../../test/yield/stage/seda_stage_scheduler_test.cpp

../../../build/yield/stage_test/stage_test.o: ../../../test/yield/stage/stage_test.cpp
	-mkdir -p ../../../build/yield/stage_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/stage_test/stage_test.o -MD $(CXXFLAGS) ../../../test/yield/stage/stage_test.cpp

../../../build/yield/stage_test/wavefront_stage_scheduler_test.o: ../../../test/yield/stage/wavefront_stage_scheduler_test.cpp
	-mkdir -p ../../../build/yield/stage_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/stage_test/wavefront_stage_scheduler_test.o -MD $(CXXFLAGS) ../../../test/yield/stage/wavefront_stage_scheduler_test.cpp

../../../build/yield/stage_test/yield_stage_test_main.o: ../../../test/yield/stage/yield_stage_test_main.cpp
	-mkdir -p ../../../build/yield/stage_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/stage_test/yield_stage_test_main.o -MD $(CXXFLAGS) ../../../test/yield/stage/yield_stage_test_main.cpp
