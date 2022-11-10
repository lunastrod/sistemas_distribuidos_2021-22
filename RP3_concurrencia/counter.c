#include "counter.h"


int counter = 0;

int readers = 0;//number of readers in the critical section
int writers = 0;//number of writers in the critical section

pthread_mutex_t readers_mutex;//mutex for readers variable (mutex1)
pthread_mutex_t writers_mutex;//mutex for writers variable (mutex2)
pthread_mutex_t new_readers;//mutex to avoid new readers go past the readers_mutex when there's a writer (mutex3)

pthread_mutex_t reading;//reading the critical section (r)
pthread_mutex_t writing;//writing to the critical section (w)

int ratio_readers = 0;//number of readers that have been in the critical section, reset when a writer enters
int ratio_writers = 0;//number of writers that have been in the critical section, reset when a reader enters
pthread_mutex_t ratio_mutex;//mutex for ratio variables
pthread_cond_t starving;//condvar to avoid starvation

int ratio_readers_goal = -1;//number of readers that have to have been in the critical section before a writer can enter, set in init_counter()
int ratio_writers_goal = -1;//number of writers that have to have been in the critical section before a reader can enter, set in init_counter()

int priority;//0 for readers, 1 for writers, set in init_counter()



void init_counter(int ratio, enum counter_operations _priority){
    /*
    Initialize the counter to the value in the file and the mutex
    */
    pthread_mutex_init(&readers_mutex, NULL);
    pthread_mutex_init(&writers_mutex, NULL);
    pthread_mutex_init(&new_readers, NULL);
    pthread_mutex_init(&reading, NULL);
    pthread_mutex_init(&writing, NULL);
    pthread_mutex_init(&ratio_mutex, NULL);

    //init priority and ratio
    if(ratio>0){
        if(_priority==COUNTER_READ){
            ratio_readers_goal = ratio;
        }else{
            ratio_writers_goal = ratio;
        }
    }
    priority = _priority;

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
        write_to_file();
    }
    *counter_value = counter; //return counter value
    usleep((rand() % 75 + 75) * 1000);
}

void check_ratio(enum counter_operations action){
    /*
        Checks if the ratio of readers/writers is respected
        args:
            action: operation to perform (read or write)
    */
    pthread_mutex_lock(&ratio_mutex);
    if(action == COUNTER_READ){
        if(ratio_readers_goal==-1){
            pthread_mutex_unlock(&ratio_mutex);
            return;
        }
        if(ratio_readers >= ratio_readers_goal){
            //the writers are starving, wait
            //pthread_mutex_unlock(&ratio_mutex);//unlock ratio_mutex before waiting
            //printf("ratio_r: %d, ratio_w: %d \t", ratio_readers, ratio_writers);
            //printf("writers are starving, reader waiting for them\n");
            //pthread_mutex_lock(&starving);
            pthread_cond_wait(&starving, &ratio_mutex);
            //printf("writers are done, reader can continue\n");
            //I still have to unlock ratio_mutex
            pthread_mutex_unlock(&ratio_mutex);
            return;
        }
    }
    else if(action == COUNTER_INCREMENT){
        if(ratio_writers_goal==-1){
            pthread_mutex_unlock(&ratio_mutex);
            return;
        }
        if(ratio_writers >= ratio_writers_goal){
            //the readers are starving, wait
            //pthread_mutex_unlock(&ratio_mutex);//unlock the mutex before waiting
            //printf("ratio_r: %d, ratio_w: %d \t", ratio_readers, ratio_writers);
            //printf("readers are starving, writer waiting for them\n");

            //it works with printf, but it doesn't work without it
            //lets try sleeping for 1us
            //usleep(1);
            // ok, now it doesn't work without the sleep
            // i'm going to try with a loop
            //for (int i = 0; i < 10000; i++){}
            // it works if the loop is bigger than 10000
            //pthread_mutex_lock(&starving);
            pthread_cond_wait(&starving, &ratio_mutex);
            pthread_mutex_unlock(&ratio_mutex);
            return;
        }
    }
    pthread_mutex_unlock(&ratio_mutex);
    return;
}

