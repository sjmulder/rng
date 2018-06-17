# rng

Copy a range of lines from standard input to standard output.

## Synopsis

**rng** _from_[,_to_]

## Description

**rng** copies lines _from_ through _to_ (inclusive) from standard input to
standard output. Lines start at number 1.

## Examples

Get lines 7-15 in a file:

    $ rng 7,15 <file.txt

Skip the first 4 lines of input.

    $ echo -e "First\nSecond\nThird\nFourth\nFifth\n" | rng 5

## Building

    make

To install or uninstall:

    make install
    make uninstall

## Authors

Originally by Nickolas Burr (<nickolasburr@gmail.com>). Rewritten by
Sijmen J. Mulder (<ik@sjmulder.nl>).
