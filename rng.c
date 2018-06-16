/**
 * rng.c
 *
 * Copyright (C) 2018 Nickolas Burr <nickolasburr@gmail.com>
 * Copyright (C) 2018 Sijmen J. Mulder <ik@sjmulder.nl>
 */

#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef __APPLE__
#include <limits.h>
#else
#include <linux/limits.h>
#endif

#define is_null(ptr) ((ptr) == ((void *) 0))

#define PROGNAME "rng"
#define RNG_VERSION "1.0.0"

#define _GNU_SOURCE
#define NULL_DEVICE "/dev/null"
#define NULL_BYTE 1

#define NUM_OPTIONS 2
#define OPT_DELIMIT "--"

typedef struct {
	char *value;
	char *alias;
	char *desc;
} option_t;

static option_t options[] = {
	{
		"--help",
		"-h",
		"Show help information.",
	},
	{
		"--version",
		"-v",
		"Show version number.",
	},
};

/**
 * Print usage information.
 */
static void usage (void) {
	int index;
	char fvalue[36];

	fprintf(stdout, "Usage: rng [OPTIONS]\n\n");
	fprintf(stdout, "OPTIONS:\n\n");

	for (index = 0; index < NUM_OPTIONS; index += 1) {
		option_t *option = &options[index];

		/**
		 * Format option->value string.
		 */
		snprintf(fvalue, sizeof(fvalue), "%s,", option->value);
		fprintf(stdout, "%4s%-22s %s: %-24s\n", "", fvalue, option->alias, option->desc);
	}
}

/**
 * Usage:
 *
 * rng 5,35 < filename
 * cat filename | rng 5,35
 */

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
				usage();

				exit(EXIT_SUCCESS);
			case 'v':
				fprintf(stdout, "%s\n", RNG_VERSION);

				exit(EXIT_SUCCESS);
			case '?':
				usage();

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
				usage();
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
