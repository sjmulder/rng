# BSD conventions by default, override to taste
prefix  ?= /usr/local
bindir  ?= $(prefix)/bin
man1dir ?= $(prefix)/man/man1

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
	install -d $(bindir) $(man1dir)
	install rng $(bindir)/
	install rng.1 $(man1dir)/

uninstall:
	rm -f $(bindir)/rng $(man1dir)/rng.1

# only compile tester.c, rng.c is #included
tester: tester.c rng.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o tester tester.c $(LDLIBS)

.PHONY: all test clean install uninstall