int safe_access_counter(long *time_waiting, int action, int id){
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
    check_ratio(action);
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
    readers++;//new reader in the critical section
    //printf("readers++: %d\n", readers);
    if(readers==1){
        pthread_mutex_lock(&writing);
    }
    pthread_mutex_unlock(&readers_mutex);
    pthread_mutex_unlock(&reading);
    pthread_mutex_unlock(&new_readers);

    access_counter(COUNTER_READ, id, start_wait, counter_value, time_waiting);

    //update ratio
    pthread_mutex_lock(&ratio_mutex);
    ratio_readers++;
    if(ratio_writers >= ratio_writers_goal && ratio_writers_goal!=-1){
        //there are readers waiting, wake them up, writers are not starving anymore
        //printf("who is waking up the writers? id %d\n", id);
        //printf("ratio_r: %d, ratio_w: %d \t", ratio_readers, ratio_writers);
        //printf("ratio_writers_goal: %d\n", ratio_writers_goal);
        //pthread_mutex_unlock(&starving);
        pthread_cond_broadcast(&starving);
    }
    ratio_writers=0;
    pthread_mutex_unlock(&ratio_mutex);

    pthread_mutex_lock(&readers_mutex);
    readers--;//readers inside critical section
    //printf("readers--: %d\n", readers);
    if(readers==0){
        pthread_mutex_unlock(&writing);
    }
    pthread_mutex_unlock(&readers_mutex);

}

//writer with write priority
void write_wp(int id, struct timespec start_wait, int *counter_value, long *time_waiting){
    pthread_mutex_lock(&writers_mutex);
    writers++;//new writer in the critical section
    //printf("writers++: %d\n", writers);
    if(writers==1){
        pthread_mutex_lock(&reading);
    }
    pthread_mutex_unlock(&writers_mutex);
    pthread_mutex_lock(&writing);

    access_counter(COUNTER_INCREMENT, id, start_wait, counter_value, time_waiting);

    //update ratio
    pthread_mutex_lock(&ratio_mutex);
    ratio_writers++;
    if(ratio_readers >= ratio_readers_goal && ratio_readers_goal!=-1){
        //there are readers waiting, wake them up, writers are not starving anymore
        //printf("who is waking up the readers? id %d\n", id);
        //printf("ratio_r: %d, ratio_w: %d \t", ratio_readers, ratio_writers);
        //printf("ratio_writers_goal: %d\n", ratio_writers_goal);
        //pthread_mutex_unlock(&starving);
        pthread_cond_broadcast(&starving);
    }
    ratio_readers=0;
    pthread_mutex_unlock(&ratio_mutex);

    pthread_mutex_unlock(&writing);
    pthread_mutex_lock(&writers_mutex);
    writers--;//writers inside critical section
    //printf("writers--: %d\n", writers);
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

    //update ratio
    pthread_mutex_lock(&ratio_mutex);
    ratio_readers++;
    if(ratio_writers >= ratio_writers_goal && ratio_writers_goal!=-1){
        //there are readers waiting, wake them up, writers are not starving anymore
        //printf("who is waking up the writers? id %d\n", id);
        //printf("ratio_r: %d, ratio_w: %d \t", ratio_readers, ratio_writers);
        //printf("ratio_writers_goal: %d\n", ratio_writers_goal);
        //pthread_mutex_unlock(&starving);
        pthread_cond_broadcast(&starving);
    }
    ratio_writers=0;
    pthread_mutex_unlock(&ratio_mutex);
    

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

    //update ratio
    pthread_mutex_lock(&ratio_mutex);
    ratio_writers++;
    if(ratio_readers >= ratio_readers_goal && ratio_readers_goal!=-1){
        //there are readers waiting, wake them up, writers are not starving anymore
        printf("who is waking up the readers? id %d\n", id);
        //printf("ratio_r: %d, ratio_w: %d \t", ratio_readers, ratio_writers);
        //printf("ratio_writers_goal: %d\n", ratio_readers_goal);
        //pthread_mutex_unlock(&starving);
        pthread_cond_broadcast(&starving);
    }
    ratio_readers=0;
    pthread_mutex_unlock(&ratio_mutex);

    pthread_mutex_unlock(&writing);
    printf("WRITE i finished writing\n");
}