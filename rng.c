/**
 * rng.c
 *
 * Copyright (C) 2018 Nickolas Burr <nickolasburr@gmail.com>
 * Copyright (C) 2018 Sijmen J. Mulder <ik@sjmulder.nl>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <err.h>

#define PROGNAME "rng"
#define RNG_VERSION "1.0.0"

static const char usage[] =
"Usage: rng [OPTIONS]\n\n"
"OPTIONS:\n\n"
" -h, --help     Show help information.\n"
" -v, --version  Show version number.\n";

int main (int argc, char **argv) {
	int count, start, end, opt_value, long_opt_index;
	char *line;
	size_t len;
	ssize_t read;

	long_opt_index = 0;

	static struct option long_options[] = {
		{ "help", no_argument, 0, 'h' },
		{ "version", no_argument, 0, 'v' },
	};

	while ((opt_value = getopt_long(argc, argv, "hv", long_options, &long_opt_index)) != -1) {
		switch (opt_value) {
			case 'h':
				fputs(usage, stderr);
				exit(EXIT_SUCCESS);
			case 'v':
				fprintf(stdout, "%s\n", RNG_VERSION);
				exit(EXIT_SUCCESS);
			case '?':
				fputs(usage, stderr);
				exit(EXIT_FAILURE);
		}
	}

	if (argc != 2)
		errx(1, "invalid number of arguments");
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
