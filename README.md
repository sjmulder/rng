# rng

Copy a range of lines from standard input to standard output.

## Synopsis

**rng** [_from_]:[_to_] ...

## Description

**rng** copies the specified line ranges from standard input to standard
output. Ranges are specified as inclusive start and end line numbers. The
first line is numbered 1.

Ranges must not overlap or backtrack, that is to say, any range's _from_ must
be greater than the previous range's _to_.

## Examples

Get the lines 2-5 and 10-end:

    $ rng 2:5 10: <file.txt

## Building

Unix-likes:

    make

    # optionally
    make install
    make uninstall

Visual C++, from the Developer Command Prompt:

    nmake /f Makefile.msvc

## Authors

Originally by Nickolas Burr (<nickolasburr@gmail.com>). Rewritten by
Sijmen J. Mulder (<ik@sjmulder.nl>).
