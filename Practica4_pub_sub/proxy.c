#include "proxy.h"

void new_log(char * text){
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    printf("[%lu.%lu] %s", spec.tv_sec, spec.tv_nsec, text);
}

//connects client to server
//returns: int sockfd
int setup_client(char* ip, int port) {
    // create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        err(1,"Socket creation failed\n");
    }
    else {
        printf("Socket successfully created\n" );
    }
    // assign ip and port
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port = htons(port);

    // connect client to server
    while((connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) < 0) {
        warn("Connection with the server failed, retrying");
        sleep(1);
    }
    printf("Client conected to server\n");
    return sockfd;
}

//setup server listen
//returns: int sockfd
int setup_server(int port){
    // create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1){
        err(1,"Socket creation failed\n");
    }
    else{
        printf("Socket successfully created\n");
    }
    // assign ip and port
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    // bind to socket
    if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
        err(1,"Socket bind failed");
    }
    else{
        printf("Socket successfully binded\n");
    }
    
    //listen to clients
    if ((listen(sockfd, 10000)) != 0) {
        err(1,"Listen failed");
    } 
    else {
        printf("Server listening\n");
    }
    return sockfd;
}

//returns: connfd
int accept_new_client(int sockfd){
    int connfd = accept(sockfd, (struct sockaddr*)NULL, NULL); //Acepta un nuevo cliente
    if (connfd < 0) {
        err(1,"Server accept failed");
    } else {
        printf("Server accepts the client\n");
    }
    return connfd;
}

int pub_sub_register_unregister(int sockfd, char topic[100], int id, enum operations action){
    if(     action!=REGISTER_PUBLISHER &&\
            action!=UNREGISTER_PUBLISHER &&\
            action!=REGISTER_SUBSCRIBER &&\
            action!=UNREGISTER_SUBSCRIBER)
        {
            warnx("pub_sub_register_unregister: action not recognised");
            return -1;
        }

    struct message msg;
    struct response resp;
    int ret_val;
    bzero(&msg,sizeof(msg));

    msg.action=action;
    msg.id=id;
    strncpy(msg.topic,topic,100);

    debug_print_msg(msg,"pub_sub_register_unregister");

    ret_val = send(sockfd,&msg,sizeof(msg),0);
    if(ret_val!=sizeof(msg)){
        warnx("send broker register/unregister to topic %s failed",topic);
        return -1;
    }

    ret_val = recv(sockfd, &resp, sizeof(resp),0);
    if(ret_val!=sizeof(resp)){
        warn("recv broker response failed topic:%s",topic);
        return -1;
    }
    TDEB("recv");

    switch (resp.response_status){
    case OK:
        return resp.id;
    case LIMIT:
        warnx("limit reached on topic: %s", topic);
        return -1;
    case ERROR:
        if(action==REGISTER_PUBLISHER || action==REGISTER_SUBSCRIBER){
            warnx("register to %s failed", topic);
        }
        if(action==UNREGISTER_PUBLISHER || action==UNREGISTER_SUBSCRIBER){
            warnx("unregister to %s failed", topic);
        }
        return -2;
    }
    warnx("pub_sub_register_unregister: recv failed, invalid response_status");
    return -1;
}

int  pub_register(char topic[100]){
    return pub_sub_register_unregister(my_sockfd,topic,0,REGISTER_PUBLISHER);
}
void pub_unregister(char topic[100], int id){
    pub_sub_register_unregister(my_sockfd,topic,id,UNREGISTER_PUBLISHER);
}
int  sub_register(char topic[100]){
    return pub_sub_register_unregister(my_sockfd,topic,0,REGISTER_SUBSCRIBER);
}
void sub_unregister(char topic[100], int id){
    pub_sub_register_unregister(my_sockfd,topic,id,UNREGISTER_SUBSCRIBER);
}

void brok_recv(int connfd){
    TDEB("hola?");
    struct message msg;
    int ret_val;
    ret_val = recv(connfd, &msg, sizeof(msg),0);
    if(ret_val!=sizeof(msg)){
        warn("recv broker failed");
        return;
    }
    if(msg.action==PUBLISH_DATA){
        TDEB("publish data");
        brok_seq_send(msg);
    }
    else{
        TDEB("new register");
        brok_new_register(connfd, msg.topic, msg.id, msg.action);
    }
}

