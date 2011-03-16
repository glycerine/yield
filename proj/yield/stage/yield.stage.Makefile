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


ifeq ($(UNAME), FreeBSD)
	LIBS += -lpthread
endif
ifeq ($(UNAME), Linux)
	LIBS += -lpthread -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lcpc -lkstat -lm -lrt -lstdc++
endif
LIBS += -lyield_thread -lyield


DEP_FILE_PATHS := $(shell find ../../../build/yield/stage -name "*.d")


OBJECT_FILE_PATHS += ../../../build/yield/stage/polling_stage_scheduler.o ../../../build/yield/stage/seda_stage_scheduler.o ../../../build/yield/stage/stage.o ../../../build/yield/stage/stage_scheduler.o ../../../build/yield/stage/wavefront_stage_scheduler.o


../../../lib/yield/libyield_stage.a: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(OBJECT_FILE_PATHS)

clean:
	$(RM) ../../../lib/yield/libyield_stage.a $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../build/yield/stage/polling_stage_scheduler.o: ../../../src/yield/stage/polling_stage_scheduler.cpp
	-mkdir -p ../../../build/yield/stage 2>/dev/null
	$(CXX) -c -o ../../../build/yield/stage/polling_stage_scheduler.o -MD $(CXXFLAGS) ../../../src/yield/stage/polling_stage_scheduler.cpp

../../../build/yield/stage/seda_stage_scheduler.o: ../../../src/yield/stage/seda_stage_scheduler.cpp
	-mkdir -p ../../../build/yield/stage 2>/dev/null
	$(CXX) -c -o ../../../build/yield/stage/seda_stage_scheduler.o -MD $(CXXFLAGS) ../../../src/yield/stage/seda_stage_scheduler.cpp

../../../build/yield/stage/stage.o: ../../../src/yield/stage/stage.cpp
	-mkdir -p ../../../build/yield/stage 2>/dev/null
	$(CXX) -c -o ../../../build/yield/stage/stage.o -MD $(CXXFLAGS) ../../../src/yield/stage/stage.cpp

../../../build/yield/stage/stage_scheduler.o: ../../../src/yield/stage/stage_scheduler.cpp
	-mkdir -p ../../../build/yield/stage 2>/dev/null
	$(CXX) -c -o ../../../build/yield/stage/stage_scheduler.o -MD $(CXXFLAGS) ../../../src/yield/stage/stage_scheduler.cpp

../../../build/yield/stage/wavefront_stage_scheduler.o: ../../../src/yield/stage/wavefront_stage_scheduler.cpp
	-mkdir -p ../../../build/yield/stage 2>/dev/null
	$(CXX) -c -o ../../../build/yield/stage/wavefront_stage_scheduler.o -MD $(CXXFLAGS) ../../../src/yield/stage/wavefront_stage_scheduler.cpp


