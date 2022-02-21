# multithread quicksort
Operating Systems course assignment

1. generate random numbers and store them in a single array
2. divide into subarrays and create a thread for each
3. apply quicksort on each subarray and then merge results

- uses pthread mutex
- creates results.dat file containing initial and sorted array

## usage
compile with pthread
```C
gcc -pthread os_ex.c -o psort
```
run with parameters, eg.
```C
./psort -numbers=10000 -threads=4 -seed=168 -mode=2
```
* numbers: random numbers to generate
* threads: threads to use
* seed: rng seed for srand
* mode: thread synch type
  * 1: total exclusion (read and write)
  * 2: mild exclusion (write only)
  * 3: no exclusion
