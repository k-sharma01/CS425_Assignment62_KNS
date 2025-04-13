/**
CS 425 Assignment 6.2
Kirin Sharma
This program creates a number of posix threads specified by a command-line argument and
distributes an array of integers across processes, sums its portion, then computes the total sum.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ARRAYSIZE 100000 // constant for the size of the array

// Globally accessible variables that can be used by any thread
int thread_count; /*Global thread count*/
int *array;
long totalSum = 0;
pthread_mutex_t lock;
int chunk_size;

void *sumArray(void* thread); /*Thread function*/

int main(int argc, char* argv[]) {
    long thread;
	pthread_t* thread_handles;

    // Initialize shared array
    array = (int*) malloc(ARRAYSIZE * sizeof(int));
    for(int i = 0; i < ARRAYSIZE; i++) {
        array[i] = i + 1;
    }

	/* Get num of threads from command line and allocate space for their handles*/
	thread_count = strtol(argv[1], NULL, 10);
	thread_handles = malloc(thread_count*sizeof(pthread_t));
    chunk_size = ARRAYSIZE / thread_count;
    pthread_mutex_init(&lock, NULL);

    /* Create each thread and start working. */
	for (thread = 0; thread < thread_count; thread++) {
		pthread_create(&thread_handles[thread], NULL, sumArray, (void*) thread);
	}

    // Join the threads
	for (thread = 0; thread < thread_count; thread++) {
		pthread_join(thread_handles[thread], NULL);
	}

    printf("The global sum is: %ld.\n", totalSum); // For nums 1-100,000, the expected output is 5000050000 

    // Free memory and return
	free(thread_handles);
    free(array);
    pthread_mutex_destroy(&lock);
	return 0;
} /* end main */

void *sumArray(void* thread) {
    int thread_id = (long) thread; // gets the id of the thread
    int startIndex = thread_id * chunk_size;
    int endIndex = (thread_id == thread_count - 1) ? ARRAYSIZE : startIndex + chunk_size;

    long localSum = 0;
    for(int i = startIndex; i < endIndex; i++) {
        localSum += array[i];
    }

    pthread_mutex_lock(&lock);
    totalSum += localSum;
    pthread_mutex_unlock(&lock);
    return NULL;
} // end sumArray