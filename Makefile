# Output binary name
bin=crash

# Set the following to '0' to disable log messages:
LOGGER ?= 1

# Compiler/linker flags
CFLAGS += -g -Wall -fPIC -DLOGGER=$(LOGGER)
LDLIBS += -lm -lreadline
LDFLAGS +=

src=history.c shell.c ui.c jobs.c
obj=$(src:.c=.o)

all: $(bin) libshell.so

$(bin): $(obj)
	$(CC) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(obj) -o $@

libshell.so: $(obj)
	$(CC) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(obj) -shared -o $@

shell.o: shell.c history.h logger.h ui.h jobs.h
history.o: history.c history.h logger.h jobs.h
ui.o: ui.h ui.c logger.h history.h jobs.h
jobs.o: jobs.c jobs.h history.h logger.h ui.h

clean:
	rm -f $(bin) $(obj) libshell.so vgcore.*


# Tests --

test: $(bin) libshell.so ./tests/run_tests
	@DEBUG="$(debug)" ./tests/run_tests $(run)

testupdate: testclean test

./tests/run_tests:
	rm -rf tests
	git clone https://github.com/usf-cs521-sp21/P2-Tests.git tests

testclean:
	rm -rf tests