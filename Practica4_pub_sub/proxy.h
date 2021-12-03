//std
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <string.h>

//sockets
#include <arpa/inet.h>

//broker saves client info using client_list.h
#include "client_list.h"



enum{
    IP_SIZE=16,
};

enum operations {
    REGISTER_PUBLISHER = 0,
    UNREGISTER_PUBLISHER,
    REGISTER_SUBSCRIBER,
    UNREGISTER_SUBSCRIBER,
    PUBLISH_DATA
};

struct publish {
    struct timespec time_generated_data;
    char data[100];
};

struct message {
    enum operations action;
    char topic[100];
    int id;// Solo utilizado en mensajes de UNREGISTER
    struct publish data;// Solo utilizado en mensajes PUBLISH_DATA
};

enum status {
    OK = 0,
    LIMIT,
    ERROR
};
struct response {
    enum status response_status;
    int id;
};

int my_sockfd;


//====================================================
    //general
    //connects client to server, writes: int my_sockfd
    int setup_client(char* ip, int port);
    //setup server listen writes: int my_sockfd
    int setup_server(int port);
    //returns: connfd
    int accept_new_client(int sockfd);

    int pub_sub_register_unregister(int sockfd, char topic[100], int id, enum operations action);
//====================================================
    //publisher
    void pub_init(char* ip, int port);
    void pub_close();//calls unregister, closes sockfd

    int  pub_register(char topic[100]);//returns id, id=-1 if error
    void pub_unregister(char topic[100], int id);
    void pub_send_data(char data[100]);
//====================================================
    //broker
    void brok_init(int port);
    void brok_close();

    void brok_new_register();
    void brok_recv();
//====================================================
    //subscriber
    void sub_init_subscriber(char* ip, int port);
    void sub_close_subscriber();//calls unregister, closes sockfd

    int  sub_register(char topic[100]);//returns id, id=-1 if error
    void sub_unregister(char topic[100], int id);
    struct message sub_recv_data();
//====================================================








