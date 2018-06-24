prefix ?= /usr/local
bindir ?= $(prefix)/bin
mandir ?= $(prefix)/man
man1dir ?= $(mandir)/man1

CFLAGS += -Wall -Wextra -pedantic

all: rng tester

test: tester
	./tester

clean:
	rm -f rng tester

install: rng
	install rng $(bindir)/
	install rng.1 $(man1dir)/

uninstall:
	rm -f $(bindir)/rng $(man1dir)/rng.1

# only compile tester.c, rng.c is #included
tester: tester.c rng.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o tester tester.c $(LDLIBS)

.PHONY: all test clean install uninstall
