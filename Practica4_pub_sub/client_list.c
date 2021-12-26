#include "client_list.h"

struct client_list* client_list_init(size_t size){
    struct client_list * result=malloc(sizeof(struct client_list));
    //TDEB("malloc clients %p", result);
    result->list=malloc(size*sizeof(struct client));
    //TDEB("malloc clients->list %p", result->list);
    result->size=size;
    result->count=0;
    return result;
}

void client_list_delete(struct client_list * clients){
    client_list_empty(clients);//to close the connfds

    //TDEB("free clients->list %p", clients->list);

    free(clients->list);
    //TDEB("free clients %p", clients);

    free(clients);
    clients=NULL;
}

void client_list_empty(struct client_list * clients){
    for(int i=0; i<clients->count; i++){
        if(close(clients->list[i].connfd)<0){
            warn("client_list_empty: close %d failed", clients->list[i].connfd);
        }
    }
    clients->count=0;
    bzero(clients->list,clients->size);
}

void client_list_print(struct client_list * clients){
    for(int i=0; i<clients->size;i++){
        if(i==clients->count){
            printf("| ");
        }
        printf("%d ",clients->list[i].connfd);
    }
    printf("\n");
}

void client_list_remove_index(struct client_list * clients, int index){
    if(clients->count<=0){
        warnx("remove_client: client_list empty");
        return;
    }
    if(index>clients->count){
        warnx("remove_client: list index out of range");
        return;
    }
    if(close(clients->list[index].connfd)<0){
        warn("client_list_remove_index: close %d failed", clients->list[index].connfd);
    }

    for(int i=index; i<clients->count-1; i++){
        memcpy(&clients->list[i],&clients->list[i+1],sizeof(clients->list[i+1]));
    }
    clients->count--;
    
}

void client_list_insert(struct client_list * clients, struct client client){
    if(clients->count>=clients->size){
        warnx("remove_client client_list full");
        return;
    }
    clients->list[clients->count]=client;
    clients->count++;
}

void client_list_remove_id(struct client_list * clients, int id){
    int8_t done=0;
    for(int i=0; i<clients->count; i++){
        if(clients->list->id==id){
            client_list_remove_index(clients, i);
            done=1;
            break;
        }
    }
    if(!done){
        warnx("client_list_remove_id: not valid id: %d", id);
    }
}
































void topic_list_init(){
    sem_init(&topics_mutex, 0, 1);
    bzero(topics, sizeof(topics));
    client_id_counter=0;
    for(int i=0; i<TOPICS_MAX; i++){
        //TDEB("init %d", i);
        topics[i].pubs=client_list_init(PUBLISHERS_MAX);
        topics[i].subs=client_list_init(SUBSCRIBERS_MAX);
    }
}

void topic_list_delete(){
    for(int i=0; i<TOPICS_MAX; i++){
        //TDEB("delete %d",i);
        //TDEB("pubs");
        sem_wait(&topics_mutex);
        client_list_delete(topics[i].pubs);
        //TDEB("subs");
        client_list_delete(topics[i].subs);
        sem_post(&topics_mutex);

    }
}

void topic_list_print(){
    sem_wait(&topics_mutex);
    for(int i=0; i<TOPICS_MAX; i++){
        printf("%d topic: %s pubs: %ld subs %ld\n",topics[i].is_valid, topics[i].name, topics[i].pubs->count, topics[i].subs->count);
        for(int j=0; j<topics[i].pubs->count; j++){
            printf("p(fd%d id%d) ",topics[i].pubs->list[j].connfd, topics[i].pubs->list[j].id);
            if(j==topics[i].pubs->count-1){
                printf("\n");
            }
        }
        for(int j=0; j<topics[i].subs->count; j++){
            printf("s(fd%d id%d) ",topics[i].subs->list[j].connfd, topics[i].subs->list[j].id);
            if(j==topics[i].subs->count-1){
                printf("\n");
            }
        }
    }
    sem_post(&topics_mutex);
}

