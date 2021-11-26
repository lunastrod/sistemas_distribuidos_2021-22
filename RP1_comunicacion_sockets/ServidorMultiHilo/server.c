#include "proxy.h"
#include <signal.h>//SIGINT

enum{
    PORT=8080
};

static volatile int running = 1;
void int_handler(int sig) {
    running = 0;
}

int main(){
    int sockfd=setup_server(PORT);
    int connfd=accept_new_client(sockfd);
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