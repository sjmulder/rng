prefix ?= /usr/local
bindir ?= $(prefix)/bin
mandir ?= $(prefix)/man
man1dir ?= $(mandir)/man1

CFLAGS += -Wall -Wextra -pedantic

all: rng

clean:
	rm -f rng

install: rng
	install rng $(bindir)/
	install rng.1 $(man1dir)/

uninstall:
	rm -f $(bindir)/rng $(man1dir)/rng.1

.PHONY: all clean install uninstall