int topic_list_index_from_name(char topic_name[TOPIC_NAME_SIZE]){
    for(int i=0; i<TOPICS_MAX; i++){
        sem_wait(&topics_mutex);
        if(0==strncmp(topic_name,topics[i].name,TOPIC_NAME_SIZE)){
            sem_post(&topics_mutex);
            return i;
        }
        sem_post(&topics_mutex);
    }
    //warnx("topic name \"%s\" doesn't exist", topic_name);
    return -1;
}


void topic_list_remove_topic(char topic_name[TOPIC_NAME_SIZE]){
    int index=topic_list_index_from_name(topic_name);
    if(index<0){
        return;
    }

    if(index>=TOPICS_MAX){
        warnx("trying to remove an invalid topic, index %d", index);
        return;
    }
    sem_wait(&topics_mutex);
    if(!topics[index].is_valid){
        warnx("trying to remove an empty topic, index %d", index);
    }
    bzero(&topics[index].name,sizeof(topics[index].name));
    topics[index].is_valid=0;
    client_list_empty(topics[index].pubs);
    client_list_empty(topics[index].subs);
    sem_post(&topics_mutex);
}

int topic_list_new_topic(char name[TOPIC_NAME_SIZE]){
    if(topic_list_index_from_name(name)>=0){
        warnx("cannot add topic %s , already exists", name);
        return 0;
    }

    for(int i=0; i<TOPICS_MAX; i++){
        sem_wait(&topics_mutex);
        if(!topics[i].is_valid){
            strncpy(topics[i].name,name,TOPIC_NAME_SIZE);
            topics[i].is_valid=1;
            sem_post(&topics_mutex);
            return 1;
        }
        sem_post(&topics_mutex);
    }

    warnx("topic_list_new_topic: cannot add topic %s, topic list full", name);
    return 0;
}

int topic_list_new_sub(char topic_name[TOPIC_NAME_SIZE], int connfd){
    int index=topic_list_index_from_name(topic_name);
    if(index<0){
        if(0==topic_list_new_topic(topic_name)){
            return -1;
        }
        index=topic_list_index_from_name(topic_name);
    }
    struct client cl;
    cl.connfd=connfd;
    cl.id=client_id_counter;
    client_id_counter++;

    sem_wait(&topics_mutex);
    client_list_insert(topics[index].subs,cl);
    sem_post(&topics_mutex);

    return cl.id;
}

void topic_list_remove_sub(char topic_name[TOPIC_NAME_SIZE], int id){
    int index=topic_list_index_from_name(topic_name);
    if(index<0){
        return;
    }
    sem_wait(&topics_mutex);
    client_list_remove_id(topics[index].subs,id);
    sem_post(&topics_mutex);

    sem_wait(&topics_mutex);
    if(topics[index].subs->count==0 && topics[index].pubs->count==0){
        sem_post(&topics_mutex);
        topic_list_remove_topic(topic_name);
    }
    else{
        sem_post(&topics_mutex);
    }
}

int topic_list_new_pub(char topic_name[TOPIC_NAME_SIZE], int connfd){
    
    int index=topic_list_index_from_name(topic_name);
    
    if(index<0){
        
        if(0==topic_list_new_topic(topic_name)){
            return -1;
        }
        //TDEB("llegas aqui?");
        index=topic_list_index_from_name(topic_name);
        //TDEB("pero aqui no");
    }
    

    struct client cl;
    cl.connfd=connfd;
    cl.id=client_id_counter;
    client_id_counter++;

    sem_wait(&topics_mutex);
    client_list_insert(topics[index].pubs,cl);
    sem_post(&topics_mutex);

    return cl.id;
}
void topic_list_remove_pub(char topic_name[TOPIC_NAME_SIZE], int id){
    TDEB("topic_list_remove_pub %s, %d\n",topic_name,id);
    int index=topic_list_index_from_name(topic_name);
    if(index<0){
        return;
    }
    TDEB("index:%d\n",index);

    sem_wait(&topics_mutex);
    client_list_remove_id(topics[index].pubs,id);
    sem_post(&topics_mutex);

    sem_wait(&topics_mutex);
    if(topics[index].subs->count==0 && topics[index].pubs->count==0){
        sem_post(&topics_mutex);
        topic_list_remove_topic(topic_name);
    }
    else{
        sem_post(&topics_mutex);
    }
    
}