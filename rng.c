/* rng.c - Copyright (c) 2018 Sijmen J. Mulder, see LICENSE.md */

#include <stdio.h>
#include <stdlib.h>	/* exit */
#include <string.h>	/* memchr */
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

static char *
readall(FILE *f, size_t *lenp)
{
	char *buf;
	size_t cap=4096, len=0, nread;

	assert(f);

	if (!(buf = malloc(cap)))
		fatal("malloc failed");

	while ((nread = fread(buf+len, 1, cap-len, stdin)) > 0) {
		len += nread;
		if (len >= cap && !(buf = realloc(buf, cap += 4096)))
			fatal("realloc failed");
	}

	*lenp = len;
	return buf;
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

/*
 * Returns the start the given line (1-based) in the buffer, or NULL if not
 * found.
 */
static char *
find_start(char *buf, int line, size_t len)
{
	char *p;

	assert(buf);
	assert(line > 0);

	p = buf;
	while (--line) {
		if (!(p = memchr(p, '\n', buf+len-p)))
			return NULL;
		if (++p >= buf+len) /* skip \n */
			return NULL;
	}

	return p;
}

/*
 * Returns the end (inclusive) of the given line (1-based) in the buffer, or
 * &buf[len-1] if not found.
 */
static char *
find_end(char *buf, int line, size_t len)
{
	char *p;

	assert(buf);
	assert(line > 0);
	assert(len > 0); /* because of buf[len-1] cap */

	p = buf;
	while (line--) {
		if (++p >= buf+len)
			return buf+len-1; /* skip \n */
		if (!(p = memchr(p, '\n', buf+len-p)))
			return buf+len-1;
	}

	return p;
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
	char *buf, *start, *end;
	size_t len;
	int i, nlines;

	assert(ranges);
	assert(n >= 0);

	buf = readall(stdin, &len);

	for (i=0; i<n; i++) {
		nlines = ranges[i].to - ranges[i].from + 1;
		assert(nlines > 0);

		if (!(start = find_start(buf, ranges[i].from, len)))
			continue;
		if (!(end = find_end(start, nlines, buf+len-start)))
			continue;

		fwrite(start, end-start+1, 1, stdout);
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
