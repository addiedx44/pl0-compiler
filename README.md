PL/0 Compiler
=============
Written by Adam Dunson
Last updated on 2012-04-23

[PL/0 Wikipedia page](http://en.wikipedia.org/wiki/PL/0)

This was written as a class project in college. User's Manual located
[here](https://raw.github.com/adamdunson/pl0-compiler/master/doc/PL0%20User%27s%20Manual.pdf).

This README.markdown is written in Markdown and contains instructions for the Linux
operating system.


Compilation Instructions (GCC)
------------------------------
### Prerequisites
You will need the `make` utility.

### Instructions
From the project directory (where `Makefile` is located), run the following:

    make

This will create binary `.o` files in the `obj/` directory. The final
executable will be located in `bin/` and is called `pl0-compiler`.

To clean up `.o` and executable files, run the following:

    make clean    # rm -f obj/*.o
    make spotless # make clean; rm -f bin/*


Execution Instruction
---------------------
You will need to provide an input file written in PL/0. Some examples are
located in the `sample/` directory.

To run the program after compiling, use the following:

    bin/pl0-compiler /path/to/input_file

or just:

    bin/pl0-compiler input_file

if the input file is in the current directory.

### Command-line options
For more output, three command line flags are available.

    bin/pl0-compiler -l -a -v input_file

or just:

    bin/pl0-compiler -lav input_file

(or any combination thereof).

- -l (ell) displays both a raw and a pretty token file from the scanner
- -a displays the generated code in both a raw and a pretty format
- -v displays the stack trace for the virtual machine as it executes


Other Notes
-----------
You can redirect output to a file by using the following syntax:

    bin/pl0-compiler input_file > output_file
