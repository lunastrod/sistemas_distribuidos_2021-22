// std
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TOPIC_NAME_SIZE 100
#define TOPICS_MAX 10
#define PUBLISHERS_MAX 100
#define SUBSCRIBERS_MAX 900

struct client{
    int id;
    int socket;
};

struct topic{
    char name[TOPIC_NAME_SIZE];
    struct client subs[SUBSCRIBERS_MAX];
    struct client pubs[PUBLISHERS_MAX];
};

struct client_list{
    struct topic topics[TOPICS_MAX];
    int pub_counter;
    int sub_counter;
};

enum client_type{
    PUBLISHER = 0,
    SUBSCRIBER
};

void init_client_list(struct client_list *cl);
void print_client_list(struct client_list *cl);
int n_clients(struct client *clients);
int get_new_id(struct client_list *cl, enum client_type ct);
int add_client(struct client_list *cl, enum client_type ct, char *topic, int socket);
void remove_client(struct client_list *cl, enum client_type ct, char *topic, int id);


