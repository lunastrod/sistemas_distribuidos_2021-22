#include "proxy.h"

/*
1. P1 y P3 notifican a P2 que están listos para apagarse (READY_TO_SHUTDOWN)
2. P2 recibe los mensajes y envía a P1 la orden de apagarse. (SHUTDOWN_NOW)
3. P1 recibe el mensaje y envía a P2 el ACK de apagado. (SHUTDOWN_ACK)
4. P2 recibe el mensaje y envía a P3 la orden de apagarse. (SHUTDOWN_NOW)
5. P3 recibe el mensaje y envía a P2 el ACK de apagado. (SHUTDOWN_ACK)
6. P2 recibe el mensaje
*/

/*
void *thread_comm(void *arg) {
    int sockfd = *((int *)arg);

    int connfdp1 = accept_new_client(sockfd);
    int connfdp3 = accept_new_client(sockfd);

    char pname[NAME_SIZE];
    // p1 and p3 are ready to shutdown
    recv_ready_shutdown(connfdp1, pname);
    recv_ready_shutdown(connfdp3, pname);

    if (strcmp(pname, "P1") == 0) {
        // swap connfdp1 and connfdp3
        int tmp = connfdp1;
        connfdp1 = connfdp3;
        connfdp3 = tmp;
    }

    // send shutdown now to p1
    send_shutdown_now(connfdp1);
    recv_shutdown_ack(connfdp1);

    // send shutdown now to p3
    send_shutdown_now(connfdp3);
    recv_shutdown_ack(connfdp3);

    close_socket(connfdp1);
    close_socket(connfdp3);

    return NULL;
}
*/
/*^

int main(int argc, char *argv[]) {
    if (argc != 3) {
        errx(1, "usage: %s <ip> <port>", argv[0]);
    }

    set_name("P2");
    set_ip_port(argv[1], atoi(argv[2]));

    int sockfd = setup_server(my_port);
    pthread_t thread1;
    if (pthread_create(&thread1, NULL, thread_comm, &sockfd) != 0) {
        err(1, "pthread_create");
    }
    printf("thread created\n");
    if (pthread_join(thread1, NULL) != 0) {
        err(1, "pthread_join");
    }
    printf("Los clientes fueron correctamente apagados en t(lamport) = %d\n", get_clock_lamport());
    close_socket(sockfd);

    return 0;
}
*/

// design a program that implements the following scenario:
// 1. P1 and P3 send to P2 that they are ready to shutdown (READY_TO_SHUTDOWN) 
// 2. P2 receives the messages on lamport clock = 3 and sends to P1 the order to shutdown. (SHUTDOWN_NOW)
// 3. P1 receives the message on lamport clock = 4 and sends to P2 the ACK of shutdown. (SHUTDOWN_ACK)
// 4. P2 receives the message on lamport clock = 5 and sends to P3 the order to shutdown. (SHUTDOWN_NOW)
// 5. P3 receives the message on lamport clock = 6 and sends to P2 the ACK of shutdown. (SHUTDOWN_ACK)
// 6. P2 receives the message on lamport clock = 7
// use a while loop that checks the lamport clock and receives the messages in the correct orde
// use a thread to receive and send the messages from P1 and P3
// don't use a mutex to protect the critical section, assume that the lamport clock is atomic

struct thread_args {
    int connfd;
    int wait_until_lamport;
    struct message msg;
};

void * send(void *arg){
    struct thread_args *args = (struct thread_args *)arg;
    int connfd = args->connfd;
    int wait_until_lamport = args->wait_until_lamport;
    struct message msg = args->msg;
    while(get_clock_lamport() < wait_until_lamport);
    send_message(connfd, &msg);
    return NULL;
}

void * recv(void *arg){
    struct thread_args *args = (struct thread_args *)arg;
    int connfd = args->connfd;
    int wait_until_lamport = args->wait_until_lamport;
    struct message expected_msg = args->msg;
    struct message msg;
    while(get_clock_lamport() < wait_until_lamport);
    recv_message(connfd, &msg);
    if (msg.action != expected_msg.action) {
        errx(1, "expected message type %d, got %d", expected_msg.action, msg.action);
    }
    if (strcmp(msg.origin, expected_msg.origin) != 0) {
        errx(1, "expected message name %s, got %s", expected_msg.origin, msg.origin);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        errx(1, "usage: %s <ip> <port>", argv[0]);
    }

    set_name("P2");
    set_ip_port(argv[1], atoi(argv[2]));

    int sockfd = setup_server(my_port);
    int connfd1 = accept_new_client(sockfd);
    int connfd2 = accept_new_client(sockfd);

    struct thread_args args1;
    args1.connfd = connfd1;
    args1.wait_until_lamport = 3;
    args1.msg.action = READY_TO_SHUTDOWN;
    strcpy(args1.msg.origin, "P2");
    pthread_t thread1;
    if (pthread_create(&thread1, NULL, recv, &args1) != 0) {
        err(1, "pthread_create");
    }

    struct thread_args args2;
    args2.connfd = connfd2;
    args2.wait_until_lamport = 3;
    args2.msg.action = READY_TO_SHUTDOWN;
    strcpy(args2.msg.origin, "P2");
    pthread_t thread2;
    if (pthread_create(&thread2, NULL, recv, &args2) != 0) {
        err(1, "pthread_create");
    }

            
