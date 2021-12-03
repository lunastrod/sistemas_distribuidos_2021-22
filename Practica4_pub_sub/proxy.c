#include "proxy.h"

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
    if ((listen(sockfd, 100)) != 0) {
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

    ret_val = send(sockfd,&msg,sizeof(msg),0);
    if(ret_val!=sizeof(msg)){
        warnx("send broker register/unregister to topic %s failed",topic);
        return -1;
    }

    ret_val = recv(sockfd, &resp, sizeof(resp),0);
    if(ret_val!=sizeof(resp)){
        warnx("recv broker response failed topic:%s",topic);
        return -1;
    }

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
        return -1;
    }
    warnx("pub_sub_register_unregister: recv failed, invalid response_status");
    return -1;
}

int  pub_register(char topic[100]){
    return pub_sub_register_unregister(my_sockfd,topic,0,REGISTER_PUBLISHER);
}
void pub_unregister(char topic[100], int id){
    pub_sub_register_unregister(my_sockfd,topic,id,REGISTER_PUBLISHER);
}
int  sub_register(char topic[100]){
    return pub_sub_register_unregister(my_sockfd,topic,0,REGISTER_PUBLISHER);
}
void sub_unregister(char topic[100], int id){
    pub_sub_register_unregister(my_sockfd,topic,id,REGISTER_PUBLISHER);
}

void brok_recv(int connfd){
    struct message msg;
    int ret_val;
    ret_val = recv(connfd, &msg, sizeof(msg),0);
    if(ret_val!=sizeof(msg)){
        warnx("recv broker failed");
        return;
    }
    if(msg.action==PUBLISH_DATA){
        ;
    }
    else{
        brok_new_register(connfd, msg.topic, msg.id, msg.action);
    }
}

void brok_new_register(int connfd, char topic[100], int id, enum operations action){
    struct response msg;
    msg.id=1;
    msg.response_status=OK;

    int ret_val;
    ret_val = send(connfd,&msg,sizeof(msg),0);
    if(ret_val!=sizeof(msg)){
        warnx("send broker register/unregister to topic %s failed",topic);
    }
    struct client cl;
    cl.id=msg.id;
    cl.connfd=connfd;
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
    my_sockfd=setup_server(port);
}
void brok_close();