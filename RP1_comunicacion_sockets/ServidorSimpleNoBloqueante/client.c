#include "proxy.h"
#include <signal.h>//SIGINT
#include "time.h"

enum{
    PORT=8086
};

static volatile int running = 1;
void int_handler(int sig) {
    running = 0;
}

int main(){
    setbuf(stdout, NULL);
    int sockfd;
    sockfd=setup_client("127.0.0.1",PORT);
    while (running){
        send_recv(sockfd);
        signal(SIGINT, int_handler);
        if(!running){
            break;
        }
    }
    close_client(sockfd);
}
