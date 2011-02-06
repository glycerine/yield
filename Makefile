all: aio aio_test common common_test fs fs_test http http_test i18n i18n_test marshal marshal_test net net_test poll poll_test process process_test stage stage_test thread thread_test

clean:
	$(MAKE) -C proj/yield/aio -f aio.Makefile clean
	$(MAKE) -C proj/yield/aio -f aio_test.Makefile clean
	$(MAKE) -C proj/yield/common -f common.Makefile clean
	$(MAKE) -C proj/yield/common -f common_test.Makefile clean
	$(MAKE) -C proj/yield/fs -f fs.Makefile clean
	$(MAKE) -C proj/yield/fs -f fs_test.Makefile clean
	$(MAKE) -C proj/yield/http -f http.Makefile clean
	$(MAKE) -C proj/yield/http -f http_test.Makefile clean
	$(MAKE) -C proj/yield/i18n -f i18n.Makefile clean
	$(MAKE) -C proj/yield/i18n -f i18n_test.Makefile clean
	$(MAKE) -C proj/yield/marshal -f marshal.Makefile clean
	$(MAKE) -C proj/yield/marshal -f marshal_test.Makefile clean
	$(MAKE) -C proj/yield/net -f net.Makefile clean
	$(MAKE) -C proj/yield/net -f net_test.Makefile clean
	$(MAKE) -C proj/yield/poll -f poll.Makefile clean
	$(MAKE) -C proj/yield/poll -f poll_test.Makefile clean
	$(MAKE) -C proj/yield/process -f process.Makefile clean
	$(MAKE) -C proj/yield/process -f process_test.Makefile clean
	$(MAKE) -C proj/yield/stage -f stage.Makefile clean
	$(MAKE) -C proj/yield/stage -f stage_test.Makefile clean
	$(MAKE) -C proj/yield/thread -f thread.Makefile clean
	$(MAKE) -C proj/yield/thread -f thread_test.Makefile clean

depclean:
	$(MAKE) -C proj/yield/aio -f aio.Makefile depclean
	$(MAKE) -C proj/yield/aio -f aio_test.Makefile depclean
	$(MAKE) -C proj/yield/common -f common.Makefile depclean
	$(MAKE) -C proj/yield/common -f common_test.Makefile depclean
	$(MAKE) -C proj/yield/fs -f fs.Makefile depclean
	$(MAKE) -C proj/yield/fs -f fs_test.Makefile depclean
	$(MAKE) -C proj/yield/http -f http.Makefile depclean
	$(MAKE) -C proj/yield/http -f http_test.Makefile depclean
	$(MAKE) -C proj/yield/i18n -f i18n.Makefile depclean
	$(MAKE) -C proj/yield/i18n -f i18n_test.Makefile depclean
	$(MAKE) -C proj/yield/marshal -f marshal.Makefile depclean
	$(MAKE) -C proj/yield/marshal -f marshal_test.Makefile depclean
	$(MAKE) -C proj/yield/net -f net.Makefile depclean
	$(MAKE) -C proj/yield/net -f net_test.Makefile depclean
	$(MAKE) -C proj/yield/poll -f poll.Makefile depclean
	$(MAKE) -C proj/yield/poll -f poll_test.Makefile depclean
	$(MAKE) -C proj/yield/process -f process.Makefile depclean
	$(MAKE) -C proj/yield/process -f process_test.Makefile depclean
	$(MAKE) -C proj/yield/stage -f stage.Makefile depclean
	$(MAKE) -C proj/yield/stage -f stage_test.Makefile depclean
	$(MAKE) -C proj/yield/thread -f thread.Makefile depclean
	$(MAKE) -C proj/yield/thread -f thread_test.Makefile depclean


aio: fs net poll stage
	$(MAKE) -C proj/yield/aio -f aio.Makefile

aio_test: aio
	$(MAKE) -C proj/yield/aio -f aio_test.Makefile

common: 
	$(MAKE) -C proj/yield/common -f common.Makefile

common_test: common
	$(MAKE) -C proj/yield/common -f common_test.Makefile

fs: i18n
	$(MAKE) -C proj/yield/fs -f fs.Makefile

fs_test: fs
	$(MAKE) -C proj/yield/fs -f fs_test.Makefile

http: aio stage
	$(MAKE) -C proj/yield/http -f http.Makefile

http_test: http
	$(MAKE) -C proj/yield/http -f http_test.Makefile

i18n: common
	$(MAKE) -C proj/yield/i18n -f i18n.Makefile

i18n_test: i18n
	$(MAKE) -C proj/yield/i18n -f i18n_test.Makefile

marshal: common
	$(MAKE) -C proj/yield/marshal -f marshal.Makefile

marshal_test: marshal
	$(MAKE) -C proj/yield/marshal -f marshal_test.Makefile

net: common
	$(MAKE) -C proj/yield/net -f net.Makefile

net_test: net
	$(MAKE) -C proj/yield/net -f net_test.Makefile

poll: net thread
	$(MAKE) -C proj/yield/poll -f poll.Makefile

poll_test: poll
	$(MAKE) -C proj/yield/poll -f poll_test.Makefile

process: fs
	$(MAKE) -C proj/yield/process -f process.Makefile

process_test: process
	$(MAKE) -C proj/yield/process -f process_test.Makefile

stage: thread
	$(MAKE) -C proj/yield/stage -f stage.Makefile

stage_test: stage
	$(MAKE) -C proj/yield/stage -f stage_test.Makefile

thread: common
	$(MAKE) -C proj/yield/thread -f thread.Makefile

thread_test: thread
	$(MAKE) -C proj/yield/thread -f thread_test.Makefile

