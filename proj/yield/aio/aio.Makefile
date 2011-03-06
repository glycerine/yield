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
	LIBS += -liconv -lintl -lpthread
endif
ifeq ($(UNAME), Linux)
	LIBS += -lpthread -lrt -lstdc++ -laio
endif
ifeq ($(UNAME), Solaris)
	LIBS += -liconv -lnsl -lsocket -lcpc -lkstat -lm -lrt -lstdc++
endif
LIBS += -lyield_fs -lyield_i18n -lyield_poll -lyield_net -lyield_stage -lyield_thread -lyield_common


DEP_FILE_PATHS := $(shell find ../../../build/yield/aio -name "*.d")


OBJECT_FILE_PATHS += ../../../build/yield/aio/bio_queue.o ../../../build/yield/aio/fs/aio_queue.o ../../../build/yield/aio/fs/aiocb.o ../../../build/yield/aio/fs/bio_queue.o ../../../build/yield/aio/fs/fdatasync_aiocb.o ../../../build/yield/aio/fs/fsync_aiocb.o ../../../build/yield/aio/fs/pread_aiocb.o ../../../build/yield/aio/fs/pwrite_aiocb.o ../../../build/yield/aio/fs/setlk_aiocb.o ../../../build/yield/aio/fs/unlk_aiocb.o ../../../build/yield/aio/net/sockets/accept_aiocb.o ../../../build/yield/aio/net/sockets/aio_queue.o ../../../build/yield/aio/net/sockets/aiocb.o ../../../build/yield/aio/net/sockets/bio_queue.o ../../../build/yield/aio/net/sockets/connect_aiocb.o ../../../build/yield/aio/net/sockets/nbio_queue.o ../../../build/yield/aio/net/sockets/recv_aiocb.o ../../../build/yield/aio/net/sockets/send_aiocb.o
ifeq ($(UNAME), Darwin)
	OBJECT_FILE_PATHS += ../../../build/yield/aio/fs/posix/aio_queue.o ../../../build/yield/aio/fs/posix/fdatasync_aiocb.o ../../../build/yield/aio/fs/posix/fsync_aiocb.o ../../../build/yield/aio/fs/posix/pread_aiocb.o ../../../build/yield/aio/fs/posix/pwrite_aiocb.o ../../../build/yield/aio/net/sockets/posix/accept_aiocb.o ../../../build/yield/aio/posix/aiocb.o
endif
ifeq ($(UNAME), FreeBSD)
	OBJECT_FILE_PATHS += ../../../build/yield/aio/fs/posix/aio_queue.o ../../../build/yield/aio/fs/posix/fdatasync_aiocb.o ../../../build/yield/aio/fs/posix/fsync_aiocb.o ../../../build/yield/aio/fs/posix/pread_aiocb.o ../../../build/yield/aio/fs/posix/pwrite_aiocb.o ../../../build/yield/aio/net/sockets/posix/accept_aiocb.o ../../../build/yield/aio/posix/aiocb.o
endif
ifeq ($(UNAME), Linux)
	OBJECT_FILE_PATHS += ../../../build/yield/aio/fs/posix/aio_queue.o ../../../build/yield/aio/fs/posix/fdatasync_aiocb.o ../../../build/yield/aio/fs/posix/fsync_aiocb.o ../../../build/yield/aio/fs/posix/pread_aiocb.o ../../../build/yield/aio/fs/posix/pwrite_aiocb.o ../../../build/yield/aio/net/sockets/posix/accept_aiocb.o ../../../build/yield/aio/posix/aiocb.o
endif
ifeq ($(UNAME), Solaris)
	OBJECT_FILE_PATHS += ../../../build/yield/aio/fs/posix/aio_queue.o ../../../build/yield/aio/fs/posix/fdatasync_aiocb.o ../../../build/yield/aio/fs/posix/fsync_aiocb.o ../../../build/yield/aio/fs/posix/pread_aiocb.o ../../../build/yield/aio/fs/posix/pwrite_aiocb.o ../../../build/yield/aio/net/sockets/posix/accept_aiocb.o ../../../build/yield/aio/posix/aiocb.o
