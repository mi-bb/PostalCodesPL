# SPDX-License-Identifier: GPL-3.0-or-later
#
# Builds the postc C example and its Unity test suite. Run from the repo
# root; both binaries open pc_base.db by a path relative to the cwd.

CC       = gcc
CFLAGS   = -std=c11 -Wall
LDLIBS   = -lsqlite3

.PHONY: all test check clean

all: postc

postc: examples/postc.c examples/utf8.c examples/utf8.h
	$(CC) $(CFLAGS) examples/postc.c examples/utf8.c $(LDLIBS) -o $@

tests/test_postc: tests/test_postc.c examples/utf8.c examples/utf8.h tests/unity/unity.c tests/unity/unity.h
	$(CC) $(CFLAGS) -Wno-unused-function \
	    tests/test_postc.c examples/utf8.c tests/unity/unity.c \
	    $(LDLIBS) -o $@

test check: tests/test_postc
	./tests/test_postc

clean:
	rm -f postc tests/test_postc
