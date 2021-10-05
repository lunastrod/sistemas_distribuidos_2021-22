#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <netdb.h>
#include <sys/types.h>
#include <pthread.h>


#define MAX_BUFF 80
#define PORT 8080
#define SA struct sockaddr
#define N_THREADS 100

typedef struct server_thread_parameters {
    int id;
    int connfd;
} server_thread_parameters;

void * server_thread(void * arg){
    server_thread_parameters params= *(server_thread_parameters *) arg;
    
    int id= params.id;
    int sockfd= params.connfd;
        
    char buff[MAX_BUFF];

    snprintf(buff, MAX_BUFF, "Hello client!");
    send(sockfd, buff, sizeof(buff), 0);

    bzero(buff, sizeof(buff));
    recv(sockfd, buff, sizeof(buff), 0);

    printf("+++%s From client: %d\n", buff,id);
    fflush(stdout);
}

int main(){
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;
   
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed\n");
        exit(0);
    }
    else
        printf("Socket successfully created...\n");
    bzero(&servaddr, sizeof(servaddr));
   
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
   
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");
   
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed\n");
        exit(0);
    }
    else
        printf("Server listening...\n");
    len = sizeof(cli);


    pthread_t server_threads[N_THREADS];
    for(int t=0; t<N_THREADS; t++){
        connfd = accept(sockfd, (SA*)&cli, &len);
        if (connfd < 0) {
            printf("server accept failed\n");
            exit(0);
        }
        else
            printf("server accept the client...\n");

        server_thread_parameters arg;
        arg.id=t;
        arg.connfd=connfd;
        pthread_create(&server_threads[t],NULL,server_thread,(void *)&arg);
    }

    for(int t=0; t<N_THREADS; t++){
        pthread_join(server_threads[t],NULL);
    }
    close(sockfd);
}