# multithread quicksort
project for Operating Systems course

generate random numbers in an array and then apply the quicksort algorithm in multiple threads.

uses pthread mutex

creates results.dat file

## usage
compile with pthread
```C
gcc -pthread os_ex.c -o psort
```
run with parameters, eg.
```C
./psort -numbers=100 -threads=4 -seed=168 -mode=2
```
* numbers: random numbers to generate
* threads: threads to use
* seed: rng seed for srand
* mode: thread synch type
  * 1: total exclusion
  * 2: mild exclusion
  * 3: no exclusion
