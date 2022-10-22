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
    READY_TO_SHUTDOWN = 0,
    SHUTDOWN_NOW,
    SHUTDOWN_ACK
};
enum {
    NAME_SIZE = 20,  // size of client names
    IP_SIZE = 16,    // size of ip
    N_CLIENTS = 2    // number of clients
};

#define SERVER_NAME "p2"
#define CLIENT0_NAME "p1"
#define CLIENT1_NAME "p2"

/*
The origin field will contain the name of the process that sends the message.
The action field can contain values ​​from the operations enumerated:
· READY_TO_SHUTDOWN: the machine notifies that it is ready to shut down.
· SHUTDOWN_NOW: When receiving this message the machine knows that it must shut down.
· SHUTDOWN_ACK: The machine sends this message just before performing the shutdown.
The clock_lamport field is used to send the lamport counter.
*/

struct message {
    char origin[20];
    enum operations action;
    unsigned int clock_lamport;
};

extern char my_name[NAME_SIZE];
extern char my_ip[16];
extern unsigned int my_port;
extern unsigned int local_clock_lamport;

// SEND RECV FUNCTIONS
void send_ready_shutdown(int connfd);                // void notify_ready_shutdown();
void recv_ready_shutdown(int connfd, char *p_name);  // p_name is the name of the process that sent the message

void send_shutdown_now(int connfd);
void recv_shutdown_now(int connfd);

void send_shutdown_ack(int connfd);  // void notify_shutdown_ack();
void recv_shutdown_ack(int connfd);

void simple_send(int sockfd, void *buffer, int buffer_size, int send_flags);
void simple_recv(int sockfd, void *buffer, int buffer_size, int recv_flags);

// SETUP FUNCTIONS
int setup_client(char *ip, int port);
int setup_server(int port);
int accept_new_client(int sockfd);
void close_socket(int sockfd);

void set_name(char *name);
void set_ip_port(char *ip, unsigned int port);

void print_event(char *p_name, int lamport, int8_t is_recv, enum operations action);
int get_clock_lamport();
