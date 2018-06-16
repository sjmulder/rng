prefix ?= /usr/local
bindir ?= $(prefix)/bin

CFLAGS += -Wall -Wextra -pedantic -Iinclude/

all: rng

clean:
	rm -f rng

install: rng
	install -c rng $(bindir)/

uninstall:
	rm -f $(bindir)/rng

.PHONY: all clean install uninstall
