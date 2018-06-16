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

#define PROGNAME "rng"
#define RNG_VERSION "1.0.0"

static const char usage[] =
"Usage: rng [OPTIONS]\n\n"
"OPTIONS:\n\n"
" -h, --help     Show help information.\n"
" -v, --version  Show version number.\n";

int main (int argc, char **argv) {
	int count, index, start, end, opt_value, long_opt_index;
	char *line, *range, *token, *c;
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

	if (argc != 2) {
		fprintf(stderr, "%s: Invalid number of arguments\n", PROGNAME);

		exit(EXIT_FAILURE);
	}

	index = 0;
	start = 0;
	end = 0;

	range = argv[1];

	while ((token = strsep(&range, ","))) {
		for (c = token; *c; c++) {
			if (*c < '0' || *c > '9') {
				fprintf(stderr, "%s: '%s' is not a valid integer\n\n", PROGNAME, token);
				fputs(usage, stderr);
				exit(EXIT_FAILURE);
			}
		}

		switch (index) {
			case 0:
				start = atoi(token);

				break;
			case 1:
				end = atoi(token);

				break;
			default:
				break;
		}

		index++;
	}

	count = 1;

	while ((read = getline(&line, &len, stdin)) != -1) {
		if (count >= start && (count <= end || !end)) {
			fwrite(line, read, 1, stdout);
		}

		count++;
	}

	return EXIT_SUCCESS;
}
