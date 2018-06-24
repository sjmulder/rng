# rng

Copy ranges of lines from standard input to standard output.

## Synopsis

**rng** [_from_]:[_to_] ...

## Description

**rng** copies the specified line ranges from standard input to standard
output. Ranges can be specified as follows:

|-------------|-------------------------------------------|
| _from_:_to_ | Line _from_ through _to_ (inclusive).     |
| _from_:     | Line _from_ to end of input.              |
| :_to_       | Start of input to line _end_ (inclusive). |
| :           | The entire input.                         |
| line        | Line _line_ only.                         |


Line numbers go up from 1 relative to the start of input, and down
from -1 relative to the end of input.

Use of negative line numbers, overlapping ranges, or backtring ranges causes
the file to be buffered in its entirety.

## Examples

Output lines 2-5 of a file:

    rng 2:5 <file.txt

Output the first and last three lines of a file:

    rng :3 -3: <file.txt

Output a file twice:

    rng : : <file.txt

## Building

Unix-likes:

    make

    # optionally
    make test
    make install
    make uninstall

Visual C++, from the Developer Command Prompt:

    nmake /f Makefile.msvc

## Authors

Originally by Nickolas Burr (<nickolasburr@gmail.com>). Rewritten by
Sijmen J. Mulder (<ik@sjmulder.nl>).
