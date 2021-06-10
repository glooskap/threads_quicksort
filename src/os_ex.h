#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

typedef struct threaddata {
	int thread_id;
	int start;
	int end;
}THREAD_DATA;

pthread_mutex_t mutx;
pthread_rwlock_t rwlock;
int* numbers;
int numbers_count;
int synch_type;

/**
	finds until which point
	2 strings are equal
	and returns the int value of the remaining characters
	of the 1st arguement
*/
int ArgCheck(char*, char*);

/**
	prints the array from
	1st arguement until the second
*/
void printA(int, int);

/**
	sorts the array from
	1st arguement until the second
	with recursive quicksort algorithm
*/
void quicksort(int, int);

/**
	thread function with input a THREAD_DATA struct
	calls quicksort
*/
void *sort(void*);

/**
	merges 2 subarrays
*/
void merge(int, int, int);