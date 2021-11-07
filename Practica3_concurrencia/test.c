#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS 1
#define MAX_COUNT 100000

/*
Reader: Writer:
P(mutex); P(write);
readers++; write database
if (readers == 1) V(write);
P(write);
V(mutex);
read database
P(mutex);
readers– –;
if (readers == 0)
V(write);
V(mutex);
*/

long counter=0;
sem_t write_mutex;
sem_t read_mutex;

int active_readers=0;
int active_writers=0;
int waiting_readers=0;
int waiting_writers=0;
sem_t control_mutex;

//depends: active_readers write_mutex control_mutex
void read_rp(int id){
    //reader needs mutually exclusive access while
    //manipulating “readers” variable
    sem_wait(&control_mutex);
    active_readers++;
    if(active_readers==1){
        //If this reader is the first reader, it has to 
        //wait if there is an active writer (which has
        //exclusive access to the database)
        sem_wait(&write_mutex);
        //If other readers come along while the first 
        //one is waiting, they wait at the sem_wait(&readers_mutex);
    }
    //reader does not need mutually exclusive access 
    //while reading database
    sem_post(&control_mutex);


    printf("P%d counter:%ld\n",id,counter);

    sem_wait(&control_mutex);
    active_readers--;
    if(active_readers==0){
        //When the last reader finishes, if there are
        //waiting writers, it must wake one up
        sem_post(&write_mutex);
    }
    sem_post(&control_mutex);
}

//depends: active_readers write_mutex control_mutex
void write_rp(int id){
    //If there is an active writer/reader
    //this writer has to wait
    sem_wait(&write_mutex);
    counter++;
    printf("P%d counter++:%ld\n",id,counter);
    sem_post(&write_mutex);
    //When the writer finishes, it wakes up writer/reader
}

/*
depends:
long counter=0;
sem_t write_mutex;
sem_t read_mutex;

int active_readers=0;
int active_writers=0;
int waiting_readers=0;
int waiting_writers=0;
sem_t control_mutex;
*/
void read_wp(int id){
    sem_wait(&control_mutex);
    if(active_readers+waiting_writers>0){
        //if there are active readers or waiting writers, this
        //reader has to wait (writers have priority)
        waiting_readers++;
    }
    else{
        //else, this readers and other ones can read 
        sem_post(&read_mutex);
        active_readers++;
    }
    sem_post(&control_mutex);

    sem_wait(&read_mutex);


    printf("P%d counter:%ld\n",id,counter);
    fflush(stdout);

    sem_wait(&control_mutex);
    active_readers--;
    if(active_readers==0 && waiting_writers>0){
        //When the last reader finishes, if there are
        //waiting writers, it must wake one up
        sem_post(&write_mutex);
        active_writers++;
        waiting_writers--;
    }
    sem_post(&control_mutex);

}
void write_wp(int id){
    sem_wait(&control_mutex);
    if(active_writers+active_readers>0){
        //If there are active readers or writers, this
        //writer has to wait
        waiting_writers++;
    }
    else{
        //Otherwise, this writer can write (exclusive)
        sem_post(&write_mutex);
        active_writers++;
    }
    sem_post(&control_mutex);

    sem_wait(&write_mutex);

    sleep(100);
    counter++;
    printf("P%d counter++:%ld\n",id,counter);
    fflush(stdout);


    sem_wait(&control_mutex);
    active_writers--;
    if(waiting_writers>0){
        //if there are waiting writers, wake one up 
        sem_post(&write_mutex);
        active_writers++;
        waiting_writers--;
    }
    else if(waiting_readers>0){
        //if there are waiting readers, wake one up 
        sem_post(&read_mutex);
        active_readers++;
        waiting_readers--;
    }
    sem_post(&control_mutex);
}


void *count(void *ptr) {
    //fprintf(stderr,"\tar:%d aw:%d wr:%d ww:%d\n",active_readers,active_writers,waiting_readers,waiting_writers);
    int id=(int)ptr;
    //if(id==13||id==25||id==79){
    
    if(id==0){
        write_wp(id);
    }
    else{
        read_wp(id);
    }
    return NULL;
}
int main (int argc, char* argv[]) {

    pthread_t threads[NUM_THREADS];
    sem_init(&write_mutex, 0, 1);
    sem_init(&control_mutex, 0, 1);

    for (int i=0; i<NUM_THREADS; i++){
        pthread_create(&threads[i],NULL, count, (void *)i);
    }

    for(int i=0; i<NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Contador:%ld\n", counter);
}
