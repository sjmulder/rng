/**
 * rng.c
 *
 * Copyright (C) 2018 Nickolas Burr <nickolasburr@gmail.com>
 * Copyright (C) 2018 Sijmen J. Mulder <ik@sjmulder.nl>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

int
main(int argc, char **argv) {
	int count, start, end;
	char *line;
	size_t len;
	ssize_t read;

	(void)argc;
	(void)argv;

	if (argc != 2) {
		fputs("usage: rng from[,to] <foo >bar\n", stderr);
		return 1;
	}

	if (sscanf(argv[1], "%d,%d", &start, &end) != 2)
		errx(1, "invalid range format");

	count = 1;

	while ((read = getline(&line, &len, stdin)) != -1) {
		if (count >= start && (count <= end || !end)) {
			fwrite(line, read, 1, stdout);
		}

		count++;
	}

	return EXIT_SUCCESS;
}
