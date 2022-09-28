//std
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>

//sockets
#include <arpa/inet.h>

/*
MACRO PARA PONER TRAZAS
usage: TDEB("ip:%s port%d",ip,port);
asi son mas faciles de quitar para hacer la entrega
*/
#define TDEB(...) fprintf(stderr,"DEBUG: "__VA_ARGS__); fprintf(stderr,"\n")

enum{
    BUFF_SIZE=256
};

int setup_client(char* ip, int port);
void close_client(int sockfd);
int setup_server(int port);
int accept_new_client(int sockfd);
void simple_send(int sockfd, char * buffer, int buffer_size);
void simple_recv(int sockfd, char * buffer, int buffer_size);
void close_server(int sockfd);
