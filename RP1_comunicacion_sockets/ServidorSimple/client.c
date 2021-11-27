#include "proxy.h"
#include <signal.h>//SIGINT

enum{
    PORT=8080
};

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

    while (running){

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
