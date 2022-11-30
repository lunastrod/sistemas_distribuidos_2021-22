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
    REGISTER_PUBLISHER = 0,
    UNREGISTER_PUBLISHER,
    REGISTER_SUBSCRIBER,
    UNREGISTER_SUBSCRIBER,
    PUBLISH_DATA
};

#define DATA_SIZE 100

struct publish {
    struct timespec time_generated_data;
    char data[DATA_SIZE];
};

#define TOPIC_SIZE 100

struct message {
    enum operations action;
    char topic[TOPIC_SIZE];
    // Solo utilizado en mensajes de UNREGISTER
    int id;
    // Solo utilizado en mensajes PUBLISH_DATA
    struct publish data;
};

enum status{
    STATUS_OK = 0, // El registro fue correcto
    STATUS_LIMIT,  // Se llegó al límite de los publicadores/suscriptores/topics que se pueden almacenar.
    STATUS_ERROR   // Cualquier otro tipo de error
};

struct response {
    enum status response_status;
    int id;
};

#define IP_SIZE 16
#define CONNECT_RETRY 0 //(0 or 1)

// PRIVATE FUNCTIONS
int setup_client(char *ip, int port);
int setup_server(int port);
int accept_new_client(int sockfd);
void close_connection(int sockfd);

void simple_send(int sockfd, void *buffer, int buffer_size, int send_flags);
void simple_recv(int sockfd, void *buffer, int buffer_size, int recv_flags);

// PUBLIC FUNCTIONS
int setup_broker(int port);
int setup_subscriber(char *ip, int port);
int setup_publisher(char *ip, int port);

void send_config_msg(int sockfd, enum operations action, char *topic, int id);//clients
int recv_response_msg(int sockfd);//clients returns id
void send_publisher_msg(int sockfd, char *topic, char *data, int data_size);//publisher
void recv_subscriber_msg(int sockfd, struct publish *publish);//subscriber

void recv_client_msg(int sockfd, struct message *message);//broker
void send_response_msg(int sockfd, enum status response_status, int id);//broker
void send_subscriber_msg(int sockfd, struct publish *publish);//broker


/*
-----register publisher-----
publisher sends (struct message) to broker with topic and REGISTER_PUBLISHER action (id = 0)
broker receives config message and sends (struct response) to subscriber with id
publisher receives response and stores id

-----register subscriber-----
subscriber sends (struct message) to broker with topic and REGISTER_SUBSCRIBER action (id = 0)
broker receives config message and sends (struct response) to subscriber with id
subscriber receives response and stores id

-----publish data-----
publisher sends (struct message) to broker with topic using send_publish_msg()
broker receives publish message and sends (struct publish) to all subscribers with topic
subscriber receives publish message
*/