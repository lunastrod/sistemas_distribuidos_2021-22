#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080

/*
socket
connect

recv
send

close
*/

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

