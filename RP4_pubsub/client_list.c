// std
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// threads
#include <pthread.h>

#include "client_list.h"

#define DEBUG 0

pthread_mutex_t client_list_mutex;

void print_debug2(char *msg) {
    if (DEBUG) {
        printf("DEBUG:%s\n", msg);
    }
}

void debug_print_client_list(struct client_list *cl){
    printf("Resumen:\n");
    for (int i = 0; i < TOPICS_MAX; i++) {
        if(cl->topics[i].name[0] != '\0'){
            printf("\t%d %s: ", i, cl->topics[i].name);
        }
        else{
            printf("\t%d %s: ", i, "$(EMPTY)");
        }
        printf("\n\t%d pubs:", n_clients(cl->topics[i].pubs, PUBLISHERS_MAX));
        for(int j = 0; j < PUBLISHERS_MAX; j++){
            if(cl->topics[i].pubs[j].id != -1){
                printf("%d ", cl->topics[i].pubs[j].id);
            }
            else{
                printf("%s", "-");
            }
        }

        printf("\n\t%d subs:", n_clients(cl->topics[i].subs, SUBSCRIBERS_MAX));

        for(int j = 0; j < SUBSCRIBERS_MAX; j++){
            if(cl->topics[i].subs[j].id != -1){
                printf("%d ", cl->topics[i].subs[j].id);
            }
            else{
                printf("%s", "-");
            }
        }

        printf("\n");
    }
}

void init_client_list(struct client_list *cl) {
    for (int i = 0; i < TOPICS_MAX; i++) {
        bzero(cl->topics[i].name, TOPIC_NAME_SIZE);
        for (int j = 0; j < PUBLISHERS_MAX; j++) {
            cl->topics[i].pubs[j].id = -1;
            cl->topics[i].pubs[j].socket = -1;
        }
        for (int j = 0; j < SUBSCRIBERS_MAX; j++) {
            cl->topics[i].subs[j].id = -1;
            cl->topics[i].subs[j].socket = -1;
        }
    }
    cl->pub_counter = 0;
    cl->sub_counter = 0;
    cl->id_counter = 0;
    pthread_mutex_init(&client_list_mutex, NULL);
}



void print_client_list(struct client_list *cl) {
    if(DEBUG){
        debug_print_client_list(cl);
        return;
    }
    printf("Resumen:\n");
    for (int i = 0; i < TOPICS_MAX; i++) {
        if (cl->topics[i].name[0] != '\0') {
            printf("\t%s: ", cl->topics[i].name);
            printf("%d Suscriptores - %d Publicadores\n", n_clients(cl->topics[i].subs, SUBSCRIBERS_MAX), n_clients(cl->topics[i].pubs, PUBLISHERS_MAX));
        }
    }
}

int n_clients(struct client *clients, int size) {
    int n = 0;
    for (int i = 0; i < size; i++) {
        if (clients[i].id != -1) {
            n++;
        }
    }
    return n;
}

int add_client(struct client_list *cl, enum client_type ct, char *topic, int socket) {
    print_debug2("before add_client lock");
    pthread_mutex_lock(&client_list_mutex);
    if (ct == PUBLISHER && cl->pub_counter == PUBLISHERS_MAX) {
        print_debug2("too many publishers");
        pthread_mutex_unlock(&client_list_mutex);
        return -1;
    } else if (ct == SUBSCRIBER && cl->sub_counter == SUBSCRIBERS_MAX) {
        print_debug2("too many subscribers");
        pthread_mutex_unlock(&client_list_mutex);
        return -1;
    }
    int i = 0;
    while (cl->topics[i].name[0] != '\0' && strncmp(cl->topics[i].name, topic, TOPIC_NAME_SIZE) != 0 && i < TOPICS_MAX) {
        i++;
    }
    if (i == TOPICS_MAX) {
        print_debug2("too many topics");
        pthread_mutex_unlock(&client_list_mutex);
        return -1;
    }
    // now i is the index of the topic
    // if topic is empty, add name
    if (cl->topics[i].name[0] == '\0') {
        strncpy(cl->topics[i].name, topic, TOPIC_NAME_SIZE);
    }
    // generate new id for client
    int client_id = get_new_id(cl, ct);
    // add client to topic
    if (ct == PUBLISHER) {
        int j = 0;
        // search for empty publisher
        while (cl->topics[i].pubs[j].id != -1 && j < PUBLISHERS_MAX) {
            j++;
        }
        // now j is the index of the publisher
        cl->topics[i].pubs[j].id = client_id;
        cl->topics[i].pubs[j].socket = socket;
        cl->pub_counter++;
    } else {
        int j = 0;
        // search for empty subscriber
        while (cl->topics[i].subs[j].id != -1 && j < SUBSCRIBERS_MAX) {
            j++;
        }
        // now j is the index of the subscriber
        cl->topics[i].subs[j].id = client_id;
        cl->topics[i].subs[j].socket = socket;
        cl->sub_counter++;
    }
    pthread_mutex_unlock(&client_list_mutex);
    return client_id;
}

