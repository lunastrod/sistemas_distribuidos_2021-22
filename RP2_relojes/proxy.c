#include "proxy.h"

void print_event(char*p_name, int lamport ,int8_t is_recv, enum operations action){
    char action_name[50];
    switch (action){
        case READY_TO_SHUTDOWN:
            strncpy(action_name,"READY_TO_SHUTDOWN",50);
            break;
        case SHUTDOWN_NOW:
            strncpy(action_name,"SHUTDOWN_NOW",50);
            break;
        case SHUTDOWN_ACK:
            strncpy(action_name,"SHUTDOWN_ACK",50);
            break;
    }

    if(is_recv){
        //PX, contador_lamport, SEND, operations
        printf("%s, %d, SEND, %s",my_name,lamport,action_name);
    }
    else{
        //PX, contador_lamport, RECV (PY), operations
        printf("%s, %d, RECV (%s), %s",my_name,lamport,p_name,action_name);
    }
}

// Establece el nombre del proceso (para los logs y trazas)
void set_name (char name[2]){
    strncpy(my_name,name,PNAME_SIZE-1);
    my_name[PNAME_SIZE-1]='\0';
}
// Establecer ip y puerto (para los logs y trazas)
void set_ip_port (char* ip, unsigned int port){
    strncpy(my_ip,ip,IP_SIZE-1);
    my_name[IP_SIZE-1]='\0';
    my_port=port;
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

//closes client
void close_client(int sockfd){
    if(close(sockfd) == 1) {
        err(1,"Close failed\n");
    }
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

//closes server
void close_server(int sockfd){
    if(close(sockfd) == 1) {
        err(1,"Close failed\n");
    }
}