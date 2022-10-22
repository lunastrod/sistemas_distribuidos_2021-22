#include "proxy.h"

void *thread_comm(void *arg) {
    int connfd = *((int *)arg);

    send_ready_shutdown(connfd);
    recv_shutdown_now(connfd);
    send_shutdown_ack(connfd);

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        errx(1, "usage: %s <ip> <port>", argv[0]);
    }
    set_name("P3");
    set_ip_port(argv[1], atoi(argv[2]));

    int connfd = setup_client(my_ip, my_port);

    pthread_t thread1;
    if (pthread_create(&thread1, NULL, thread_comm, &connfd) != 0) {
        err(1, "pthread_create");
    }
    printf("thread created\n");
    if (pthread_join(thread1, NULL) != 0) {
        err(1, "pthread_join");
    }
    close_socket(connfd);
    return 0;
}