# multithread quicksort
project for Operating Systems course

generate random numbers in an array and then apply the quicksort algorithm in multiple threads.

using pthread mutex

## usage
compile with pthread
```C
gcc -pthread os_ex.c -o psort
```
run with parameters, eg.
```C
./psort -numbers=100 -threads=4 -seed=168 -mode=2
```
