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
