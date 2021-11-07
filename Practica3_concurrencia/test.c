#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS 10
#define MAX_COUNT 100000

long counter=0;
sem_t write_sem;
sem_t read_sem;
sem_t new_readers_sem;

int readers=0;
int writers=0;
sem_t readers_mutex;
sem_t writers_mutex;


void read_rp(int id){
    //reader needs mutually exclusive access while
    //manipulating “readers” variable
    sem_wait(&readers_mutex);
    readers++;
    if(readers==1){
        //If this reader is the first reader, it has to 
        //wait if there is an active writer (which has
        //exclusive access to the database)
        sem_wait(&write_sem);
        //If other readers come along while the first 
        //one is waiting, they wait at the sem_wait(&readers_mutex);
    }
    //reader does not need mutually exclusive access 
    //while reading database
    sem_post(&readers_mutex);


    printf("P%d counter:%ld\n",id,counter);

    sem_wait(&readers_mutex);
    readers--;
    if(readers==0){
        //When the last reader finishes, if there are
        //waiting writers, it must wake one up
        sem_post(&write_sem);
    }
    sem_post(&readers_mutex);
}

void write_rp(int id){
    //If there is an active writer/reader
    //this writer has to wait
    sem_wait(&write_sem);
    counter++;
    printf("P%d counter++:%ld\n",id,counter);
    sem_post(&write_sem);
    //When the writer finishes, it wakes up writer/reader
}


void read_wp(int id){
    fprintf(stderr,"P%d quiero leer\n",id);
    fprintf(stderr,"P%d cola new_readers_sem de leer\n",id);
    sem_wait(&new_readers_sem);
    fprintf(stderr,"P%d cola readsem de leer\n",id);
    sem_wait(&read_sem);
        sem_wait(&readers_mutex);
            readers++;
            if(readers==1){
                sem_wait(&write_sem);
            }
        sem_post(&readers_mutex);
    sem_post(&read_sem);
    sem_post(&new_readers_sem);

    sleep(1);
    printf("P%d counter:%ld\n",id,counter);

    sem_wait(&readers_mutex);
        readers--;
        if(readers==0){
            sem_post(&write_sem);
        }
    sem_post(&readers_mutex);
}
void write_wp(int id){
    fprintf(stderr,"P%d quiero escribir\n",id);
    sem_wait(&writers_mutex);
        writers++;
        if(writers==1){
            sem_wait(&read_sem);
        }
    sem_post(&writers_mutex);
    fprintf(stderr,"P%d cola de escribir\n",id);
    sem_wait(&write_sem);
        counter++;
        printf("P%d counter++:%ld\n",id,counter);
        fflush(stdout);
    sem_post(&write_sem);

    sem_wait(&writers_mutex);
        writers--;
        if(writers==0){
            sem_post(&read_sem);
        }
    sem_post(&writers_mutex);
}


void *count(void *ptr) {
    //fprintf(stderr,"\tar:%d aw:%d wr:%d ww:%d\n",active_readers,active_writers,waiting_readers,waiting_writers);
    int id=(int)ptr;
    //if(id==13||id==25||id==79){
    
    if(id%2==0){
        write_wp(id);
    }
    else{
        read_wp(id);
    }
    return NULL;
}
int main (int argc, char* argv[]) {

    pthread_t threads[NUM_THREADS];
    sem_init(&write_sem, 0, 1);
    sem_init(&read_sem,  0, 1);
    sem_init(&readers_mutex, 0, 1);
    sem_init(&writers_mutex, 0, 1);
    sem_init(&new_readers_sem, 0, 1);

    for (int i=0; i<NUM_THREADS; i++){
        pthread_create(&threads[i],NULL, count, (void *)i);
    }

    for(int i=0; i<NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Contador:%ld\n", counter);
}
