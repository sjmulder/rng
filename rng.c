/* rng.c - Copyright (c) 2018 Sijmen J. Mulder, see LICENSE.md */

#include <stdio.h>

int
main(int argc, char **argv)
{
	int start, end=0, line=1, c;

	(void)argc;
	(void)argv;

	if (argc != 2) {
		fputs("usage: rng from[,to] <foo >bar\n", stderr);
		return 1;
	}

	if (sscanf(argv[1], "%d,%d", &start, &end) < 1) {
		fprintf(stderr, "%s: invalid range format\n", *argv);
		return 1;
	}

	while ((c = getchar()) != EOF) {
		if (line >= start && (!end || line <= end))
			putchar(c);
		if (c == '\n')
			line++;
	}

	return 0;
}
