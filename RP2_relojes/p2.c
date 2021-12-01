#include "proxy.h"

int local_lamport_counter=0;


/*
DONE 1. P1 y P3 notifican a P2 que están listos para apagarse (READY_TO_SHUTDOWN)
2. P2 recibe los mensajes y envía a P1 la orden de apagarse. (SHUTDOWN_NOW)
3. P1 recibe el mensaje y envía a P2 el ACK de apagado. (SHUTDOWN_ACK)
4. P2 recibe el mensaje y envía a P3 la orden de apagarse. (SHUTDOWN_NOW)
5. P3 recibe el mensaje y envía a P2 el ACK de apagado. (SHUTDOWN_ACK)
6. P2 recibe el mensaje
*/

void recv_ready_shutdown(){
    int client_counter=N_CLIENTS;
    while(client_counter>0){
        int new_connfd=accept_new_client(my_sockfd);
        struct message msg;
        int ret_recv = recv(new_connfd, &msg, sizeof(msg), MSG_DONTWAIT);
        if(ret_recv!=sizeof(msg)){
            warnx("recv failed, client discarded");
            continue;
        }
        //TODO:lamport;
        if(msg.action!=READY_TO_SHUTDOWN){
            warnx("invalid message format, client discarded");
            continue;
        }
        for(int i=0; i<N_CLIENTS; i++){
            if(0==strncmp(msg.origin,client_names[i],NAME_SIZE)){
                //client accepted
                client_connfds[i]=new_connfd;
                client_counter--;
                break;
            }
        }
    }
}

int8_t recv_shutdown_ack(int connfd){
    struct message msg;
    int ret_recv = recv(connfd, &msg, sizeof(msg), MSG_DONTWAIT);
    //if(ret_recv);
    //if(msg.action==READY_TO_SHUTDOWN);
    return 1;
}

void send_shutdown_now(char * name){
    int connfd=-1;
    for(int i=0; i<N_CLIENTS; i++){
        if(0==strncmp(name,client_names[i],NAME_SIZE)){
            connfd=client_connfds[i];
            break;
        }
    }
    if(connfd==-1){
        warn("send_shutdown_now: %s invalid client name",name);
        return;
    }
    
    //TODO: lamport
    print_event(name, 0 ,0, SHUTDOWN_NOW);

    
    struct message msg;
    strncpy(msg.origin,my_name,NAME_SIZE);
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

    //accept all clients
    //recv ready to shutdown from everyone
    recv_ready_shutdown();

    //P2 recibe los mensajes y envía a P1 la orden de apagarse. (SHUTDOWN_NOW)
    //P1 recibe el mensaje y envía a P2 el ACK de apagado. (SHUTDOWN_ACK)
    send_shutdown_now(CLIENT0_NAME);



    //P2 recibe el mensaje y envía a P3 la orden de apagarse. (SHUTDOWN_NOW)
    //P3 recibe el mensaje y envía a P2 el ACK de apagado. (SHUTDOWN_ACK)
    send_shutdown_now(CLIENT1_NAME);




    close_server();

    return 0;
}