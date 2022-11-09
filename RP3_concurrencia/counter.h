// std
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// threads
#include <pthread.h>
#include <semaphore.h>

#define COUNTER_FILENAME "server_output.txt"
#define MAX_LINE_SIZE 50

enum counter_operations{
    COUNTER_INCREMENT = 0,
    COUNTER_READ = 1
};

// private
extern int counter;
extern pthread_mutex_t counter_mutex;
void write_to_file();
// critical section
void access_counter(enum counter_operations action, int id, struct timespec start_wait, int *counter_value, long *time_waiting);

// public
void init_counter();
int increment_counter(int id, long *time_waiting);//thread-safe
int read_counter(int id, long *time_waiting);//thread-safe