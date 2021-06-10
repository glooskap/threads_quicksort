#include "os_ex.h"

int ArgCheck(char* arg, char* pref) {

	size_t a_length = strlen(arg);
	size_t s_length = strlen(pref);

	if( a_length <= s_length ) {
		return -1;
	}

	char* suff = malloc(sizeof(char)*10);

	int i = 0;
	while( arg[i] == pref[i]) { ++i; }

	if( i != s_length) {
		return -1;
	}

	int j = 0;
	while( i < a_length) {
		suff[j] = arg[i];
		++j;
		++i;
	}

	int s = atoi(suff);
	free(suff);

	return s;

}

void printA(int start, int end) {
	printf("\n[|");
	int i;
	for(i=start; i<end; i++) {
		printf(" %d |", numbers[i]);
	}
	printf("]\n");
}

void quicksort(int left, int right) {

	int i = left, j = right;
	int tmp;

	if (synch_type == 1) pthread_mutex_lock(&mutx);
	int pivot = numbers[(left+right)/2];
	if (synch_type == 1) pthread_mutex_unlock(&mutx);

	while( i <= j) {
		if (synch_type == 1) pthread_mutex_lock(&mutx);
		while( numbers[i] < pivot)
			++i;
		while( numbers[j] > pivot)
			--j;
		if (synch_type == 1) pthread_mutex_unlock(&mutx);
		if( i <= j) {

			tmp = numbers[i];
			if (synch_type == 1 || synch_type == 2) pthread_mutex_lock(&mutx);
			numbers[i] = numbers[j];
			numbers[j] = tmp;
			if (synch_type == 1 || synch_type == 2) pthread_mutex_unlock(&mutx);
			++i;
			--j;
		}
	}

	if( left < j)
		quicksort(left, j);
	if( i < right)
		quicksort(i, right);
}

void *sort(void* t) {

	THREAD_DATA* strc = (THREAD_DATA*) t;

	printf("Started thread %d for cells %d to %d\n", strc->thread_id, strc->start, strc->end);

	quicksort(strc->start, strc->end);

	pthread_exit(NULL);

}

