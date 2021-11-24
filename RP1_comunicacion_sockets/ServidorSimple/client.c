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

//connects client to server
//returns: int sockfd
int setup_client(char* ip, int port) {
    // create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        fprintf(stderr,"Socket creation failed\n" );
        exit(1);
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
        fprintf(stderr,"Connection with the server failed, retrying\n");
        sleep(1);
    }
    printf("Client conected to server\n");
    return sockfd;
}

//closes client
void close_client(int sockfd){
    if(close(sockfd) == 1) {
        fprintf(stderr,"Close failed\n");
        exit(1);
    }
}

static volatile int running = 1;
void int_handler(int sig) {
    running = 0;
}

void recv_print_str(int sockfd){
    char buff[BUFF_SIZE];
    recv(sockfd, buff, BUFF_SIZE, 0);
    printf("+++%s\n", buff);
    fflush(stdout);
}

void stdin_send_str(int sockfd){
    char buff[BUFF_SIZE];
    printf(">");
    fflush(stdout);
    fgets(buff, BUFF_SIZE, stdin);
    send(sockfd, buff, BUFF_SIZE, 0);
}

int main(){
    int sockfd;
    sockfd=setup_client("127.0.0.1",PORT);

    while (running){//CLIENT============================================================================================

        stdin_send_str(sockfd);
        signal(SIGINT, int_handler);
        if(!running){
            break;
        }

        recv_print_str(sockfd);
        signal(SIGINT, int_handler);
        if(!running){
            break;
        }
    }
    close_client(sockfd);
}
