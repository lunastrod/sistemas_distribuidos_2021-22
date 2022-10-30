#include "proxy.h"

// 1. P1 and P3 send to P2 that they are ready to shutdown (READY_TO_SHUTDOWN)
// 2. P2 receives the messages on lamport clock = 3 and sends to P1 the order to shutdown. (SHUTDOWN_NOW)
// 3. P1 receives the message on lamport clock = 4 and sends to P2 the ACK of shutdown. (SHUTDOWN_ACK)
// 4. P2 receives the message on lamport clock = 5 and sends to P3 the order to shutdown. (SHUTDOWN_NOW)
// 5. P3 receives the message on lamport clock = 6 and sends to P2 the ACK of shutdown. (SHUTDOWN_ACK)
// 6. P2 receives the message on lamport clock = 7
// use a while loop that checks the lamport clock and receives the messages in the correct order
// use a thread to receive and send the messages from P1 and P3
// don't use a mutex to protect the critical section, assume that the lamport clock is atomic
void *thread_comm(void *arg) {
    int sockfd = *((int *)arg);
    int connfdp1 = accept_new_client(sockfd);
    int connfdp3 = accept_new_client(sockfd);
    int lcl = 0;
    while (get_clock_lamport() < 11) {
        struct message msg;
        //printf("P2: lamport clock = %d\n", get_clock_lamport());
        // wait for 0.1 seconds
        usleep(100000);
        lcl = get_clock_lamport();
        switch (lcl) {
            case 0:  // p1 and p3 send ready to shutdown
                recv_message(connfdp1, &msg);
                if (msg.action != READY_TO_SHUTDOWN) {
                    errx(1, "expected message type %d, got %d", READY_TO_SHUTDOWN, msg.action);
                }
                break;
            case 1:  // p1 and p3 send ready to shutdown
            case 2:  // p2 receives ready to shutdown from p1 and p3
                recv_message(connfdp3, &msg);
                if (msg.action != READY_TO_SHUTDOWN) {
                    errx(1, "expected message type %d, got %d", READY_TO_SHUTDOWN, msg.action);
                }
                if (strcmp(msg.origin, "P1") == 0) {
                    // swap connfdp1 and connfdp3
                    int tmp = connfdp1;
                    connfdp1 = connfdp3;
                    connfdp3 = tmp;
                }
                break;
            case 3:  // p2 receives ready to shutdown from p1/p3
                msg.action = SHUTDOWN_NOW;
                strncpy(msg.origin, "P2", NAME_SIZE);
                send_message(connfdp1, &msg);
                break;
            case 4:  // p2 sends shutdown now to p1
            case 5:  // p1 receives shutdown now from p2
            case 6:  // p1 sends shutdown ack to p2
                recv_message(connfdp1, &msg);
                if (msg.action != SHUTDOWN_ACK) {
                    errx(1, "expected message type %d, got %d", SHUTDOWN_ACK, msg.action);
                }
                break;
            case 7:  // p2 receives shutdown ack from p1
                msg.action = SHUTDOWN_NOW;
                strncpy(msg.origin, "P2", NAME_SIZE);
                send_message(connfdp3, &msg);
                break;
            case 8:   // p2 sends shutdown now to p3
            case 9:   // p3 receives shutdown now from p2
            case 10:  // p3 sends shutdown ack to p2
                recv_message(connfdp3, &msg);
                if (msg.action != SHUTDOWN_ACK) {
                    errx(1, "expected message type %d, got %d", SHUTDOWN_ACK, msg.action);
                }
                break;
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
    return 0;
}
