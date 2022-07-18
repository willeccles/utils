# Will Eccles' Utils

This is a collection of small utility programs I have written to make my daily
life easier. You may or may not find any of them useful. This repository is
likely a huge mess, but that is irrelevant.

## Building

Use GNU Make or something compatible. Otherwise, use a compiler manually for any
particular program. Use `make install' to install the programs. They will be
placed in $DESTDIR/$PREFIX/bin (default is /usr/local/bin).

## Usage

Run a program without any arguments for details. It's not a guarantee that it
will have useful information, and it's certainly not a guarantee that you will
find the programs useful, but you can always try.

## Programs

At this time, the following programs are included in this repository:
 - hex2float: Converts a 32-bit hex value to a 32-bit float.
 - symgrab:   Reads a symbol from an object file in various formats.
 - jedcrc:    Calculates the checksum of the transmission data in a JEDEC .jed
              file.
 - resdiv:    Calculates resistive divider values.
