/* rng.c - Copyright (c) 2018 Sijmen J. Mulder, see LICENSE.md */

#include <stdio.h>
#include <stdlib.h>	/* exit */
#include <stdarg.h>
#include <assert.h>
#include <limits.h>

#define LEN(a) (sizeof(a)/sizeof(*a))

struct range {
	int from, to;
};

static void
fatal(const char *fmt, ...)
{
	va_list ap;

	assert(fmt);

	va_start(ap, fmt);
	fputs("rng: ", stderr);
	vfprintf(stderr, fmt, ap);
	fputs("\n", stderr);
	va_end(ap);
	exit(1);
}

static int
parse_range(char *s, struct range *range)
{
	char *endptr;

	assert(s);
	assert(range);

	if (*s == ':') {
		range->from = 1;
		s++;
	} else {
		range->from = (int)strtol(s, &endptr, 10);
		if (*endptr != ':' || range->from < 1)
			return -1;
		s = endptr + 1;
	}

	if (!*s)
		range->to = INT_MAX;
	else {
		range->to = (int)strtol(s, &endptr, 10);
		if (*endptr || range->to < 1)
			return -1;
	}

	return 0;
}

int
main(int argc, char **argv)
{
	struct range ranges[64];
	int n=0, i=0, line=1, c;

	if (argc < 2) {
		fputs("usage: rng [from]:[to] ...\n", stderr);
		return 1;
	}

	if ((n = argc-1) > (int)LEN(ranges))
		fatal("too many ranges");

	for (i = 0; i<n; i++) {
		if (parse_range(argv[i+1], &ranges[i]) == -1)
			fatal("invalid range format: '%s'", argv[i+1]);
		if (ranges[i].from > ranges[i].to)
			fatal("end before start: '%s'", argv[1+1]);
		if (i && ranges[i-1].from >= ranges[i].to)
			fatal("range '%s' precedes '%s'", argv[i], argv[i+1]);
		if (i && ranges[i-1].to >= ranges[i].from)
			fatal("range '%s' overlaps '%s'", argv[i], argv[i+1]);
	}

	i = 0;
	while ((c = getchar()) != EOF) {
		while (i<n && line > ranges[i].to)
			i++;
		if (i<n && line >= ranges[i].from)
			putchar(c);
		if (c == '\n')
			line++;
	}

	return 0;
}
