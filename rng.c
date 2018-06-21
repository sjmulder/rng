/* rng.c - Copyright (c) 2018 Sijmen J. Mulder, see LICENSE.md */

#include <stdio.h>
#include <stdlib.h>	/* exit */
#include <stdarg.h>
#include <assert.h>
#include <limits.h>

#define LEN(a) (sizeof(a)/sizeof(*a))

struct range {
	int from, to;
	char *fromptr, *toptr; /* used by process_buffered() */
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

/*
 * Parses a range in the form [from],[to] where `from` and `to` are both
 * positive numbers that fit an int. Either may be omitted, in which case they
 * are assigned 1 and INT_MAX respectively.
 *
 * Returns 0 on success, or -1 on failure.
 */
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

static void
process_singlepass(struct range *ranges, int n)
{
	int i=0, line=1, c;

	assert(ranges);

	while ((c = getchar()) != EOF) {
		if (line >= ranges[i].from)
			putchar(c);
		if (c == '\n' && ++line > ranges[i].to && ++i >= n)
			break;
	}
}

static void
process_buffered(struct range *ranges, int n)
{
	char *buf;
	size_t cap=4096, len=0, nread, pos;
	int i, line=1, newline=1;

	assert(ranges);

	if (!(buf = malloc(cap)))
		fatal("malloc failed");

	/* read the entire file first */
	while ((nread = fread(buf+len, 1, cap-len, stdin)) > 0) {
		len += nread;
		if (len >= cap && !(buf = realloc(buf, cap += 4096)))
			fatal("realloc failed");
	}

	for (i=0; i<n; i++)
		ranges[i].fromptr = ranges[i].toptr = NULL;

	/* assign all range boundary pointers in one go */
	for (pos=0; pos<len; pos++) {
		if (newline) {
			for (i=0; i<n; i++) {
				if (ranges[i].from == line)
					ranges[i].fromptr = buf+pos;
				if (ranges[i].to == line-1)
					ranges[i].toptr = buf+pos;
			}
			line++;
		}

		newline = buf[pos] == '\n';
	}

	/* now it's a matter of blitting bytes */
	for (i=0; i<n; i++) {
		if (!ranges[i].fromptr) /* `from` is past EOF, so skip */
			continue;
		if (!ranges[i].toptr) /* `end` is past EOF */
			ranges[i].toptr = buf+len;

		fwrite(ranges[i].fromptr, 1, ranges[i].toptr -
		    ranges[i].fromptr, stdout);
	}
}

int
main(int argc, char **argv)
{
	struct range ranges[64];
	int n=0, i=0, singlepass=1;

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
		if (i && ranges[i-1].to >= ranges[i].from)
			singlepass = 0;
	}

	if (singlepass)
		process_singlepass(ranges, n);
	else
		process_buffered(ranges, n);

	return 0;
}
