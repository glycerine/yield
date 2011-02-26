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


ifeq ($(UNAME), Darwin)
	LIBS += -liconv
endif
ifeq ($(UNAME), FreeBSD)
	LIBS += -liconv -lintl
endif
ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++ -ldl
endif
ifeq ($(UNAME), Solaris)
	LIBS += -liconv -lm -lrt -lstdc++
endif
LIBS += -lyield_process -lyield_fs -lyield_i18n -lyield_common


DEP_FILE_PATHS := $(shell find ../../../build/yield/process_test -name "*.d")


OBJECT_FILE_PATHS += ../../../build/yield/process_test/process_test.o ../../../build/yield/process_test/shared_library_test.o ../../../build/yield/process_test/yield_process_test_main.o


../../../bin/yield/yield_process_test: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../bin/yield 2>/dev/null
	$(LINK.cpp) $(OBJECT_FILE_PATHS) -o $@ $(LIBS)

clean:
	$(RM) ../../../bin/yield/yield_process_test $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../build/yield/process_test/process_test.o: ../../../src/yield/process/process_test.cpp
	-mkdir -p ../../../build/yield/process_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/process_test/process_test.o -MD $(CXXFLAGS) ../../../src/yield/process/process_test.cpp

../../../build/yield/process_test/shared_library_test.o: ../../../src/yield/process/shared_library_test.cpp
	-mkdir -p ../../../build/yield/process_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/process_test/shared_library_test.o -MD $(CXXFLAGS) ../../../src/yield/process/shared_library_test.cpp

../../../build/yield/process_test/yield_process_test_main.o: ../../../src/yield/process/yield_process_test_main.cpp
	-mkdir -p ../../../build/yield/process_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/process_test/yield_process_test_main.o -MD $(CXXFLAGS) ../../../src/yield/process/yield_process_test_main.cpp