void brok_new_register(int connfd, char topic[100], int id, enum operations action){
    struct response msg;
    msg.id=-1;
    msg.response_status=ERROR;


    if(action==REGISTER_PUBLISHER){
        msg.id=topic_list_new_pub(topic,connfd);
        msg.response_status=OK;
    }
    if(action==REGISTER_SUBSCRIBER){
        msg.id=topic_list_new_sub(topic,connfd);
        msg.response_status=OK;
    }
    if(action==UNREGISTER_PUBLISHER){
        topic_list_remove_pub(topic,id);
        msg.response_status=OK;
    }
    if(action==UNREGISTER_SUBSCRIBER){
        topic_list_remove_sub(topic,id);
        msg.response_status=OK;
    }
    debug_print_response(msg,"publish_data");

    int ret_val;
    ret_val = send(connfd,&msg,sizeof(msg),0);
    if(ret_val!=sizeof(msg)){
        warn("send broker register/unregister to topic %s failed ",topic);
    }
}

void pub_init(char* ip, int port){
    my_sockfd=setup_client(ip,port);
}
void pub_close();//calls unregister, closes sockfd

void sub_init(char* ip, int port){
    my_sockfd=setup_client(ip,port);
}
void sub_close();//calls unregister, closes sockfd

void brok_init(int port){
    topic_list_init();
    my_sockfd=setup_server(port);

}
void brok_close(){
    topic_list_delete();
    close(my_sockfd);
}



void pub_publish_data(char data[100], char topic[TOPIC_NAME_SIZE]){
    struct message msg;
    clock_gettime(CLOCK_REALTIME, &msg.data.time_generated_data);
    strncpy(msg.data.data,data,sizeof(msg.data.data));
    msg.action=PUBLISH_DATA;
    msg.id=-1;
    strncpy(msg.topic,topic,sizeof(msg.topic));
    debug_print_msg(msg,"publish_data");

    int ret_val = send(my_sockfd,&msg,sizeof(msg),0);
    if(ret_val!=sizeof(msg)){
        warn("pub_publish_data %s failed ", data);
    }
    
    
}

void debug_print_msg(struct message msg, char * debug_msg){
    printf("%s msg{action:",debug_msg);
    switch(msg.action){
        case REGISTER_PUBLISHER:
            printf("REGISTER_PUBLISHER");
            break;
        case UNREGISTER_PUBLISHER:
            printf("UNREGISTER_PUBLISHER");
            break;
        case REGISTER_SUBSCRIBER:
            printf("REGISTER_SUBSCRIBER");
            break;
        case UNREGISTER_SUBSCRIBER:
            printf("UNREGISTER_SUBSCRIBER");
            break;
        case PUBLISH_DATA:
            printf("PUBLISH_DATA");
            break;
    }
    printf(", id:%d, topic:%s, time:[%lu.%lu], data:%s}\n",msg.id,msg.topic,msg.data.time_generated_data.tv_sec,msg.data.time_generated_data.tv_nsec,msg.data.data);
}

void debug_print_response(struct response msg, char * debug_msg){
    printf("%s response{status: ", debug_msg);
    switch(msg.response_status){
        case OK:
            printf("OK");
            break;
        case LIMIT:
            printf("LIMIT");
            break;
        case ERROR:
            printf("ERROR");
            break;
    }
    printf(", id:%d}\n",msg.id);
}

void brok_seq_send(struct message msg){
    debug_print_msg(msg,"brok_seq_send");

    int topic_index=topic_list_index_from_name(msg.topic);
    if(topic_index<0){
        warnx("brok_seq_send topic doesnt exist");
    }
    for(int i=0; i<topics[topic_index].subs->count; i++){
        int fd=topics[topic_index].subs->list[i].connfd;
        debug_print_msg(msg,"seq");
        send(fd,&msg,sizeof(msg),0);
    }
}

void brok_seq_recv(){
    for(int i=0; i<TOPICS_MAX; i++){
        TDEB("topic:%s %d",topics[i].name,topics[i].pubs->count);
        if(!topics[i].is_valid){
            continue;
        }
        for(int j=0; j<topics[i].pubs->count; j++){
            struct message msg;
            int connfd=topics[i].pubs->list[j].connfd;
            TDEB("haciendo recv");
            int ret_recv = recv(connfd, &msg, sizeof(msg), MSG_DONTWAIT);
            if(ret_recv<0){
                warn("recv error");
                return;
            }
            brok_seq_send(msg);
        }
    }
}