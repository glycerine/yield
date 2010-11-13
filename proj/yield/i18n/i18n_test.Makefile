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


LDFLAGS += -L../../../lib
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
	LIBS += -lm -lrt -lstdc++ -liconv
endif
LIBS += -lyield_i18n -lyield_common


DEP_FILE_PATHS := $(shell find ../../../build/yield/i18n_test -name "*.d")


OBJECT_FILE_PATHS += ../../../build/yield/i18n_test/iconv_test.o ../../../build/yield/i18n_test/tstring_test.o ../../../build/yield/i18n_test/yield_i18n_test_main.o


../../../bin/yield_i18n_test: $(OBJECT_FILE_PATHS)
	$(LINK.cpp) $(OBJECT_FILE_PATHS) -o $@ $(LIBS)

clean:
	$(RM) ../../../bin/yield_i18n_test $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../build/yield/i18n_test/iconv_test.o: ../../../src/yield/i18n/iconv_test.cpp
	$(CXX) -c -o ../../../build/yield/i18n_test/iconv_test.o -MD $(CXXFLAGS) ../../../src/yield/i18n/iconv_test.cpp

../../../build/yield/i18n_test/tstring_test.o: ../../../src/yield/i18n/tstring_test.cpp
	$(CXX) -c -o ../../../build/yield/i18n_test/tstring_test.o -MD $(CXXFLAGS) ../../../src/yield/i18n/tstring_test.cpp

../../../build/yield/i18n_test/yield_i18n_test_main.o: ../../../src/yield/i18n/yield_i18n_test_main.cpp
	$(CXX) -c -o ../../../build/yield/i18n_test/yield_i18n_test_main.o -MD $(CXXFLAGS) ../../../src/yield/i18n/yield_i18n_test_main.cpp


