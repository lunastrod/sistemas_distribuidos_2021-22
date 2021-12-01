#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>

struct client{
    int id;
    int connfd;
};

struct client_list{
    size_t size;
    size_t count;
    struct client * list;//array, malloc when init_list
};

struct client_list* init_client_list(size_t size);
void delete_client_list(struct client_list * clients);

void insert_client(struct client_list * clients, struct client client);
void remove_client_index(struct client_list * clients, int index);

void print_list(struct client_list * clients);
