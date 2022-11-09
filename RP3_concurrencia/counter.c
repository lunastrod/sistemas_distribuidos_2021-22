#include "counter.h"

int counter = 0;
pthread_mutex_t counter_mutex;

void init_counter(){
    /*
    Initialize the counter to the value in the file and the mutex
    */
    pthread_mutex_init(&counter_mutex, NULL);

    //if file exists, read counter from the last line of the file
    FILE *file = fopen(COUNTER_FILENAME, "r");
    if(file != NULL){
        printf("Reading counter from file: %s\n", COUNTER_FILENAME);
        fseek(file, -MAX_LINE_SIZE, SEEK_END);
        char line[MAX_LINE_SIZE];
        while(fgets(line, MAX_LINE_SIZE, file) != NULL){
            //skips lines until the last one
        }
        printf("Line: %s", line);
        counter=atoi(line);
    }
    //if file does not exist, create it
    else{
        write_to_file();
    }
}

long subtract_timespecs(struct timespec start, struct timespec end){
    /*
    Returns the difference between two timespecs in nanoseconds
    */
    struct timespec result;
    if ((end.tv_nsec - start.tv_nsec) < 0) {
        result.tv_sec = end.tv_sec - start.tv_sec - 1;
        result.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } else {
        result.tv_sec = end.tv_sec - start.tv_sec;
        result.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    //convert to long (nanoseconds)
    return result.tv_sec*1000000000 + result.tv_nsec;
}

void access_counter(enum counter_operations action, int id, struct timespec start_wait, int *counter_value, long *time_waiting){
    /*
        Critical section of the counter, it's not protected by a mutex
        args:
            action: operation to perform (read or write)
            id: id of the thread that is performing the operation
            start_wait: time when the thread started waiting
            counter_value: returns value of the counter
            time_waiting: returns time the thread waited
    */
    struct timespec end_wait;
    clock_gettime(CLOCK_REALTIME, &end_wait);
    *time_waiting = subtract_timespecs(start_wait, end_wait); //return time waiting in nanoseconds
    if(action == COUNTER_READ){
        printf("[%ld.%ld][LECTOR #%d] lee contador con valor %d\n", end_wait.tv_sec, end_wait.tv_nsec, id, counter);
    }
    else if(action == COUNTER_INCREMENT){
        counter++;
        printf("[%ld.%ld][ESCRITOR #%d] modifica contador con valor %d\n", end_wait.tv_sec, end_wait.tv_nsec, id, counter);
        write_to_file();
    }
    *counter_value = counter; //return counter value
    usleep((rand() % 75 + 75) * 1000);
}

int read_counter(int id, long *time_waiting){
    /*
        Reads the counter
        args:
            id: id of the thread that is performing the operation
            time_waiting: returns time the thread waited
        returns:
            value of the counter
    */
    struct timespec start_wait;
    clock_gettime(CLOCK_REALTIME, &start_wait);
    int counter_value;

    pthread_mutex_lock(&counter_mutex);
    access_counter(COUNTER_READ, id, start_wait, &counter_value, time_waiting);
    pthread_mutex_unlock(&counter_mutex);
    return counter_value;

}

int increment_counter(int id, long *time_waiting){
    /*
        Increments the counter
        args:
            id: id of the thread that is performing the operation
            time_waiting: returns time the thread waited
        returns:
            value of the counter
    */
    struct timespec start_wait;
    clock_gettime(CLOCK_REALTIME, &start_wait);
    int counter_value;
    pthread_mutex_lock(&counter_mutex);
    access_counter(COUNTER_INCREMENT, id, start_wait, &counter_value, time_waiting);
    pthread_mutex_unlock(&counter_mutex);
    return counter_value;
}

void write_to_file(){
    /*
        Writes the counter to the file
    */
    FILE *file = fopen(COUNTER_FILENAME, "a");
    if(file == NULL){
        err(1, "Error opening file");
    }
    fprintf(file, "%d\n", counter);
    fclose(file);
}