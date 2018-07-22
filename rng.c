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

/* macOS does not provide memrchr */
#if defined(__APPLE__)
static void *
memrchr(const void *s, int c, size_t n)
{
	const unsigned char *p;

	while (n) {
		p = (const unsigned char *)s + --n;
		if (*p == c)
			return (void *)p;
	}

	return NULL;
}
#endif

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
 * Parses a line number range in one of the following forms:
 *   "from:to"  _from_ to _to_
 *   "from:"    _from_ to end (.to = INT_MAX)
 *   ":to"      start to _to_ (.from = 1)
 *   ":"        start to end  (.from = 1, .to = INT_MAX)
 *   "line"     single line (.from = line, .to = line)
 *
 * Line numbers are 1-based. Negative line numbers are relative to the end of
 * the file, with -1 being the last line and -2 the next-to-last. 0 is not a
 * valid value.
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
		if (range->from == 0)
			return -1;
		if (!*endptr) {
			range->to = range->from;
			return 0;
		}
		if (*endptr != ':')
			return -1;
		s = endptr + 1;
	}

	if (!*s)
		range->to = INT_MAX;
	else {
		range->to = (int)strtol(s, &endptr, 10);
		if (*endptr || range->to == 0)
			return -1;
	}

	return 0;
}

/*
 * Returns a pointer to the first character on the given line, searching
 * forward if >0 or backward from the end if <0. Line 1 is the first, -1 the
 * last.
 *
 * If not found, forward searches return NULL, and backward searches `buf`.
 */
static char *
find_start(char *buf, int line, size_t len)
{
	char *p;

	assert(buf);
	assert(line != 0);
	assert(len > 0); /* because of buf[len-1] cap */

	if (line > 0) {
		p = buf;
		while (--line) {
			if (!(p = memchr(p, '\n', buf+len-p)))
				return NULL;
			if (++p >= buf+len) /* skip \n */
				return NULL;
		}
	} else {
		p = buf+len-1;
		if (*p == '\n')
			line--; /* compensate for trailing \n */
		while (line++) {
			if (!(p = memrchr(buf, '\n', p-buf+1)))
				return buf;
			if (--p < buf) /* skip \n */
				return buf;
		}
		/* p points to the char before \n, we need the char after */
		if ((p+=2) >= buf+len)
			return NULL;
	}

	return p;
}

/*
 * Returns a character to the last character on the given line, searching
 * forward if >0 or backward from the end if <0. Line 1 is the first, -1 the
 * last.
 *
 * If not found, forward searches return buf+len-1, and backward searches
 * NULL.
 */
static char *
find_end(char *buf, int line, size_t len)
{
	char *p;

	assert(buf);
	assert(line != 0);
	assert(len > 0); /* because of buf[len-1] cap */

	if (line > 0) {
		p = buf;
		while (line--) {
			if (++p >= buf+len)
				return buf+len-1; /* skip \n */
			if (!(p = memchr(p, '\n', buf+len-p)))
				return buf+len-1;
		}
	} else {
		p = buf+len-1;
		if (*p == '\n')
			line--; /* compensate for trailing \n */
		while (1) {
			if (!(p = memrchr(buf, '\n', p-buf+1)))
				return NULL;
			if (++line >= -1) /* check before --p */
				break;
			if (--p < buf) /* skip \n */
				return NULL;
		}
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
	int i;

	assert(ranges);
	assert(n >= 0);

	buf = readall(stdin, &len);

	for (i=0; i<n; i++) {
		if (!(start = find_start(buf, ranges[i].from, len)))
			continue;
		if (!(end = find_end(buf, ranges[i].to, len)))
			continue;
		if (end >= start)
			fwrite(start, end-start+1, 1, stdout);
	}
}

#ifndef TEST
int
main(int argc, char **argv)
{
	struct range ranges[64];
	int n=0, i=0, singlepass=1;

	if (argc < 2) {
		fputs("usage: rng range... (see `man rng`)\n", stderr);
		return 1;
	}

	if ((n = argc-1) > (int)LEN(ranges))
		fatal("too many ranges");

	for (i = 0; i<n; i++) {
		if (parse_range(argv[i+1], &ranges[i]) == -1)
			fatal("invalid range format: '%s'", argv[i+1]);
		if (singlepass) {
			if (ranges[i].from < 0 || ranges[i].to < 0)
				singlepass = 0;
			else if (i && ranges[i-1].to >= ranges[i].from)
				singlepass = 0;
		}

	}

	if (singlepass)
		process_singlepass(ranges, n);
	else
		process_buffered(ranges, n);

	return 0;
}
#endif /* TEST */