void remove_client(struct client_list *cl, enum client_type ct, char *topic, int id) {
    print_debug2("before remove_client lock");
    pthread_mutex_lock(&client_list_mutex);
    int i = 0;
    // search for topic with same name
    while (strncmp(cl->topics[i].name, topic, TOPIC_NAME_SIZE) != 0 && i < TOPICS_MAX) {
        i++;
    }
    if(i == TOPICS_MAX){
        print_debug2("topic not found");
        pthread_mutex_unlock(&client_list_mutex);
        return;
    }
    // now i is the index of the topic
    // remove client from topic
    if (ct == PUBLISHER) {
        int j = 0;
        // search for publisher with same id
        while (cl->topics[i].pubs[j].id != id && j < PUBLISHERS_MAX) {
            j++;
        }
        if(j == PUBLISHERS_MAX){
            print_debug2("publisher not found, invalid id");
            pthread_mutex_unlock(&client_list_mutex);
            return;
        }
        // now j is the index of the publisher
        cl->topics[i].pubs[j].id = -1;
        cl->topics[i].pubs[j].socket = -1;
        cl->pub_counter--;
    } else {
        int j = 0;
        // search for subscriber with same id
        while (cl->topics[i].subs[j].id != id && j < SUBSCRIBERS_MAX) {
            j++;
        }
        if(j == SUBSCRIBERS_MAX){
            print_debug2("subscriber not found, invalid id");
            pthread_mutex_unlock(&client_list_mutex);
            return;
        }
        // now j is the index of the subscriber
        cl->topics[i].subs[j].id = -1;
        cl->topics[i].subs[j].socket = -1;
        cl->sub_counter--;
    }
    // if topic is empty, remove name
    if (n_clients(cl->topics[i].subs, SUBSCRIBERS_MAX) == 0 && n_clients(cl->topics[i].pubs, PUBLISHERS_MAX) == 0) {
        print_debug2("topic is empty, removing");
        bzero(cl->topics[i].name, TOPIC_NAME_SIZE);
    }
    pthread_mutex_unlock(&client_list_mutex);
}

int get_new_id(struct client_list *cl, enum client_type ct) {
    return cl->id_counter++;//returns the id and then increments it
}

int get_subscribers(struct client_list *cl, char *topic, int *connfds) {
    print_debug2("before get_subscribers lock");
    pthread_mutex_lock(&client_list_mutex);
    int i = 0;
    // search for topic with same name
    while (strncmp(cl->topics[i].name, topic, TOPIC_NAME_SIZE) != 0 && i < TOPICS_MAX) {
        i++;
    }
    if(i == TOPICS_MAX){
        print_debug2("topic not found");
        pthread_mutex_unlock(&client_list_mutex);
        return -1;
    }
    // now i is the index of the topic
    // copy subscribers to connfds
    // j is the index of the subscriber in the topic
    // k is the index of the subscriber in connfds
    int k=0;
    for (int j = 0; j < SUBSCRIBERS_MAX; j++) {
        if(cl->topics[i].subs[j].id != -1){
            connfds[k] = cl->topics[i].subs[j].socket;
            k++;
        }
    }
    pthread_mutex_unlock(&client_list_mutex);
    return k;
}

int get_all_publishers(struct client_list *cl, int *connfds) {
    print_debug2("before get_all_publishers lock");
    pthread_mutex_lock(&client_list_mutex);
    int k=0;
    for(int i=0; i<TOPICS_MAX; i++){
        if(cl->topics[i].name[0] != '\0'){
            for(int j=0; j<PUBLISHERS_MAX; j++){
                if(cl->topics[i].pubs[j].id != -1){
                    connfds[k] = cl->topics[i].pubs[j].socket;
                    k++;
                }
            }
        }
    }
    pthread_mutex_unlock(&client_list_mutex);
    return k;
}

int get_all_subscribers(struct client_list *cl, int *connfds) {
    print_debug2("before get_all_subscribers lock");
    pthread_mutex_lock(&client_list_mutex);
    int k=0;
    for(int i=0; i<TOPICS_MAX; i++){
        if(cl->topics[i].name[0] != '\0'){
            for(int j=0; j<SUBSCRIBERS_MAX; j++){
                if(cl->topics[i].subs[j].id != -1){
                    connfds[k] = cl->topics[i].subs[j].socket;
                    k++;
                }
            }
        }
    }
    pthread_mutex_unlock(&client_list_mutex);
    return k;
}