#include "proxy.h"
#include <signal.h>//SIGINT
#include "time.h"

enum{
    PORT=8081
};

void send_msg(int sockfd, int id){
    char buff[BUFF_SIZE];
    snprintf(buff,BUFF_SIZE,"Hello server! From client: %d", id);
    send(sockfd, buff, BUFF_SIZE, 0);
}

int main(int argc, char ** argv){
    int sockfd;
    sockfd=setup_client("127.0.0.1",PORT);
    if(argc!=2){
        err(1,"usage: client id");
    }
    send_msg(sockfd,strtol(argv[1],NULL,10));
    char buff[BUFF_SIZE];
    recv(sockfd,buff,sizeof(buff)-1,0);
    printf("+++%s [client %s]\n",buff, argv[1]);
    fflush(stdout);
    close_client(sockfd);
}
