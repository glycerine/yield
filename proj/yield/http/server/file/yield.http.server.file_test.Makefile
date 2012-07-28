TIMESTAMP=$(shell date +%Y%m%dT%H%M%S)
UNAME := $(shell uname)


CXXFLAGS += -I../../../../../include
ifeq ($(UNAME), FreeBSD)
	CXXFLAGS += -I/usr/local/include
endif
ifeq ($(UNAME), FreeBSD)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wold-style-cast -Wunused
endif
ifeq ($(UNAME), Linux)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wold-style-cast -Wunused
endif
ifeq ($(UNAME), Darwin)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wold-style-cast -Wunused
endif
ifeq ($(UNAME), Solaris)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wold-style-cast -Wunused
endif
ifeq ($(UNAME), MINGW32)
	CXXFLAGS += /EHsc /GR- /MP /nologo /wd"4100" /wd"4127" /wd"4290" /wd"4355" /wd"4512"
endif
ifneq ($(COVERAGE),)
	CXXFLAGS += -fprofile-arcs -ftest-coverage
	LDFLAGS += -fprofile-arcs -ftest-coverage -lgcov
else ifneq ($(RELEASE),)
	CXXFLAGS += -O2
else
	CXXFLAGS += -g -D_DEBUG
endif



LDFLAGS += -L../../../../../lib/yield
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
	LIBS += -liconv -lintl -lpthread
endif
ifeq ($(UNAME), Linux)
	LIBS += -lpthread -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -liconv -lnsl -lsocket -lkstat -lm -lrt -lstdc++
endif
ifeq ($(UNAME), MINGW32)
	LIBS += -lgtestd
endif
ifeq ($(UNAME), Darwin)
	LIBS += -lgtest
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lgtest
endif
ifeq ($(UNAME), FreeBSD)
	LIBS += -lgtest
endif
ifeq ($(UNAME), Linux)
	LIBS += -lgtest
endif
LIBS += -lyield_http_server_file -lyield_http_server -lyield_fs -lyield_i18n -lyield_http -lyield_uri -lyield_sockets_aio -lyield_poll -lyield_sockets -lyield_stage -lyield_thread -lyield


D_FILE_PATHS := $(shell find ../../../../../build/yield/http/server/file -name "*.d")


O_FILE_PATHS += ../../../../../build/yield/http/server/file/http_file_server_test.o ../../../../../build/yield/http/server/file/yield_http_server_file_test_main.o


all: ../../../../../bin/yield/yield_http_server_file_test

clean:
	$(RM) ../../../../../bin/yield/yield_http_server_file_test $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)

lcov: ../../../../../bin/yield/yield_http_server_file_test
	lcov --directory ../../../../../build/yield/http/server/file --zerocounters
	-../../../../../bin/yield/yield_http_server_file_test
	lcov --base-directory . --directory ../../../../../build/yield/http/server/file --capture --output-file yield.http.server.file_test_lcov-$(TIMESTAMP)
	mkdir yield.http.server.file_test_lcov_html-$(TIMESTAMP)
	genhtml -o yield.http.server.file_test_lcov_html-$(TIMESTAMP) yield.http.server.file_test_lcov-$(TIMESTAMP)
	-cp -R yield.http.server.file_test_lcov_html-$(TIMESTAMP) /mnt/hgfs/minorg/Desktop
	zip -qr yield.http.server.file_test_lcov_html-$(TIMESTAMP).zip yield.http.server.file_test_lcov_html-$(TIMESTAMP)/*
	rm -fr yield.http.server.file_test_lcov_html-$(TIMESTAMP)


../../../../../lib/yield/libyield_http_server_file.a:
	$(MAKE) -C . yield.http.server.file.Makefile


../../../../../bin/yield/yield_http_server_file_test: $(O_FILE_PATHS) ../../../../../lib/yield/libyield_http_server_file.a
	-mkdir -p ../../../../../bin/yield 2>/dev/null
	$(LINK.cpp) $(O_FILE_PATHS) -o $@ $(LIBS)

../../../../../build/yield/http/server/file/http_file_server_test.o: ../../../../../test/yield/http/server/file/http_file_server_test.cpp
	-mkdir -p ../../../../../build/yield/http/server/file 2>/dev/null
	$(CXX) -c -o ../../../../../build/yield/http/server/file/http_file_server_test.o -MD $(CXXFLAGS) ../../../../../test/yield/http/server/file/http_file_server_test.cpp

../../../../../build/yield/http/server/file/yield_http_server_file_test_main.o: ../../../../../test/yield/http/server/file/yield_http_server_file_test_main.cpp
	-mkdir -p ../../../../../build/yield/http/server/file 2>/dev/null
	$(CXX) -c -o ../../../../../build/yield/http/server/file/yield_http_server_file_test_main.o -MD $(CXXFLAGS) ../../../../../test/yield/http/server/file/yield_http_server_file_test_main.cpp
