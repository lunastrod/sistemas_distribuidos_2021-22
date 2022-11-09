// std
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// sockets
#include <arpa/inet.h>
#include <pthread.h>


enum operations {
    WRITE = 0,
    READ = 1
};

struct request {
    enum operations action;
    unsigned int id;
};

struct response {
    enum operations action;
    unsigned int counter;
    long waiting_time;
};

#define IP_SIZE 16
#define CONNECT_RETRY 0 //(0 or 1)

#define MAX_SERVER_THREADS 250

// SETUP FUNCTIONS
int setup_client(char *ip, int port);
int setup_server(int port);
int accept_new_client(int sockfd);
void close_socket(int sockfd);

void simple_send(int sockfd, void *buffer, int buffer_size, int send_flags);
void simple_recv(int sockfd, void *buffer, int buffer_size, int recv_flags);

void send_request(int sockfd, enum operations action, unsigned int id);//client
void recv_request(int sockfd, struct request *request);//server

void send_response(int sockfd, enum operations action, unsigned int counter, long waiting_time);//server
void recv_response(int sockfd, struct response *response, int client_id);//client

void client_print(struct response *response, int id);