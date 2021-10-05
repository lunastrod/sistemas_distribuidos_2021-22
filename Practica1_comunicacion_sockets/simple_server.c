#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <netdb.h>
#include <sys/types.h>

#define MAX_BUFF 80
#define PORT 8080
#define SA struct sockaddr

static volatile int running = 1;

void int_handler(int sig) {
    running = 0;
}

void send_recv(int sockfd)
{
    char buff[MAX_BUFF];
    while (running){
        bzero(buff, sizeof(buff));
        recv(sockfd, buff, sizeof(buff),0);
        printf("+++%s\n", buff);
        fflush(stdout);

        bzero(buff, sizeof(buff));
        printf(">");
        fflush(stdout);

        fgets(buff, MAX_BUFF, stdin);

        send(sockfd, buff, sizeof(buff), 0);
        signal(SIGINT, int_handler);
    }
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
   
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        printf("server accept failed\n");
        exit(0);
    }
    else
        printf("server accept the client...\n");
   
    send_recv(connfd);
   
    close(sockfd);
}