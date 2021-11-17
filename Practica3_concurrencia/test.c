#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS 10
#define MAX_COUNT 100000


#define RATIO_N 50
int ratio=0;//boolean, if(ratio)
int ratio_counter=0;
sem_t ratio_mutex;//acceder a la variable ratio_counter
sem_t ratio_sem;

long counter=0;
sem_t write_sem;
sem_t read_sem;
sem_t new_readers_sem;

int readers=0;
int writers=0;
sem_t readers_mutex;//acceder a la variable readers
sem_t writers_mutex;//acceder a la variable writers

void read_rp(int id){

    //fprintf(stderr,"P%d quiero leer ratio_sem\n",id);
    //sem_wait(&ratio_sem);
    fprintf(stderr,"P%d quiero leer readers_mutex\n",id);
    sem_wait(&readers_mutex);
        readers++;
        ratio++;
        if(readers==1){
            fprintf(stderr,"P%d quiero leer write_sem\n",id);
            sem_wait(&write_sem);
        }

        fprintf(stderr,"P%d quiero leer modifico ratio_counter,%d\n",id,ratio_counter);
        sem_wait(&ratio_mutex);
        ratio_counter++;
        sem_post(&ratio_mutex);

        if(ratio && ratio_counter>RATIO_N){
            sem_post(&write_sem);
            sem_wait(&ratio_sem);
        }
    sem_post(&readers_mutex);
    //sem_post(&ratio_sem);

    //sleep(1);
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
    //fprintf(stderr,"P%d quiero escribir ratio_sem\n",id);
    //sem_wait(&ratio_sem);
    fprintf(stderr,"P%d quiero escribir write_sem\n",id);
    sem_wait(&write_sem);

    //sleep(1);

    sem_wait(&ratio_mutex);
    fprintf(stderr,"P%d quiero escribir me dejan pasar con ratio=%d\n",id,ratio_counter);
    ratio_counter=0;
    sem_post(&ratio_mutex);

    sem_post(&ratio_sem);

    counter++;
    printf("P%d counter++:%ld\n",id,counter);
    sem_post(&write_sem);
    //sem_post(&ratio_sem);
    //When the writer finishes, it wakes up writer/reader
}


void read_wp(int id){
    fprintf(stderr,"P%d quiero leer cola new_readers_sem\n",id);
    sem_wait(&new_readers_sem);
    fprintf(stderr,"P%d quiero leer cola readsem\n",id);
    sem_wait(&read_sem);
        sem_wait(&readers_mutex);
            readers++;
            if(readers==1){
                sem_wait(&write_sem);
            }
        sem_post(&readers_mutex);
    sem_post(&read_sem);
    sem_post(&new_readers_sem);

    //sleep(1);
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
    
    if(id==0){
        while(1){
            write_rp(id);
            //sleep(1);
        }
    }
    else{
        while(1){
            read_rp(id);
            //sleep(1);
        }
        
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
    sem_init(&ratio_sem,0,1);
    sem_init(&ratio_mutex,0,1);

    for (int i=0; i<NUM_THREADS; i++){
        pthread_create(&threads[i],NULL, count, (void *)i);
    }

    for(int i=0; i<NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Contador:%ld\n", counter);
}
