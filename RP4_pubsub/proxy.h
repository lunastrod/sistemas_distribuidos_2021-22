#include "client_list.h"

/*
TODO:
create threads for each publisher and subscriber instead of using poll in the accept thread
*/

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

struct message {
    enum operations action;
    char topic[TOPIC_NAME_SIZE];
    // Solo utilizado en mensajes de UNREGISTER
    int id;
    // Solo utilizado en mensajes PUBLISH_DATA
    struct publish data;
};

enum status {
    STATUS_OK = 0,  // El registro fue correcto
    STATUS_LIMIT,   // Se llegó al límite de los publicadores/suscriptores/topics que se pueden almacenar.
    STATUS_ERROR    // Cualquier otro tipo de error
};

enum broker_mode {
    MODE_SEQUENTIAL = 0,
    MODE_PARALLEL = 1,
    MODE_FAIR = 2
};

struct response {
    enum status response_status;
    int id;
};

#define IP_SIZE 16

// CONFIGURATION
#define CONNECT_RETRY 0  //(0 or 1)
#define DEBUG 1          //(0 or 1)

// PRIVATE FUNCTIONS
int setup_client(char *ip, int port);
int setup_server(int port);
int accept_new_client(int sockfd);
void close_connection(int sockfd);

void simple_send(int sockfd, void *buffer, int buffer_size, int send_flags);
void simple_recv(int sockfd, void *buffer, int buffer_size, int recv_flags);

// SETUP
int setup_broker(int port);
int setup_subscriber(char *ip, int port);
int setup_publisher(char *ip, int port);

// CLIENTS
int send_config(int sockfd, enum operations action, char *topic, int id);
int recv_response_msg(int sockfd);  //(called by send_config_msg)
void publish(int sockfd, char *topic, char *data, int data_size);
void subscribe(int sockfd, char *topic, struct publish *publish);

// BROKER
void recv_register_msg(int sockfd, struct message *message, struct client_list *client_list);                          // register (called by the accept thread)
void recv_publisher_msg(int sockfd, struct message *message, struct client_list *client_list, enum broker_mode mode);  // unregister publisher or publish_data (called by the fordwarder thread)
void recv_unregister_subscriber_msg(int sockfd, struct message *message, struct client_list *client_list);             // unregister subscriber (called by the unregister_sub thread)

void sequential_fordwarder(int *connfds, int connfds_size, struct message *msg);
void parallel_fordwarder(int *connfds, int connfds_size, struct message *msg);
void fair_fordwarder(int *connfds, int connfds_size, struct message *msg);

void send_response_msg(int sockfd, enum status response_status, int id);  // broker (called by recv_client_msg)
void send_subscriber_msg(int sockfd, struct message *message);            // broker (called by recv_client_msg)

// THREAD FUNCTIONS
struct broker_threads {
    pthread_t accept_thread;
    pthread_t fordwarder_thread;  // recv messages from publishers and send them to subscribers
    pthread_t subscriber_thread;  // recv unregister messages from subscribers and modify the client list
};
struct accept_thread_args {
    int sockfd;
    struct client_list *cl;
};
void *accept_thread(void *args);
struct fordwarder_thread_args {
    struct client_list *cl;
    enum broker_mode mode;
};
void *fordwarder_thread(void *args);
struct subscriber_thread_args {
    struct client_list *cl;
};
void *subscriber_thread(void *args);