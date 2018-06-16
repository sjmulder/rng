/**
 * rng.c
 *
 * Copyright (C) 2018 Nickolas Burr <nickolasburr@gmail.com>
 * Copyright (C) 2018 Sijmen J. Mulder <ik@sjmulder.nl>
 */

#include <stdio.h>
#include <err.h>

int
main(int argc, char **argv)
{
	int start, end, line=1, c;

	(void)argc;
	(void)argv;

	if (argc != 2) {
		fputs("usage: rng from[,to] <foo >bar\n", stderr);
		return 1;
	}

	if (sscanf(argv[1], "%d,%d", &start, &end) != 2)
		errx(1, "invalid range format");

	while ((c = getchar()) != EOF) {
		if (line >= start && (!end || line <= end))
			putchar(c);
		if (c == '\n')
			line++;
	}

	return 0;
}
