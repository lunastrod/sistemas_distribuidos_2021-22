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
