#include "proxy.h"

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define MAX_BUFF 80
#define SA struct sockaddr

unsigned int _lcl=0;
int _sockfd;
#define NAME_LEN 3
char _process_name[NAME_LEN];
char _ip[20];
unsigned int _port;


int maximum(int a1, int a2){
    if(a1>a2)
        return a1;
    else
        return a2;
}

void print_msg(struct message msg, int sending){
    if(sending){
        printf("sending:\t");
    }
    else{
        printf("receiving:\t");
    }
    char action[32];
    switch(msg.action){
        case READY_TO_SHUTDOWN:
            snprintf (action, 32, "%s", "READY_TO_SHUTDOWN");
            break;
        case SHUTDOWN_NOW:
            snprintf (action, 32, "%s", "SHUTDOWN_NOW     ");
            break;
        case SHUTDOWN_ACK:
            snprintf (action, 32, "%s", "SHUTDOWN_ACK     ");
            break;
    }
    
    printf("origin:%s\taction=%s\tclock=%d\n", msg.origin, action, msg.clock_lamport);
    fflush(stdout);
}

void create_msg(struct message * msg, int action){
    snprintf (msg->origin, NAME_LEN, "%s", _process_name);
    msg->clock_lamport=_lcl;
    msg->action=action;
    print_msg(*msg,1);
}

//setup del servidor
void setup_server(){
    //int _sockfd;
    int connfd, len;
    struct sockaddr_in servaddr, cli;
   
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd == -1) {
        printf("socket creation failed\n");
        exit(0);
    }
    else
        printf("Socket successfully created...\n");
    bzero(&servaddr, sizeof(servaddr));
   
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(_port);
   
    if ((bind(_sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");
   
    if ((listen(_sockfd, 5)) != 0) {
        printf("Listen failed\n");
        exit(0);
    }
    else
        printf("Server listening...\n");
    len = sizeof(cli);
   
    connfd = accept(_sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        printf("server accept failed\n");
        exit(0);
    }
    else
        printf("server accept the client...\n");
}
//setup del cliente
void connect_to_server(){
    //int _sockfd
    int connfd;
    struct sockaddr_in servaddr, cli;
   
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created...\n");
    bzero(&servaddr, sizeof(servaddr));
   
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(_port);
   
    if (connect(_sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed\n");
        exit(0);
    }
    else
        printf("connected to the server...\n");
}


//----------------------------------------------------------------------------------------------

// Establece el nombre del proceso (para los logs y trazas)
void set_name (char name[2]){
    snprintf (_process_name, 3, "%s", name);
}
// Establecer ip y puerto
void set_ip_port (char* ip, unsigned int port){
    snprintf (_ip, 20, "%s", ip);
    _port=port;
}

// Obtiene el valor del reloj de lamport.
// Utilízalo cada vez que necesites consultar el tiempo.
int get_clock_lamport(){
    return _lcl;
}
// Notifica que está listo para realizar el apagado (READY_TO_SHUTDOWN)
void notify_ready_shutdown(){
    //p1 y p3
    struct message msg;

    create_msg(&msg,READY_TO_SHUTDOWN);
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
}(_sockfd, &msg, sizeof(msg), 0);
    _lcl++;

    //active wait receive shutdown now
    //TODO: comprobar si es un shtdowns now

    recv(_sockfd, &msg, sizeof(msg),0);
    _lcl=1+maximum(_lcl,msg.clock_lamport);
    print_msg(msg,0);
    
}

// Notifica que va a realizar el shutdown correctamente (SHUTDOWN_ACK)
void notify_shutdown_ack(){
    struct message msg;

    create_msg(&msg,SHUTDOWN_ACK);
    send(_sockfd, &msg, sizeof(msg), 0);
    _lcl++;
}

//el servidor responde que el cliente puede apagarse (SHUTDOWN_NOW)
void reply_shutdown_now(){
    struct message msg;

    //active wait receive ready

    recv(_sockfd, &msg, sizeof(msg),0);
    _lcl=1+maximum(_lcl,msg.clock_lamport);
    print_msg(msg,0);

    if(msg.action!=READY_TO_SHUTDOWN){
        return;
    }

    create_msg(&msg,SHUTDOWN_ACK);
    send(_sockfd, &msg, sizeof(msg), 0);
    _lcl++;
}

//setup del servidor
void setup_server(){
    //int _sockfd;
    int connfd, len;
    struct sockaddr_in servaddr, cli;
   
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd == -1) {
        printf("socket creation failed\n");
        exit(0);
    }
    else
        printf("Socket successfully created...\n");
    bzero(&servaddr, sizeof(servaddr));
   
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(_port);
   
    if ((bind(_sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");
   
    if ((listen(_sockfd, 5)) != 0) {
        printf("Listen failed\n");
        exit(0);
    }
    else
        printf("Server listening...\n");
    len = sizeof(cli);
   
    connfd = accept(_sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        printf("server accept failed\n");
        exit(0);
    }
    else
        printf("server accept the client...\n");
}
//setup del cliente
void connect_to_server(){
    //int _sockfd
    int connfd;
    struct sockaddr_in servaddr, cli;
   
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created...\n");
    bzero(&servaddr, sizeof(servaddr));
   
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(_port);
   
    if (connect(_sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed\n");
        exit(0);
    }
    else
        printf("connected to the server...\n");
}