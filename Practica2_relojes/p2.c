#include "proxy.h"

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

/*

void send_recv(int sockfd)
{
    unsigned int lcl=0;
    struct message msg;
    msg.clock_lamport=0;
    msg.action=0;

    while (1){
        printf("receiving:\t");
        fflush(stdout);

        recv(sockfd, &msg, sizeof(msg),0);

        lcl=1+maximum(lcl,msg.clock_lamport);

        printf("origin:%s\taction=%d\tclock=%d\n", msg.origin, msg.action, msg.clock_lamport);
        fflush(stdout);
        sleep(1);





        printf("sending:\t");
        snprintf (msg.origin, 19, "servidor!");
        fflush(stdout);

        msg.clock_lamport=lcl;

        printf("origin:%s\taction=%d\tclock=%d\n", msg.origin, msg.action, msg.clock_lamport);
        fflush(stdout);
        send(sockfd, &msg, sizeof(msg), 0);

        lcl++;
        sleep(1);
    }
}

int main() {
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
    return 0;
}

*/

int main(){
    set_name("p2");
    set_ip_port("asdasd",8080);
    setup_server();
    while(1){
        reply_shutdown_now();
    }
    printf("por que te sales");
}