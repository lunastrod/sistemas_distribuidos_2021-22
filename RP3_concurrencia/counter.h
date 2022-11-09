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

enum counter_operations {
    COUNTER_INCREMENT = 0,
    COUNTER_READ = 1
};

// public
void init_counter();
int safe_access_counter(long *time_waiting, int action, int id, int priority, int ratio); // thread-safe

// private
extern int counter;
extern int waiting_readers;
extern int waiting_writers;

void write_to_file();

// priorities
//https://www.researchgate.net/profile/Pierre-Jacques-Courtois/publication/234787964_Concurrent_control_with_readers_and_writers/links/55941d8608ae793d137979cb/Concurrent-control-with-readers-and-writers.pdf
void read_rp(int id, struct timespec start_wait, int *counter_value, long *time_waiting);
void write_rp(int id, struct timespec start_wait, int *counter_value, long *time_waiting);

void read_wp(int id, struct timespec start_wait, int *counter_value, long *time_waiting);
void write_wp(int id, struct timespec start_wait, int *counter_value, long *time_waiting);

void read_ra(int id, struct timespec start_wait, int *counter_value, long *time_waiting, int ratio);
void write_ra(int id, struct timespec start_wait, int *counter_value, long *time_waiting, int ratio);

// critical section
void access_counter(enum counter_operations action, int id, struct timespec start_wait, int *counter_value, long *time_waiting);