void merge(int l, int m, int r) {
	// l=start of 1st subarray
	// m=end of 1st subarray
	// r=end of 2nd subarray

    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;

    // temp arrays
    int L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = numbers[l + i];
    for (j = 0; j < n2; j++)
        R[j] = numbers[m + 1+ j];

   // Merge the temp arrays back into initial 
    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            numbers[k] = L[i];
            i++;
        } else {
            numbers[k] = R[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of L
    while (i < n1) {
        numbers[k] = L[i];
        i++;
        k++;
    }

    // Copy the remaining elements of R
    while (j < n2) {
        numbers[k] = R[j];
        j++;
        k++;
    }
}


int main(int argc, char *argv[]) {

	/*
	arg0 = program name
	arg1 = numbers to sort   / -numbers=X
	arg2 = threads to create / -threads=X
	arg3 = rng seed			 / -seed=X
	arg4 = synch type		 / -mode=X
	*/

	struct timeval start_time, end_time;

	if (gettimeofday(&start_time, NULL) == -1) {
		printf("ERROR: gettimeofday failed!\n");
		exit(-1);
	}


	if( argc != 5) {
		printf("ERROR: the program should take four arguments!\n");
		exit(-1);
	}

	numbers_count = ArgCheck(argv[1], "-numbers=");
	if( numbers_count <= 0) {
		printf("ERROR: amount of numbers to sort is not a natural number!\n");
		exit(-1);
	}

	int threads_count = ArgCheck(argv[2], "-threads=");
	if( !(threads_count == 1 || threads_count == 2 || threads_count == 4 || threads_count == 8)) {
		printf("ERROR: amount of threads to create should be 1||2||4||8 !\n");
		exit(-1);
	}

	int seed = ArgCheck(argv[3], "-seed=");
	if( seed <= 0){
		printf("ERROR: rng seed is not a natural number!\n");
		exit(-1);
	}

	synch_type = ArgCheck(argv[4], "-mode=");
	if( !(synch_type == 1 || synch_type == 2 || synch_type == 3)) {
		printf("ERROR: synch mode should be 1||2||3 !\n");
		exit(-1);
	}



	//array initialisation
	srand(seed);
	numbers = (int *) malloc(sizeof(int) * numbers_count);

	if( numbers == NULL) {
		printf("ERROR: Malloc failed not enough memory!\n");
		return -1;
	}

	//generate rand numbers and insert to array
	int i;
	for( i = 0; i < numbers_count; i++) {
		numbers[i] = rand() % numbers_count;
	}



	FILE *f = fopen("results.dat", "wb");

	fprintf(f, "Initial array:\n[|");
	for( i=0; i<numbers_count; i++) {
		fprintf(f, " %d |", numbers[i]);
	}
	fprintf(f, "]\n");




	//threads creation
	int rc ;
	pthread_t threads[threads_count];
	THREAD_DATA structs[threads_count];

	//initialise mutex.
	rc = pthread_mutex_init(&mutx, NULL);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_init() is %d\n", rc);
		exit(-1);
	}

	int size, extra;
	size = numbers_count/threads_count;
	extra = numbers_count%threads_count;

	printf("\n");

	for (i = 0; i < threads_count-1; i++) {

		structs[i].thread_id = i+1;
  		structs[i].start = i*size;
  		structs[i].end = structs[i].start + size - 1;

    	rc = pthread_create(&threads[i], NULL, sort, &structs[i]);

    	if (rc != 0) {
    		printf("ERROR: return code from pthread_create() is %d\n", rc);
       		exit(-1);
       	}
	}

	/*
		the last thread in case extra>0
		or when only one thread is used
	*/
	structs[i].thread_id = i+1;
  	structs[i].start = i*size;
  	structs[i].end = structs[i].start + size - 1 + extra;

    rc = pthread_create(&threads[i], NULL, sort, &structs[i]);

    if (rc != 0) {
    	printf("ERROR: return code from pthread_create() is %d\n", rc);
      	exit(-1);
    }


	void *status;
	for (i = 0; i < threads_count; i++) {
		rc = pthread_join(threads[i], &status);

		if (rc != 0) {
			printf("ERROR: return code from pthread_join() is %d\n", rc);
			exit(-1);
		}

	}

	rc = pthread_mutex_destroy(&mutx);
	if (rc != 0) {
   		printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
      		exit(-1);
	}


	printf("\nThread %d sorted:", threads_count);
	int prs = (threads_count-1)*size;
	printA(prs, prs + size - 1 + extra);
    for (i = threads_count-1; i > 0; i--) {
    	printf("Thread %d sorted:", i);
    	prs -= size;
    	printA(prs, prs + size - 1);
    }


    //merge
    int j = threads_count;
    int step = size;
    int ors;

    while (j > 1) {
        i = 0;
        ors = step;
        while (ors != numbers_count) {
            prs = step*i++;
            ors = (step*i++) + step;
            if (i == j) ors += extra;
            merge(prs, prs+step-1, ors-1);
        }
        j /= 2;
        step *= 2;
    }



	fprintf(f, "Sorted array:\n[|");
	for( i=0; i<numbers_count; i++) {
		fprintf(f, " %d |", numbers[i]);
	}
	fprintf(f, "]\n");

	fclose(f);


	//calculate proccess time

	if (gettimeofday(&end_time, NULL) == -1) {
		printf("ERROR: gettimeofday failed!\n");
		exit(-1);
	}

	printf("\nProgram elapse time: %ld microseconds\n\n", (end_time.tv_sec - start_time.tv_sec)*1000000L + end_time.tv_usec - start_time.tv_usec);

	free(numbers);

	return 0;

}

