#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

enum operations {
    WRITE = 0,
    READ
};

struct request {
    enum operations action;
};

struct response {
    enum operations action;
    unsigned int counter;
    long waiting_time;
};

//client==============================
void setup_client(char* ip, int port);

struct response send_request(enum operations action);

void close_client();
//client------------------------------

//server==============================
void setup_server(char* ip, int port);
void accept_new_client();

enum operations recv_request();
void send_response(enum operations action, unsigned int counter, long waiting_time);

void close_server();
//server------------------------------