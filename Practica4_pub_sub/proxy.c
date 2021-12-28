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
        //printf("Socket successfully created\n" );
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
    //printf("Client conected to server\n");
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
        //printf("Socket successfully created\n");
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
        //printf("Socket successfully binded\n");
    }
    
    //listen to clients
    if ((listen(sockfd, 20000)) != 0) {
        err(1,"Listen failed");
    } 
    else {
        //printf("Server listening\n");
    }
    return sockfd;
}

//returns: connfd
int accept_new_client(int sockfd){
    int connfd = accept(sockfd, (struct sockaddr*)NULL, NULL); //Acepta un nuevo cliente
    if (connfd < 0) {
        warn("Server accept failed");
    } else {
        //printf("Server accepts the client\n");
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

    //debug_print_msg(msg,"pub_sub_register_unregister");

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
    //TDEB("recv");

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
    //TDEB("hola?");
    struct message msg;
    int ret_val;
    ret_val = recv(connfd, &msg, sizeof(msg),0);
    if(ret_val!=sizeof(msg)){
        warn("recv broker failed");
        return;
    }
    if(msg.action==PUBLISH_DATA){
        //TDEB("publish data");
        //brok_seq_send(msg);
    }
    else{
        //TDEB("new register");

        brok_new_register(connfd, msg.topic, msg.id, msg.action);
    }
}

void brok_new_register(int connfd, char topic[100], int id, enum operations action){
    struct response msg;
    msg.id=-1;
    msg.response_status=ERROR;

    //TDEB("brok_new_register");


    if(action==REGISTER_PUBLISHER){
        //TDEB("brok_new_publisher");
        new_log(" ");
        printf("Nuevo cliente (%d) Publicador conectado :%s\n",id,topic);
        msg.id=topic_list_new_pub(topic,connfd);
        topic_list_print();
        msg.response_status=OK;
    }
    if(action==REGISTER_SUBSCRIBER){
        new_log(" ");
        printf("Nuevo cliente (%d) Subscriptor conectado : %s\n",id,topic);
        msg.id=topic_list_new_sub(topic,connfd);
        topic_list_print();
        msg.response_status=OK;
    }
    if(action==UNREGISTER_PUBLISHER){
        new_log(" ");
        printf("Eliminado cliente (%d) Publicador conectado : %s\n",id,topic);
        topic_list_remove_pub(topic,id);
        topic_list_print();
        msg.response_status=OK;
    }
    if(action==UNREGISTER_SUBSCRIBER){
        new_log(" ");
        printf("Eliminado cliente (%d) Subscriptor conectado : %s\n",id,topic);
        topic_list_remove_sub(topic,id);
        topic_list_print();
        msg.response_status=OK;
    }
    //debug_print_response(msg,"publish_data");

    int ret_val;
    ret_val = send(connfd,&msg,sizeof(msg),0);
    if(ret_val!=sizeof(msg)){
        warn("send broker register/unregister to topic %s failed ",topic);
    }
}

void pub_init(char* ip, int port){
    my_sockfd=setup_client(ip,port);
}

//calls unregister, closes sockfd
void pub_close(char topic[100], int id){
    pub_unregister(topic,id);
    sleep(1);
    close(my_sockfd);
}
    

void sub_init(char* ip, int port){
    my_sockfd=setup_client(ip,port);
}

//calls unregister, closes sockfd
void sub_close(char topic[100], int id){
    sub_unregister(topic,id);
    sleep(1);
    close(my_sockfd);
}

void brok_init(int port){
    topic_list_init();
    my_sockfd=setup_server(port);

}
void brok_close(){
    topic_list_delete();
    sleep(1);
    close(my_sockfd);
}





void pub_publish_data(char data[100], char topic[TOPIC_NAME_SIZE]){
    struct message msg;
    clock_gettime(CLOCK_REALTIME, &msg.data.time_generated_data);
    strncpy(msg.data.data,data,sizeof(msg.data.data));
    msg.action=PUBLISH_DATA;
    msg.id=-1;
    strncpy(msg.topic,topic,sizeof(msg.topic));
    //debug_print_msg(msg,"publish_data");

    int ret_val = send(my_sockfd,&msg,sizeof(msg),0);
    if(ret_val!=sizeof(msg)){
        warn("pub_publish_data %s failed ", data);
    }
    else{
        new_log(" ");
        printf("Publicado mensaje topic: %s - mensaje: %s - Generó: [%ld.%ld]\n",msg.topic,msg.data.data,msg.data.time_generated_data.tv_sec,msg.data.time_generated_data.tv_nsec);
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
    //debug_print_msg(msg,"brok_seq_send");

    int topic_index=topic_list_index_from_name(msg.topic);
    if(topic_index<0){
        warnx("brok_seq_send topic doesnt exist");
        return;
    }
    for(int i=0; i<topics[topic_index].subs->count; i++){
        int fd=topics[topic_index].subs->list[i].connfd;
        //debug_print_msg(msg,"seq");
        send(fd,&msg,sizeof(msg),0);
    }
}

void brok_seq_recv(enum broker_mode brok_mode){
    struct pollfd fds[TOPICS_MAX*PUBLISHERS_MAX];
    int npubs;
    for(int i=0; i<TOPICS_MAX; i++){
        if(!topics[i].is_valid){
            continue;
        }
        for(int j=0; j<topics[i].pubs->count; j++){
            fds[i*TOPICS_MAX+j].fd=topics[i].pubs->list[j].connfd;
            fds[i*TOPICS_MAX+j].events=POLLIN;
            npubs++;
        }
    }
    int ret = poll(fds, npubs, 1000);

    if (ret == -1) {
		err(1,"poll error");
		//return;
	}

    

    for(int i=0; i<TOPICS_MAX; i++){
        if(!topics[i].is_valid){
            continue;
        }
        for(int j=0; j<topics[i].pubs->count; j++){
            if (fds[i*TOPICS_MAX+j].revents & POLLIN){
                //printf ("fd is readable\n");
                struct message msg;
                int ret_recv = recv(fds[i*TOPICS_MAX+j].fd, &msg, sizeof(msg), MSG_DONTWAIT);
                if(ret_recv!=sizeof(msg)){
                    warn("recv error fd:%d return: %d", fds[i*TOPICS_MAX+j].fd, ret_recv);
                    return;
                }
                if(msg.action==PUBLISH_DATA){
                    new_log(" ");
                    printf("Recibido mensaje para publicar en topic: %s - mensaje: %s - Generó: [%ld.%ld]\n",msg.topic,msg.data.data,msg.data.time_generated_data.tv_sec,msg.data.time_generated_data.tv_nsec);
                    new_log(" ");
                    printf("Enviando mensaje en topic %s a %ld suscriptores.\n",msg.topic,topics[i].subs->count);
                    switch (brok_mode){
                        case SEQUENTIAL:
                            brok_seq_send(msg);
                            break;
                        case PARALLEL:
                            brok_parallel_send(msg);
                            break;
                        case FAIR:
                            brok_fair_send(msg);
                            break;
                        default:
                            warnx("brok_mode not recognised, seq");
                            brok_seq_send(msg);
                            break;
                    }
                }
                else{
                    brok_new_register(fds[i*TOPICS_MAX+j].fd, msg.topic, msg.id, msg.action);
                }

            }
        }
    }

    int nsubs;
    struct pollfd fds_subscribers[TOPICS_MAX*SUBSCRIBERS_MAX];
    for(int i=0; i<TOPICS_MAX; i++){
        if(!topics[i].is_valid){
            continue;
        }
        for(int j=0; j<topics[i].subs->count; j++){
            fds_subscribers[i*TOPICS_MAX+j].fd=topics[i].subs->list[j].connfd;
            fds_subscribers[i*TOPICS_MAX+j].events=POLLIN;
            nsubs++;
        }
    }
    poll(fds_subscribers, nsubs, 1000);
    for(int i=0; i<TOPICS_MAX; i++){
        if(!topics[i].is_valid){
            continue;
        }
        for(int j=0; j<topics[i].subs->count; j++){
            if (fds_subscribers[i*TOPICS_MAX+j].revents & POLLIN){
                //printf ("fd is readable\n");
                struct message msg;
                int ret_recv = recv(fds_subscribers[i*TOPICS_MAX+j].fd, &msg, sizeof(msg), MSG_DONTWAIT);
                if(ret_recv!=sizeof(msg)){
                    warn("recv error fd:%d return: %d", fds_subscribers[i*TOPICS_MAX+j].fd, ret_recv);
                    return;
                }
                if(msg.action!=PUBLISH_DATA){
                    brok_new_register(fds_subscribers[i*TOPICS_MAX+j].fd, msg.topic, msg.id, msg.action);
                }
            }
        }
    }


}

void * brok_thread_send(void * arg){
    struct brok_thread_send_args * args=(struct brok_thread_send_args *)arg;
    //printf("sending to %d\n", args->connfd);
    int ret=send(args->connfd,&args->msg,sizeof(struct message),0);
    if(ret<0){
        warn("send error %d",args->connfd);
    }
    return NULL;
}

void brok_parallel_send(struct message msg){
    //debug_print_msg(msg,"brok_parallel_send");
    pthread_t threads[SUBSCRIBERS_MAX];
    struct brok_thread_send_args * args[SUBSCRIBERS_MAX];

    int topic_index=topic_list_index_from_name(msg.topic);
    if(topic_index<0){
        warnx("brok_parallel_send topic doesnt exist");
        return;
    }

    int threads_used=topics[topic_index].subs->count;

    for(int i=0; i<threads_used; i++){
        args[i]=malloc(sizeof(struct brok_thread_send_args));
        args[i]->connfd=topics[topic_index].subs->list[i].connfd;
        args[i]->msg=msg;
        //printf("args[i]->connfd=%d\n",args[i]->connfd);
        pthread_create(&threads[i],NULL, brok_thread_send, (void *)args[i]);
    }

    for(int i=0; i<threads_used; i++){
        pthread_join(threads[i],NULL);
        free(args[i]);
        args[i]=NULL;
    }
}

void * brok_barrier_send(void * arg){
    struct brok_thread_send_args * args=(struct brok_thread_send_args *)arg;
    pthread_barrier_wait(&fair_send_barrier);
    int ret=send(args->connfd,&args->msg,sizeof(struct message),0);
    if(ret<0){
        warn("send error");
    }
    return NULL;
}

void brok_fair_send(struct message msg){
    //debug_print_msg(msg,"brok_fair_send");
    pthread_t threads[SUBSCRIBERS_MAX];
    struct brok_thread_send_args * args[SUBSCRIBERS_MAX];
    

    int topic_index=topic_list_index_from_name(msg.topic);
    if(topic_index<0){
        warnx("brok_fair_send topic doesnt exist");
        return;
    }

    int threads_used=topics[topic_index].subs->count;
    pthread_barrier_init(&fair_send_barrier, NULL, threads_used);

    for(int i=0; i<threads_used; i++){
        args[i]=malloc(sizeof(struct brok_thread_send_args));
        args[i]->connfd=topics[topic_index].subs->list[i].connfd;
        args[i]->msg=msg;
        pthread_create(&threads[i],NULL, brok_thread_send, (void *)args[i]);
    }

    for(int i=0; i<threads_used; i++){
        pthread_join(threads[i],NULL);
        free(args[i]);
        args[i]=NULL;
    }
}