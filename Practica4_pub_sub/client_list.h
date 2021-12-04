#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>

/*
MACRO PARA PONER TRAZAS
usage: TDEB("ip:%s port%d",ip,port);
asi son mas faciles de quitar para hacer la entrega
*/
#define TDEB(...) fprintf(stderr,"DEBUG: "__VA_ARGS__); fprintf(stderr,"\n")

enum{
    TOPICS_MAX=10,
    PUBLISHERS_MAX=100,
    SUBSCRIBERS_MAX=1000
};

enum{
    TOPIC_NAME_SIZE=100
};

struct topic{
    int8_t is_valid;
    char name[TOPIC_NAME_SIZE];
    struct client_list *subs;
    struct client_list *pubs;
};

struct client{
    int id;
    int connfd;
};

struct client_list{
    size_t size;
    size_t count;
    struct client * list;//array, malloc when init_list
};

int client_id_counter;
struct topic topics[TOPICS_MAX];

//==============================================
    //client list
    struct client_list* client_list_init(size_t size);
    void client_list_delete(struct client_list * clients);
    void client_list_empty(struct client_list * clients);
    void client_list_insert(struct client_list * clients, struct client client);
    void client_list_remove_index(struct client_list * clients, int index);
    void client_list_remove_id(struct client_list * clients, int id);
    void client_list_print(struct client_list * clients);
//==============================================
    //topic list

    //writes global variable topics[TOPICS_MAX]
    void topic_list_init();
    void topic_list_delete();
    void topic_list_print();

    //returns index
    int topic_list_index_from_name(char topic_name[TOPIC_NAME_SIZE]);

    //returns 1 if ok, 0 if error
    int topic_list_new_topic(char name[TOPIC_NAME_SIZE]);

    void topic_list_remove_topic(char topic_name[TOPIC_NAME_SIZE]);

    //returns id
    int topic_list_new_sub(char topic_name[TOPIC_NAME_SIZE], int connfd);

    void topic_list_remove_sub(char topic_name[TOPIC_NAME_SIZE], int id);

    //returns id
    int topic_list_new_pub(char topic_name[TOPIC_NAME_SIZE], int connfd);

    void topic_list_remove_pub(char topic_name[TOPIC_NAME_SIZE], int id);
//==============================================


