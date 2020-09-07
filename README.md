# Simple Quadratic Sieve for big numbers in C
This is simple implementation of the single-polynomial version of
[Quadratic Sieve](https://en.wikipedia.org/wiki/Quadratic_sieve)
using [GMP library](https://gmplib.org/). Project has been improved
with script in **Python** that allows to factor **multiple numbers in
parallel** and output results to **CSV file**.

To run just C program you will need to compile it with:
```bash 
gcc main.c list.c -lm -lgmp -o quadratic-sieve
```

and then execute it with
```bash
./quadratic-sieve [number] [arguments...]
```

With ```arguments``` you can modify sieve interval as well as modify
bound of base. See ```./quadratic-sieve --help``` for exact command.

Alternatively you can execute it with **Python**:
```bash
python3 sieve.py [numbers...]
```
Script will compile C program and run with all inputted numbers.
Results will be then shown on screen and saved to CSV file.
In configs folder there are configuration files which allow you to
control logging, CSV, parameters passed to C as well as numbers
factored.

Minimum requirement to run C program is:
* GMP library
* C compiler

To use Python script it's necessary to have:
* Linux
* Python
* GCC

Additionally script uses:
* CMake
* Make