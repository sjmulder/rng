#define TEST
#include "rng.c"

static int nfail;

#define expect_ptr(x, expr) expect_ptr_((x), (expr), __LINE__, #expr)
#define expect_int(x, expr) expect_int_((x), (expr), __LINE__, #expr)

static void
expect_ptr_(void *x, void *val, int line, char *expr)
{
	if (val != x) {
		fprintf(stderr, "%d: %s = %p, expected %p (%+td)\n", line,
		    expr, val, x, (char*)val - (char*)x);
		nfail++;
	}
}

static void
expect_int_(int x, int val, int line, char *expr)
{
	if (val != x) {
		fprintf(stderr, "%d: %s = %d, expected %d (%+d)\n", line,
		    expr, val, x, val - x);
		nfail++;
	}
}

int
main(void)
{
	static char three[] = "one\ntwo\nthree\n";

	struct range range;

	(void)process_singlepass;
	(void)process_buffered;

	expect_int(-1, parse_range("abc",   &range));
	expect_int(-1, parse_range("",      &range));
	expect_int(-1, parse_range("1:2:3", &range));
	expect_int(-1, parse_range("1:0",   &range));
	expect_int(-1, parse_range("0:1",   &range));

	expect_int(0, parse_range("1:2", &range));
	expect_int(1, range.from);
	expect_int(2, range.to);

	expect_int(0, parse_range("1:", &range));
	expect_int(1, range.from);
	expect_int(INT_MAX, range.to);

	expect_int(0, parse_range(":2", &range));
	expect_int(1, range.from);
	expect_int(2, range.to);

	expect_int(0, parse_range(":", &range));
	expect_int(1, range.from);
	expect_int(INT_MAX, range.to);

	expect_int(0, parse_range("-2:-1", &range));
	expect_int(-2, range.from);
	expect_int(-1, range.to);

	expect_ptr(three,   find_start(three, 1, sizeof(three)-1));
	expect_ptr(three+4, find_start(three, 2, sizeof(three)-1));
	expect_ptr(three+8, find_start(three, 3, sizeof(three)-1));
	expect_ptr(NULL,    find_start(three, 4, sizeof(three)-1));
	expect_ptr(three+8, find_start(three, -1, sizeof(three)-1));
	expect_ptr(three+4, find_start(three, -2, sizeof(three)-1));
	expect_ptr(three,   find_start(three, -3, sizeof(three)-1));
	expect_ptr(three,   find_start(three, -4, sizeof(three)-1));
	expect_ptr(three+8, find_start(three, -1, sizeof(three)-2));

	expect_ptr(three+3,  find_end(three, 1, sizeof(three)-1));
	expect_ptr(three+7,  find_end(three, 2, sizeof(three)-1));
	expect_ptr(three+13, find_end(three, 3, sizeof(three)-1));
	expect_ptr(three+13, find_end(three, 4, sizeof(three)-1));
	expect_ptr(three+13, find_end(three, -1, sizeof(three)-1));
	expect_ptr(three+7,  find_end(three, -2, sizeof(three)-1));
	expect_ptr(three+3,  find_end(three, -3, sizeof(three)-1));
	expect_ptr(NULL,     find_end(three, -4, sizeof(three)-1));

	return nfail;
}
