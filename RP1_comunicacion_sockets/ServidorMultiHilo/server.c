#include "proxy.h"

#include <pthread.h>
#include <string.h>


enum{
    PORT=8081,
    MAX_THREADS=100
};

void * server_thread(void * arg){
    int sockfd=*(int * ) arg;
    free(arg);
    arg=NULL;

    char buff[BUFF_SIZE];
    recv(sockfd, buff, sizeof(buff)-1, 0);
    send(sockfd, "Hello client!", sizeof("Hello client!"), 0);
    close(sockfd);
    return NULL;
}


int main(){
    pthread_t threads[MAX_THREADS];
    int8_t is_thread[MAX_THREADS];
    bzero(is_thread,sizeof(is_thread));

    int sockfd=setup_server(PORT);
    int i=0;
    while (1){
        int connfd=accept_new_client(sockfd);

        int * arg=malloc(sizeof(arg));
        *arg=connfd;

        if(is_thread[i]){
            pthread_join(threads[i],NULL);
            is_thread[i]=0;
        }
        if(pthread_create(&threads[i],NULL,server_thread,(void *)arg)){
            is_thread[i]=0;
            warn("error pthread_create");

        }
        else{
            is_thread[i]=1;
            i=(i+1)%MAX_THREADS;
        }
    }

    //never closes
    close(sockfd);
}