endif
ifeq ($(UNAME), MINGW32)
	OBJECT_FILE_PATHS += ../../../build/yield/aio/fs/win32/aio_queue.o ../../../build/yield/aio/fs/win32/aiocb.o ../../../build/yield/aio/fs/win32/pread_aiocb.o ../../../build/yield/aio/fs/win32/pwrite_aiocb.o ../../../build/yield/aio/fs/win32/setlk_aiocb.o ../../../build/yield/aio/fs/win32/unlk_aiocb.o ../../../build/yield/aio/net/sockets/win32/accept_aiocb.o ../../../build/yield/aio/net/sockets/win32/aio_queue.o ../../../build/yield/aio/net/sockets/win32/aiocb.o ../../../build/yield/aio/net/sockets/win32/connect_aiocb.o ../../../build/yield/aio/net/sockets/win32/recv_aiocb.o ../../../build/yield/aio/net/sockets/win32/send_aiocb.o ../../../build/yield/aio/win32/aio_queue.o ../../../build/yield/aio/win32/aiocb.o
endif


../../../lib/yield/libyield_aio.a: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(OBJECT_FILE_PATHS)

clean:
	$(RM) ../../../lib/yield/libyield_aio.a $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../build/yield/aio/bio_queue.o: ../../../src/yield/aio/bio_queue.cpp
	-mkdir -p ../../../build/yield/aio 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/bio_queue.o -MD $(CXXFLAGS) ../../../src/yield/aio/bio_queue.cpp

../../../build/yield/aio/fs/aio_queue.o: ../../../src/yield/aio/fs/aio_queue.cpp
	-mkdir -p ../../../build/yield/aio/fs 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/fs/aio_queue.o -MD $(CXXFLAGS) ../../../src/yield/aio/fs/aio_queue.cpp

../../../build/yield/aio/fs/aiocb.o: ../../../src/yield/aio/fs/aiocb.cpp
	-mkdir -p ../../../build/yield/aio/fs 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/fs/aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/fs/aiocb.cpp

../../../build/yield/aio/fs/bio_queue.o: ../../../src/yield/aio/fs/bio_queue.cpp
	-mkdir -p ../../../build/yield/aio/fs 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/fs/bio_queue.o -MD $(CXXFLAGS) ../../../src/yield/aio/fs/bio_queue.cpp

../../../build/yield/aio/fs/fdatasync_aiocb.o: ../../../src/yield/aio/fs/fdatasync_aiocb.cpp
	-mkdir -p ../../../build/yield/aio/fs 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/fs/fdatasync_aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/fs/fdatasync_aiocb.cpp

../../../build/yield/aio/fs/fsync_aiocb.o: ../../../src/yield/aio/fs/fsync_aiocb.cpp
	-mkdir -p ../../../build/yield/aio/fs 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/fs/fsync_aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/fs/fsync_aiocb.cpp

../../../build/yield/aio/fs/posix/aio_queue.o: ../../../src/yield/aio/fs/posix/aio_queue.cpp
	-mkdir -p ../../../build/yield/aio/fs/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/fs/posix/aio_queue.o -MD $(CXXFLAGS) ../../../src/yield/aio/fs/posix/aio_queue.cpp

../../../build/yield/aio/fs/posix/fdatasync_aiocb.o: ../../../src/yield/aio/fs/posix/fdatasync_aiocb.cpp
	-mkdir -p ../../../build/yield/aio/fs/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/fs/posix/fdatasync_aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/fs/posix/fdatasync_aiocb.cpp

../../../build/yield/aio/fs/posix/fsync_aiocb.o: ../../../src/yield/aio/fs/posix/fsync_aiocb.cpp
	-mkdir -p ../../../build/yield/aio/fs/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/fs/posix/fsync_aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/fs/posix/fsync_aiocb.cpp

../../../build/yield/aio/fs/posix/pread_aiocb.o: ../../../src/yield/aio/fs/posix/pread_aiocb.cpp
	-mkdir -p ../../../build/yield/aio/fs/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/fs/posix/pread_aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/fs/posix/pread_aiocb.cpp

../../../build/yield/aio/fs/posix/pwrite_aiocb.o: ../../../src/yield/aio/fs/posix/pwrite_aiocb.cpp
	-mkdir -p ../../../build/yield/aio/fs/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/fs/posix/pwrite_aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/fs/posix/pwrite_aiocb.cpp

