DESTDIR   ?=
PREFIX    ?= /usr/local
MANPREFIX ?= $(PREFIX)/man

CFLAGS += -ansi -g
CFLAGS += -Wall -Wextra -pedantic
# for memrchr
CFLAGS += -D_GNU_SOURCE

all: rng tester

test: tester
	./tester

clean:
	rm -f rng tester

install: rng
	install -d $(DESTDIR)$(PREFIX)/bin $(DESTDIR)$(MANPREFIX)/man1
	install -m755 rng   $(DESTDIR)$(PREFIX)/bin/
	install -m644 rng.1 $(DESTDIR)$(MANPREFIX)/man1/

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/rng
	rm -f $(DESTDIR)$(MANPREFIX)/man1/rng.1

# only compile tester.c, rng.c is #included
tester: tester.c rng.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o tester tester.c $(LDLIBS)

.PHONY: all test clean install uninstall
