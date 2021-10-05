#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <netdb.h>
#include <sys/types.h>

//#define PORT 8080

/*
socket
bind
listen
accept

recv
send

close
*/
/*
int main() {
    //int server_fd;

    printf("server\n");

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);


    int res = bind( sockfd,
                (struct sockaddr *) &servaddr,
                sizeof(servaddr)) == -1 ;


    res = listen(sockfd, 1);

    struct sockaddr_in sock_cli;
    int len;
    int conn_fd = accept(sockfd,(struct sockaddr *)&sock_cli, &len);


    
    return 0;
}
*/


#define MAX_BUFF 80
#define PORT 8080
#define SA struct sockaddr

// Function designed for chat between client and server.

static volatile int running = 1;

void intHandler(int sig) {
    running = 0;
}

void sendRecv(int sockfd)
{
    char buff[MAX_BUFF];
    while (running){
        fprintf(stderr,"reading");
        bzero(buff, sizeof(buff));
        printf(">");
        fflush(stdout);

        fgets(buff, MAX_BUFF, stdin);

        fprintf(stderr,"sending");
        send(sockfd, buff, sizeof(buff), 0);

        fprintf(stderr,"receiving");
        bzero(buff, sizeof(buff));
        recv(sockfd, buff, sizeof(buff), MSG_DONTWAIT);

        fprintf(stderr,"writing");
        printf("+++%s\n", buff);
        fflush(stdout);

        signal(SIGINT, intHandler);
    }
    printf("DEBUG:exit");
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
        printf("server accept failed...\n");
        exit(0);
    }
    else
        printf("server accept the client...\n");
   
    sendRecv(connfd);
   
    close(sockfd);
}