../../../build/yield/aio/fs/pread_aiocb.o: ../../../src/yield/aio/fs/pread_aiocb.cpp
	-mkdir -p ../../../build/yield/aio/fs 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/fs/pread_aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/fs/pread_aiocb.cpp

../../../build/yield/aio/fs/pwrite_aiocb.o: ../../../src/yield/aio/fs/pwrite_aiocb.cpp
	-mkdir -p ../../../build/yield/aio/fs 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/fs/pwrite_aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/fs/pwrite_aiocb.cpp

../../../build/yield/aio/fs/setlk_aiocb.o: ../../../src/yield/aio/fs/setlk_aiocb.cpp
	-mkdir -p ../../../build/yield/aio/fs 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/fs/setlk_aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/fs/setlk_aiocb.cpp

../../../build/yield/aio/fs/unlk_aiocb.o: ../../../src/yield/aio/fs/unlk_aiocb.cpp
	-mkdir -p ../../../build/yield/aio/fs 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/fs/unlk_aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/fs/unlk_aiocb.cpp

../../../build/yield/aio/fs/win32/aio_queue.o: ../../../src/yield/aio/fs/win32/aio_queue.cpp
	-mkdir -p ../../../build/yield/aio/fs/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/fs/win32/aio_queue.o -MD $(CXXFLAGS) ../../../src/yield/aio/fs/win32/aio_queue.cpp

../../../build/yield/aio/fs/win32/aiocb.o: ../../../src/yield/aio/fs/win32/aiocb.cpp
	-mkdir -p ../../../build/yield/aio/fs/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/fs/win32/aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/fs/win32/aiocb.cpp

../../../build/yield/aio/fs/win32/pread_aiocb.o: ../../../src/yield/aio/fs/win32/pread_aiocb.cpp
	-mkdir -p ../../../build/yield/aio/fs/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/fs/win32/pread_aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/fs/win32/pread_aiocb.cpp

../../../build/yield/aio/fs/win32/pwrite_aiocb.o: ../../../src/yield/aio/fs/win32/pwrite_aiocb.cpp
	-mkdir -p ../../../build/yield/aio/fs/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/fs/win32/pwrite_aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/fs/win32/pwrite_aiocb.cpp

../../../build/yield/aio/fs/win32/setlk_aiocb.o: ../../../src/yield/aio/fs/win32/setlk_aiocb.cpp
	-mkdir -p ../../../build/yield/aio/fs/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/fs/win32/setlk_aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/fs/win32/setlk_aiocb.cpp

../../../build/yield/aio/fs/win32/unlk_aiocb.o: ../../../src/yield/aio/fs/win32/unlk_aiocb.cpp
	-mkdir -p ../../../build/yield/aio/fs/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/fs/win32/unlk_aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/fs/win32/unlk_aiocb.cpp

../../../build/yield/aio/net/sockets/accept_aiocb.o: ../../../src/yield/aio/net/sockets/accept_aiocb.cpp
	-mkdir -p ../../../build/yield/aio/net/sockets 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/net/sockets/accept_aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/net/sockets/accept_aiocb.cpp

../../../build/yield/aio/net/sockets/aio_queue.o: ../../../src/yield/aio/net/sockets/aio_queue.cpp
	-mkdir -p ../../../build/yield/aio/net/sockets 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/net/sockets/aio_queue.o -MD $(CXXFLAGS) ../../../src/yield/aio/net/sockets/aio_queue.cpp

../../../build/yield/aio/net/sockets/aiocb.o: ../../../src/yield/aio/net/sockets/aiocb.cpp
	-mkdir -p ../../../build/yield/aio/net/sockets 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/net/sockets/aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/net/sockets/aiocb.cpp

../../../build/yield/aio/net/sockets/bio_queue.o: ../../../src/yield/aio/net/sockets/bio_queue.cpp
	-mkdir -p ../../../build/yield/aio/net/sockets 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/net/sockets/bio_queue.o -MD $(CXXFLAGS) ../../../src/yield/aio/net/sockets/bio_queue.cpp

