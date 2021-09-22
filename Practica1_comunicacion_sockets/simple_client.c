#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080



int main() {
    // printf() displays the string inside quotation
    printf("Hello, World!\n");

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    int my_socket = socket(AF_INET, SOCK_STREAM, 0);

    int res = bind( my_socket,
                (struct sockaddr *) &servaddr,
                sizeof(servaddr)) == -1 ;
    
    return 0;
}

