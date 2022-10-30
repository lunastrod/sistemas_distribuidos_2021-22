#include "proxy.h"

void *thread_comm(void *arg) {
    int connfd = *((int *)arg);
    struct message msg;
    int lcl = 0;
    while (get_clock_lamport() < 10) {
        lcl = get_clock_lamport();
        switch (lcl) {
            case 0:
                msg.action = READY_TO_SHUTDOWN;
                strncpy(msg.origin, my_name, NAME_SIZE);
                // simple_send(connfd, &msg, sizeof(struct message), 0);
                send_message(connfd, &msg);
                break;
            case 1:  // p1 and p3 send ready to shutdown
            case 2:  // p2 receives ready to shutdown from p1/p3
            case 3:  // p2 receives ready to shutdown from p1/p3
            case 4:  // p2 sends shutdown now to p1
            case 5:  // p1 receives shutdown now from p2
            case 6:  // p1 sends shutdown ack to p2
            case 7:  // p2 receives shutdown ack from p1
            case 8:  // p2 sends shutdown now to p3
                recv_message(connfd, &msg);
                if (msg.action != SHUTDOWN_NOW) {
                    errx(1, "expected message type %d, got %d", READY_TO_SHUTDOWN, msg.action);
                }
                break;
            case 9:  // p3 receives shutdown now from p2
                msg.action = SHUTDOWN_ACK;
                strncpy(msg.origin, my_name, NAME_SIZE);
                // simple_send(connfd, &msg, sizeof(struct message), 0);
                send_message(connfd, &msg);
                break;
            case 10:  // p3 sends shutdown ack to p2
            case 11:  // p2 receives shutdown ack from p3
                break;
        }
    }
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