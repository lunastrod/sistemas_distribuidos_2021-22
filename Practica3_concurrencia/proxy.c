#include "proxy.h"

int sockfd,connfd;

//CLIENT============================================================================================
//connects client to server
//depends: int sockfd
void setup_client(char* ip, int port) {
    // create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        fprintf(stderr,"Socket creation failed...\n" );
        exit(1);
    }
    else {
        printf("Socket successfully created...\n" );
    }

    // assign ip and port
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port = htons(port);

    // connect client to server
    while((connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) < 0) {
        fprintf(stderr,"Connection with the server failed, retrying...\n");
        sleep(1);
    }
    printf("Client conected to server  \n");
}

//depends: connfd
struct response send_request(enum operations action){
    struct request msg;
    msg.action=action;
    //printf("debug sending action %d\n", action);
    send(sockfd, &msg, sizeof(msg), 0);
    
    struct response reply;
    recv(sockfd, &reply, sizeof(reply),0);
    //printf("debug response\n");
    return reply;
}

//closes client
//depends: int sockfd
void close_client(){
    if(close(sockfd) == 1) {
        fprintf(stderr,"Close failed\n");
        exit(1);
    }
}
//CLIENT--------------------------------------------------------------------------------------------
//SERVER============================================================================================

//setup server listen
//depends: int sockfd
void setup_server(char* ip, int port) {
    srand(time(NULL));
    // create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1){
        fprintf(stderr,"Socket creation failed...\n");
        exit(1);
    }
    else{
        printf("Socket successfully created...\n");
    }
    // assign ip and port
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port = htons(port);

    // bind to socket
    if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
        fprintf(stderr,"Socket bind failed...\n");
        exit(1);
    }
    else{
        printf("Socket successfully binded...\n");
    }
    
    //listen to clients
    if ((listen(sockfd, 100)) != 0) {
        fprintf(stderr,"Listen failed...\n");
        exit(1);
    } 
    else {
        printf("Server listening...\n");
    }
}

//depends: int sockfd, connfd
//TODO: quizas devuelvo connfd en vez de guardarla en global no se
//TODO: o quizas quedarme la connfd aqui mismo y hacer un receive de request, modificar la variable, un send de response y salirme
void accept_new_client() {
    connfd = accept(sockfd, (struct sockaddr*)NULL, NULL); //Acepta un nuevo cliente
    if (connfd < 0) {
        printf("Server accept failed...\n");
        exit(1);
    } else {
        printf("Server accepts the client...\n");
    }
}

//depends: connfd
enum operations recv_request(){
    struct request msg;
    recv(connfd, &msg, sizeof(msg),0);
    //printf("debug recv request msg: %d\n", msg.action);
    return msg.action;
}

//depends: connfd
void send_response(enum operations action, unsigned int counter, long waiting_time){
    //printf("debug response\n");
    struct response reply;
    reply.action=action;
    reply.counter=counter;
    reply.waiting_time=waiting_time;
    //fprintf(stderr,"debug reply %d %u %ld\n", action, counter, waiting_time);
    send(connfd, &reply, sizeof(reply), 0);
    
}

//closes server
//depends: int sockfd
void close_server() {
    if(close(sockfd) == 1) {
        fprintf(stderr,"Close failed\n");
        exit(1);
    }
}
//SERVER--------------------------------------------------------------------------------------------






