#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <netdb.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>

#include <arpa/inet.h>//temp

/*
socket
connect

recv
send

close
*/

/*
int main() {
    printf("client\n");

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    int my_socket = socket(AF_INET, SOCK_STREAM, 0);
    int len;

    int connect(int sockfd, (struct sockaddr *) &servaddr, );
    
    return 0;
}
*/




#define MAX_BUFF 80
#define PORT 8080
#define SA struct sockaddr

static volatile int running = 1;

void intHandler(int sig) {
    running = 0;
}


void sendRecv(int sockfd)
{
    char buff[MAX_BUFF];

    while (running) {
        bzero(buff, sizeof(buff));
        printf(">");

        for(int i=0; i<MAX_BUFF; i++){
            char in=getchar();
            if(in=='\n'){
                break;
            }
            buff[i]=in;
        }

        send(sockfd, buff, sizeof(buff), 0);
        bzero(buff, sizeof(buff));
        recv(sockfd, buff, sizeof(buff),0);
        printf("+++%s\n", buff);

        signal(SIGINT, intHandler);
    }
    printf("DEBUG:exit");
}
   
int main()
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
   
    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
   
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);
   
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");
   
    // function for chat
    sendRecv(sockfd);
   
    // close the socket
    close(sockfd);
}
