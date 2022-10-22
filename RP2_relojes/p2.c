#include "proxy.h"

/*
1. P1 y P3 notifican a P2 que están listos para apagarse (READY_TO_SHUTDOWN)
2. P2 recibe los mensajes y envía a P1 la orden de apagarse. (SHUTDOWN_NOW)
3. P1 recibe el mensaje y envía a P2 el ACK de apagado. (SHUTDOWN_ACK)
4. P2 recibe el mensaje y envía a P3 la orden de apagarse. (SHUTDOWN_NOW)
5. P3 recibe el mensaje y envía a P2 el ACK de apagado. (SHUTDOWN_ACK)
6. P2 recibe el mensaje
*/

// preguntar al profesor si esta bien no usar el contador lamport aqui si respeto el orden de los mensajes

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