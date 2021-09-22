#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080

/*
socket
bind
listen
accept

recv
send

close
*/

int main() {
    //int server_fd;//lo que devuelve socket

    printf("servidor\n");

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
