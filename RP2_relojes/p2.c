#include "proxy.h"

enum{
    N_CLIENTS=2
};

int local_lamport_counter;

//nonblock
int8_t recv_ready_shutdown(int connfd, char * pname, int pname_size){
    struct message msg;
    int ret_recv = recv(connfd, &msg, sizeof(msg), MSG_DONTWAIT);
    //if(ret_recv);
    //if(msg.action==READY_TO_SHUTDOWN);
    //strncpy(pname,msg.origin,pname_size);
    return 1;
}

int8_t recv_shutdown_ack(int connfd){
    struct message msg;
    int ret_recv = recv(connfd, &msg, sizeof(msg), MSG_DONTWAIT);
    //if(ret_recv);
    //if(msg.action==READY_TO_SHUTDOWN);
    return 1;
}

void send_shutdown_now(int connfd){
    struct message msg;
    strncpy(msg.origin,my_name,PNAME_SIZE);
    //msg.clock_lamport=local_lamport_counter;
    msg.action=SHUTDOWN_NOW;
    send(connfd,&msg,sizeof(msg),0);
}

int8_t everyone_ready(int8_t * is_ready, int is_ready_size){
    int8_t result=1;
    for(int i=0; i<is_ready_size; i++){
        result=result&is_ready[i];
    }
    return result;
}

int main(){
    set_name("p2");
    set_ip_port("127.0.0.1",8080);
    int sockfd=setup_server(8080);

    int connfd[N_CLIENTS];
    char p_name[N_CLIENTS][PNAME_SIZE];
    int8_t is_ready[N_CLIENTS];
    bzero(is_ready,sizeof(is_ready));

    //accept all clients
    for(int i=0; i<N_CLIENTS; i++){
        connfd[i]=accept_new_client(sockfd);
    }

    //recv ready to shutdown from everyone
    while(!everyone_ready(is_ready,sizeof(is_ready))){
        for(int i=0; i<N_CLIENTS; i++){
            if(!is_ready[i]){
                is_ready[i]=recv_ready_shutdown(connfd[i],p_name[i],sizeof(p_name[i]));
            }
        }
    }

    const char names_order[N_CLIENTS][PNAME_SIZE]={"p1","p2"};

    for(int i=0; i<N_CLIENTS; i++){
        for(int j=0; j<N_CLIENTS; j++){
            if(0==strncmp(names_order[i],p_name[j],PNAME_SIZE)){
                send_shutdown_now(connfd[j]);
                recv_shutdown_ack(connfd[j]);
            }
        }
    }

    for(int i=0; i<N_CLIENTS; i++){
        close(connfd[i]);
    }

    close_server(sockfd);
    return 0;
}