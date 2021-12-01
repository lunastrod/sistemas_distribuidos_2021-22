#include "client_list.h"

struct client_list* init_client_list(size_t size){
    struct client_list * result=malloc(sizeof(struct client_list));
    result->list=malloc(size*sizeof(struct client));
    result->size=size;
    result->count=0;
    return result;
}

void delete_client_list(struct client_list * clients){
    free(clients->list);
    free(clients);
    clients=NULL;
}

void print_list(struct client_list * clients){
    for(int i=0; i<clients->size;i++){
        if(i==clients->count){
            printf("| ");
        }
        printf("%d ",clients->list[i].connfd);
    }
    printf("\n");
}

void remove_client_index(struct client_list * clients, int index){
    if(clients->count<=0){
        warn("remove_client: client_list empty");
        return;
    }
    if(index>clients->count){
        warn("remove_client: list index out of range");
        return;
    }

    for(int i=index; i<clients->count-1; i++){
        memcpy(&clients->list[i],&clients->list[i+1],sizeof(clients->list[i+1]));
    }
    clients->count--;
}

void insert_client(struct client_list * clients, struct client client){
    if(clients->count>=clients->size){
        warn("remove_client client_list full");
        return;
    }
    clients->list[clients->count]=client;
    clients->count++;
}

