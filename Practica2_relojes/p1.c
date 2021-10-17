#include "proxy.h"

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>


#define MAX_BUFF 80
#define PORT 8080
#define SA struct sockaddr

int maximum(int a1, int a2){
    if(a1>a2)
        return a1;
    else
        return a2;
}

void send_recv(int sockfd)
{
    unsigned int lcl=0;
    struct message msg;
    msg.clock_lamport=0;
    msg.action=0;

    while (1) {
        printf("sending:\t");
        snprintf (msg.origin, 19, "cliente! ");
        fflush(stdout);

        msg.clock_lamport=lcl;

        printf("origin:%s\taction=%d\tclock=%d\n", msg.origin, msg.action, msg.clock_lamport);
        fflush(stdout);
        send(sockfd, &msg, sizeof(msg), 0);

        lcl++;
        sleep(1);










        printf("receiving:\t");
        fflush(stdout);

        recv(sockfd, &msg, sizeof(msg),0);

        lcl=1+maximum(lcl,msg.clock_lamport);
        
        printf("origin:%s\taction=%d\tclock=%d\n", msg.origin, msg.action, msg.clock_lamport);
        fflush(stdout);

        sleep(1);
    }
}


int main() {
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
   
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created...\n");
    bzero(&servaddr, sizeof(servaddr));
   
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
   
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed\n");
        exit(0);
    }
    else
        printf("connected to the server...\n");
   
    send_recv(sockfd);
    close(sockfd);
    return 0;
}