../../../build/yield/aio/net/sockets/connect_aiocb.o: ../../../src/yield/aio/net/sockets/connect_aiocb.cpp
	-mkdir -p ../../../build/yield/aio/net/sockets 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/net/sockets/connect_aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/net/sockets/connect_aiocb.cpp

../../../build/yield/aio/net/sockets/nbio_queue.o: ../../../src/yield/aio/net/sockets/nbio_queue.cpp
	-mkdir -p ../../../build/yield/aio/net/sockets 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/net/sockets/nbio_queue.o -MD $(CXXFLAGS) ../../../src/yield/aio/net/sockets/nbio_queue.cpp

../../../build/yield/aio/net/sockets/posix/accept_aiocb.o: ../../../src/yield/aio/net/sockets/posix/accept_aiocb.cpp
	-mkdir -p ../../../build/yield/aio/net/sockets/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/net/sockets/posix/accept_aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/net/sockets/posix/accept_aiocb.cpp

../../../build/yield/aio/net/sockets/recv_aiocb.o: ../../../src/yield/aio/net/sockets/recv_aiocb.cpp
	-mkdir -p ../../../build/yield/aio/net/sockets 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/net/sockets/recv_aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/net/sockets/recv_aiocb.cpp

../../../build/yield/aio/net/sockets/send_aiocb.o: ../../../src/yield/aio/net/sockets/send_aiocb.cpp
	-mkdir -p ../../../build/yield/aio/net/sockets 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/net/sockets/send_aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/net/sockets/send_aiocb.cpp

../../../build/yield/aio/net/sockets/win32/accept_aiocb.o: ../../../src/yield/aio/net/sockets/win32/accept_aiocb.cpp
	-mkdir -p ../../../build/yield/aio/net/sockets/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/net/sockets/win32/accept_aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/net/sockets/win32/accept_aiocb.cpp

../../../build/yield/aio/net/sockets/win32/aio_queue.o: ../../../src/yield/aio/net/sockets/win32/aio_queue.cpp
	-mkdir -p ../../../build/yield/aio/net/sockets/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/net/sockets/win32/aio_queue.o -MD $(CXXFLAGS) ../../../src/yield/aio/net/sockets/win32/aio_queue.cpp

../../../build/yield/aio/net/sockets/win32/aiocb.o: ../../../src/yield/aio/net/sockets/win32/aiocb.cpp
	-mkdir -p ../../../build/yield/aio/net/sockets/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/net/sockets/win32/aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/net/sockets/win32/aiocb.cpp

../../../build/yield/aio/net/sockets/win32/connect_aiocb.o: ../../../src/yield/aio/net/sockets/win32/connect_aiocb.cpp
	-mkdir -p ../../../build/yield/aio/net/sockets/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/net/sockets/win32/connect_aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/net/sockets/win32/connect_aiocb.cpp

../../../build/yield/aio/net/sockets/win32/recv_aiocb.o: ../../../src/yield/aio/net/sockets/win32/recv_aiocb.cpp
	-mkdir -p ../../../build/yield/aio/net/sockets/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/net/sockets/win32/recv_aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/net/sockets/win32/recv_aiocb.cpp

../../../build/yield/aio/net/sockets/win32/send_aiocb.o: ../../../src/yield/aio/net/sockets/win32/send_aiocb.cpp
	-mkdir -p ../../../build/yield/aio/net/sockets/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/net/sockets/win32/send_aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/net/sockets/win32/send_aiocb.cpp

../../../build/yield/aio/posix/aiocb.o: ../../../src/yield/aio/posix/aiocb.cpp
	-mkdir -p ../../../build/yield/aio/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/posix/aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/posix/aiocb.cpp

../../../build/yield/aio/win32/aio_queue.o: ../../../src/yield/aio/win32/aio_queue.cpp
	-mkdir -p ../../../build/yield/aio/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/win32/aio_queue.o -MD $(CXXFLAGS) ../../../src/yield/aio/win32/aio_queue.cpp

../../../build/yield/aio/win32/aiocb.o: ../../../src/yield/aio/win32/aiocb.cpp
	-mkdir -p ../../../build/yield/aio/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio/win32/aiocb.o -MD $(CXXFLAGS) ../../../src/yield/aio/win32/aiocb.cpp


