#include "proxy.h"
#include <signal.h>//SIGINT
#include <sys/select.h>

enum{
    PORT=8086
};

static volatile int running = 1;
void int_handler(int sig) {
    running = 0;
}

int main(){
    setbuf(stdout, NULL);
    int sockfd=setup_server(PORT);
    int connfd=accept_new_client(sockfd);
    while (running){
        send_recv(connfd);
        signal(SIGINT, int_handler);
        if(!running){
            break;
        }
    }
    close(sockfd);
}

