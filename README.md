# rng

Copy a range of lines from standard input to standard output.

## Synopsis

**rng** [_from_]:[_to_] ...

## Description

**rng** copies the specified line ranges from standard input to standard
output. Ranges are specified as inclusive start and end line numbers. The
first line is numbered 1.

Ranges may overlap or backtrack, but this will cause the input to be buffered
in its entirety.

## Examples

Output lines 10-end and 2-5 of a file:

    $ rng 10: 2:5 <file.txt

Output a file twice:

    $ rng : : <file.txt

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
