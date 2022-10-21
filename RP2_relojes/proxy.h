//std
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <string.h>

//sockets
#include <arpa/inet.h>

enum operations {
    READY_TO_SHUTDOWN = 0,
    SHUTDOWN_NOW,
    SHUTDOWN_ACK
};
enum{
    NAME_SIZE=20, //size of client names
    IP_SIZE=16,   //size of ip
    N_CLIENTS=2   //number of clients
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

extern const char client_names[NAME_SIZE][N_CLIENTS];
extern char my_name[NAME_SIZE];
extern char my_ip[16];
extern unsigned int my_port;
extern int my_sockfd;
extern int client_connfds[N_CLIENTS];//client_connfds[0] is p1, client_connfds[1] is p3

//sets process name (to display in logs)
void set_name (char name[2]);
// set ip and port (to display in logs)
void set_ip_port (char* ip, unsigned int port);
// Get the value of the lamport clock.
int get_clock_lamport();
// Notifies that is ready to shutdown
void notify_ready_shutdown();
// Notifies that it is going to perform the shutdown (SHUTDOWN_ACK)
void notify_shutdown_ack();
//the server responds that the client can shutdown (SHUTDOWN_NOW)
void send_shutdown_now();

int setup_client(char* ip, int port);
int setup_server(int port);
int accept_new_client(int sockfd);

void close_socket();

void simple_send(int sockfd, void *buffer, int buffer_size, int send_flags);
void simple_recv(int sockfd, void *buffer, int buffer_size, int recv_flags);

void print_event(char*p_name, int lamport ,int8_t is_recv, enum operations action);