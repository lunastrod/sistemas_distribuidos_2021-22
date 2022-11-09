#include "counter.h"

int counter = 0;

int readers = 0;//number of readers in the critical section
int writers = 0;//number of writers in the critical section

pthread_mutex_t readers_mutex;//mutex for readers variable (mutex1)
pthread_mutex_t writers_mutex;//mutex for writers variable (mutex2)
pthread_mutex_t new_readers;//mutex to avoid new readers go past the readers_mutex when there's a writer (mutex3)

pthread_mutex_t reading;//reading the critical section (r)
pthread_mutex_t writing;//writing to the critical section (w)



void init_counter(){
    /*
    Initialize the counter to the value in the file and the mutex
    */
    pthread_mutex_init(&readers_mutex, NULL);
    pthread_mutex_init(&writers_mutex, NULL);
    pthread_mutex_init(&new_readers, NULL);
    pthread_mutex_init(&reading, NULL);
    pthread_mutex_init(&writing, NULL);

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
        //write_to_file();
    }
    *counter_value = counter; //return counter value
    usleep((rand() % 75 + 75) * 1000);
}

int safe_access_counter(long *time_waiting, int action, int id, int priority, int ratio){
    /*
        Safe access to the counter protected by mutex, uses priority and ratio to decide which threads can access the critical section
        args:
            time_waiting: returns time the thread waited
            action: operation to perform (read or write)
            id: id of the thread that is performing the operation
            priority: priority of the thread (0: reader, 1: writer)
            ratio: ratio of readers to writers
        returns:
            counter value
    */
    struct timespec start_wait;
    int counter_value;
    clock_gettime(CLOCK_REALTIME, &start_wait);
    if(action==COUNTER_READ){
        if(priority == COUNTER_INCREMENT){
            //write priority
            read_wp(id, start_wait, &counter_value, time_waiting);
        }
        else if(priority == COUNTER_READ){
            //read priority
            read_rp(id, start_wait, &counter_value, time_waiting);
        }
    }
    else if(action==COUNTER_INCREMENT){
        if(priority == COUNTER_INCREMENT){
            //write priority
            write_wp(id, start_wait, &counter_value, time_waiting);
        }
        else if(priority == COUNTER_READ){
            //read priority
            write_rp(id, start_wait, &counter_value, time_waiting);
        }
    }

    return counter_value;
}

//reader with write priority
void read_wp(int id, struct timespec start_wait, int *counter_value, long *time_waiting){
    pthread_mutex_lock(&new_readers);
    pthread_mutex_lock(&reading);
    pthread_mutex_lock(&readers_mutex);
    readers++;
    if(readers==1){
        pthread_mutex_lock(&writing);
    }
    pthread_mutex_unlock(&readers_mutex);
    pthread_mutex_unlock(&reading);
    pthread_mutex_unlock(&new_readers);

    access_counter(COUNTER_READ, id, start_wait, counter_value, time_waiting);

    pthread_mutex_lock(&readers_mutex);
    readers--;
    if(readers==0){
        pthread_mutex_unlock(&writing);
    }
    pthread_mutex_unlock(&readers_mutex);

}

//writer with write priority
void write_wp(int id, struct timespec start_wait, int *counter_value, long *time_waiting){
    pthread_mutex_lock(&writers_mutex);
    writers++;
    if(writers==1){
        pthread_mutex_lock(&reading);
    }
    pthread_mutex_unlock(&writers_mutex);
    pthread_mutex_lock(&writing);

    access_counter(COUNTER_INCREMENT, id, start_wait, counter_value, time_waiting);

    pthread_mutex_unlock(&writing);
    pthread_mutex_lock(&writers_mutex);
    writers--;
    if(writers==0){
        pthread_mutex_unlock(&reading);
    }
    pthread_mutex_unlock(&writers_mutex);
}

//reader with read priority
void read_rp(int id, struct timespec start_wait, int *counter_value, long *time_waiting){
    pthread_mutex_lock(&readers_mutex);//protect readers variable
    readers++;
    if(readers == 1){//if first reader, lock writers
        pthread_mutex_lock(&writing);//protect counter
    }
    pthread_mutex_unlock(&readers_mutex);
    access_counter(COUNTER_READ, id, start_wait, counter_value, time_waiting);
    pthread_mutex_lock(&readers_mutex);//protect readers variable
    readers--;
    if(readers == 0){//if last reader, unlock writers
        pthread_mutex_unlock(&writing);//next writer can access counter
    }
    pthread_mutex_unlock(&readers_mutex);
}

//writer with read priority
void write_rp(int id, struct timespec start_wait, int *counter_value, long *time_waiting){
    pthread_mutex_lock(&writing);//lock if there is a writer or a reader
    access_counter(COUNTER_INCREMENT, id, start_wait, counter_value, time_waiting);
    pthread_mutex_unlock(&writing);
    printf("WRITE i finished writing\n");
}