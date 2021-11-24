//std
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//sockets
//#include <sys/socket.h>
//#include <sys/types.h>
//#include <netinet/in.h>
#include <arpa/inet.h>

//extra
#include <signal.h>//SIGINT

enum{
    BUFF_SIZE=80,
    PORT=8080
};

//setup server listen
//returns: int sockfd
int setup_server(char* ip, int port) {
    // create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1){
        fprintf(stderr,"Socket creation failed\n");
        exit(1);
    }
    else{
        printf("Socket successfully created\n");
    }
    // assign ip and port
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port = htons(port);

    // bind to socket
    if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
        fprintf(stderr,"Socket bind failed\n");
        exit(1);
    }
    else{
        printf("Socket successfully binded\n");
    }
    
    //listen to clients
    if ((listen(sockfd, 100)) != 0) {
        fprintf(stderr,"Listen failed\n");
        exit(1);
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
        printf("Server accept failed\n");
        exit(1);
    } else {
        printf("Server accepts the client\n");
    }
    return connfd;
}

//closes server
void close_server(int sockfd) {
    if(close(sockfd) == 1) {
        fprintf(stderr,"Close failed\n");
        exit(1);
    }
}

void recv_print_str(int fd){
    char buff[BUFF_SIZE];
    recv(fd, buff, BUFF_SIZE, MSG_DONTWAIT);
    printf("+++%s\n", buff);
    fflush(stdout);
}

void stdin_send_str(int fd){
    char buff[BUFF_SIZE];
    printf(">");
    fflush(stdout);
    fgets(buff, BUFF_SIZE, stdin);
    send(fd, buff, BUFF_SIZE, 0);
}

static volatile int running = 1;
void int_handler(int sig) {
    running = 0;
}

int main(){
    int sockfd=setup_server("127.0.0.1", PORT);
    int connfd=accept_new_client(sockfd);
    select(sockfd, &readmask, NULL, NULL, &timeout);
    while (running){
        recv_print_str(connfd);
        signal(SIGINT, int_handler);
        if(!running){
            break;
        }
        
        stdin_send_str(connfd);
        signal(SIGINT, int_handler);
        if(!running){
            break;
        }
    }
    close(sockfd);
}