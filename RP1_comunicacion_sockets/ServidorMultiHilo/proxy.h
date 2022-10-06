//std
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>

//sockets
#include <arpa/inet.h>

enum{
    BUFF_SIZE=256
};

int setup_client(char* ip, int port);
void close_client(int sockfd);
int setup_server(int port);
int accept_new_client(int sockfd);
void close_server(int sockfd);

void simple_send(int sockfd, char *buffer, int buffer_size, int send_flags);
void simple_recv(int sockfd, char *buffer, int buffer_size, int recv_flags);

void send_recv(int sockfd); //uses select and nonblocking recv