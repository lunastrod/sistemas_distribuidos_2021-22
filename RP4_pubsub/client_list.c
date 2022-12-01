#include "client_list.h"

void client_list_init(struct client_list *cl) {
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
}

void print_client_list(struct client_list *cl) {
    printf("Resumen:\n");
    for (int i = 0; i < TOPICS_MAX; i++) {
        if (cl->topics[i].name[0] != '\0') {
            printf("\t%s: ", cl->topics[i].name);
            printf("%d Suscriptores - %d Publicadores\n", n_clients(cl->topics[i].subs), n_clients(cl->topics[i].pubs));
        }
    }
}

int n_clients(struct client *clients) {
    int n = 0;
    for (int i = 0; i < SUBSCRIBERS_MAX; i++) {
        if (clients[i].id != -1) {
            n++;
        }
    }
    return n;
}

/*
void add_client(struct client_list *cl, enum client_type ct, char *topic, int id, int socket) {
    int i = 0;
    // search for empty topic or topic with same name
    while (cl->topics[i].name[0] != '\0' && strncmp(cl->topics[i].name, topic, TOPIC_NAME_SIZE) != 0 && i < TOPICS_MAX) {
        i++;
    }
    // now i is the index of the topic
    // if topic is empty, add name
    if (cl->topics[i].name[0] == '\0') {
        strncpy(cl->topics[i].name, topic, TOPIC_NAME_SIZE);
    }
    // add client to topic
    if (ct == PUBLISHER) {
        int j = 0;
        // search for empty publisher
        while (cl->topics[i].pubs[j].id != -1 && j < PUBLISHERS_MAX) {
            j++;
        }
        // now j is the index of the publisher
        cl->topics[i].pubs[j].id = id;
        cl->topics[i].pubs[j].socket = socket;
    } else {
        int j = 0;
        // search for empty subscriber
        while (cl->topics[i].subs[j].id != -1 && j < SUBSCRIBERS_MAX) {
            j++;
        }
        // now j is the index of the subscriber
        cl->topics[i].subs[j].id = id;
        cl->topics[i].subs[j].socket = socket;
    }
}
*/
int add_client(struct client_list *cl, enum client_type ct, char *topic, int socket) {
    if (ct == PUBLISHER && cl->pub_counter + 1 == PUBLISHERS_MAX) {
        // too many publishers
        return -1;
    } else if (ct == SUBSCRIBER && cl->sub_counter + 1 == SUBSCRIBERS_MAX) {
        // too many subscribers
        return -1;
    }
    int i = 0;
    // search for empty topic or topic with same name
    while (cl->topics[i].name[0] != '\0' && strncmp(cl->topics[i].name, topic, TOPIC_NAME_SIZE) != 0 && i <= TOPICS_MAX) {
        i++;
    }
    if (i == TOPICS_MAX) {
        // no empty topic found, too many topics
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
    return client_id;
}

void remove_client(struct client_list *cl, enum client_type ct, char *topic, int id) {
    int i = 0;
    // search for topic with same name
    while (strncmp(cl->topics[i].name, topic, TOPIC_NAME_SIZE) != 0 && i < TOPICS_MAX) {
        i++;
    }
    // now i is the index of the topic
    // remove client from topic
    if (ct == PUBLISHER) {
        int j = 0;
        // search for publisher with same id
        while (cl->topics[i].pubs[j].id != id && j < PUBLISHERS_MAX) {
            j++;
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
        // now j is the index of the subscriber
        cl->topics[i].subs[j].id = -1;
        cl->topics[i].subs[j].socket = -1;
        cl->sub_counter--;
    }
    // if topic is empty, remove name
    if (n_clients(cl->topics[i].subs) == 0 && n_clients(cl->topics[i].pubs) == 0) {
        bzero(cl->topics[i].name, TOPIC_NAME_SIZE);
    }
}

int get_new_id(struct client_list *cl, enum client_type ct) {
    int id = 0;
    if (ct == PUBLISHER) {
        return cl->pub_counter;
    } else {
        return PUBLISHERS_MAX + cl->sub_counter;
    }
    return id;
}