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

void send_recv(int sockfd)
{
    char buff[MAX_BUFF];

        bzero(buff, sizeof(buff));
        printf(">");
        fflush(stdout);

        snprintf(buff, MAX_BUFF, "Hello server!");
        send(sockfd, buff, sizeof(buff), 0);

        bzero(buff, sizeof(buff));
        recv(sockfd, buff, sizeof(buff),0);
        printf("+++%s\n", buff);
        fflush(stdout);

}

int main()
{
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
}
