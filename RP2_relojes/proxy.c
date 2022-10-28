#include "proxy.h"

char my_name[NAME_SIZE];
char my_ip[16];
unsigned int my_port;
unsigned int local_clock_lamport;
/*
    the clock_lamport field is used to send the lamport counter.
    its value is incremented every time a message is sent.
    when a message is received, the value of the clock_lamport field is updated
    with the maximum value between the local clock_lamport and the received clock_lamport.
*/

int setup_client(char *ip, int port) {
    // remove stdout buffer
    setbuf(stdout, NULL);
    // create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        err(1, "Socket creation failed\n");
    } else {
        printf("Socket successfully created\n");
    }
    // assign ip and port
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port = htons(port);

    // connect client to server
    while ((connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) <
           0) {
        warn("Connection with the server failed, retrying");
        sleep(1);
    }
    printf("Client conected to server\n");
    return sockfd;
}

int setup_server(int port) {
    // remove stdout buffer
    setbuf(stdout, NULL);
    // create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        err(1, "Socket creation failed\n");
    } else {
        printf("Socket successfully created\n");
    }
    // assign ip and port
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    // bind to socket
    if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) {
        err(1, "Socket bind failed\n");
    } else {
        printf("Socket successfully binded\n");
    }

    // listen to clients
    if ((listen(sockfd, 1000)) != 0) {
        err(1, "Listen failed\n");
    } else {
        printf("Server listening\n");
    }
    return sockfd;
}

int accept_new_client(int sockfd) {
    int connfd = accept(sockfd, (struct sockaddr *)NULL,
                        NULL);  // Acepta un nuevo cliente
    if (connfd < 0) {
        warn("Server accept failed\n");
    } else {
        printf("Server accepts the client\n");
    }
    return connfd;
}

void close_socket(int sockfd) {
    if (close(sockfd) < 0) {
        warn("Socket close failed\n");
    } else {
        printf("Socket successfully closed\n");
    }
}

void simple_recv(int sockfd, void *buffer, int buffer_size, int recv_flags) {
    int bytes_received = 0;
    while (bytes_received < buffer_size) {
        int bytes = recv(sockfd, buffer + bytes_received,
                         buffer_size - bytes_received, recv_flags);
        if (bytes == -1) {
            err(1, "Recv failed\n");
        } else if (bytes == 0) {
            err(1, "Connection closed\n");
        }
        bytes_received += bytes;
    }
}

void simple_send(int sockfd, void *buffer, int buffer_size, int send_flags) {
    int bytes_sent = 0;
    while (bytes_sent < buffer_size) {
        int bytes = send(sockfd, buffer + bytes_sent, buffer_size - bytes_sent,
                         send_flags);
        if (bytes == -1) {
            err(1, "Send failed\n");
        } else if (bytes == 0) {
            err(1, "Connection closed\n");
        }
        bytes_sent += bytes;
    }
}

int maximum(int a1, int a2) {
    if (a1 > a2)
        return a1;
    else
        return a2;
}

void recv_ready_shutdown(int connfd, char *pname) {
    struct message msg;
    simple_recv(connfd, &msg, sizeof(struct message), 0);
    print_event(msg.origin, msg.clock_lamport, 1, READY_TO_SHUTDOWN);
    // update local clock
    local_clock_lamport = maximum(get_clock_lamport(), msg.clock_lamport) + 1;
    // update pname
    strcpy(pname, msg.origin);
}

void send_ready_shutdown(int connfd) {
    struct message msg;
    msg.action = READY_TO_SHUTDOWN;
    strncpy(msg.origin, my_name, NAME_SIZE);
    // update local clock
    msg.clock_lamport = get_clock_lamport();
    print_event(msg.origin, msg.clock_lamport, 0, SHUTDOWN_ACK);
    local_clock_lamport++;
    simple_send(connfd, &msg, sizeof(struct message), 0);
}

void recv_shutdown_now(int connfd) {
    struct message msg;
    simple_recv(connfd, &msg, sizeof(struct message), 0);
    print_event(msg.origin, msg.clock_lamport, 1, SHUTDOWN_NOW);
    // update local clock
    local_clock_lamport = maximum(get_clock_lamport(), msg.clock_lamport) + 1;
}

void send_shutdown_now(int connfd) {
    struct message msg;
    msg.action = SHUTDOWN_NOW;
    strncpy(msg.origin, my_name, NAME_SIZE);
    // update local clock
    msg.clock_lamport = get_clock_lamport();
    print_event(msg.origin, msg.clock_lamport, 0, SHUTDOWN_ACK);
    local_clock_lamport++;
    simple_send(connfd, &msg, sizeof(struct message), 0);
}

void recv_shutdown_ack(int connfd) {
    struct message msg;
    simple_recv(connfd, &msg, sizeof(struct message), 0);
    print_event(msg.origin, msg.clock_lamport, 1, SHUTDOWN_ACK);
    // update local clock
    local_clock_lamport = maximum(get_clock_lamport(), msg.clock_lamport) + 1;
}

void send_shutdown_ack(int connfd) {
    struct message msg;
    msg.action = SHUTDOWN_ACK;
    strncpy(msg.origin, my_name, NAME_SIZE);
    // update local clock
    msg.clock_lamport = get_clock_lamport();
    print_event(msg.origin, msg.clock_lamport, 0, SHUTDOWN_ACK);
    local_clock_lamport++;
    simple_send(connfd, &msg, sizeof(struct message), 0);
}

void print_event(char *p_name, int lamport, int8_t is_recv, enum operations action) {
    char action_name[50];
    char recv_text[50];
    switch (action) {
        case READY_TO_SHUTDOWN:
            strncpy(action_name, "READY_TO_SHUTDOWN", 50);
            break;
        case SHUTDOWN_NOW:
            strncpy(action_name, "SHUTDOWN_NOW", 50);
            break;
        case SHUTDOWN_ACK:
            strncpy(action_name, "SHUTDOWN_ACK", 50);
            break;
    }
    if (is_recv) {
        snprintf(recv_text, 50, "RECV (%s)", p_name);
    } else {
        strncpy(recv_text, "SENT", 50);
    }

    // printf("%s: message %s %s from %s with lamport %d\n", my_name, recv_text, action_name, p_name, lamport);

    printf("%s, %d, %s, %s\n", my_name, lamport, recv_text, action_name);
}

//writes name to my_name
void set_name(char *name) {
    strncpy(my_name, name, NAME_SIZE);
}

void set_ip_port(char *ip, unsigned int port) {
    strncpy(my_ip, ip, IP_SIZE);
    my_port = port;
}

int get_clock_lamport() {
    return local_clock_lamport;
}

void send_message(int connfd, struct message *msg) {
    // update local lamport clock
    local_clock_lamport++;
    simple_send(connfd, msg, sizeof(struct message), 0);
}

void recv_message(int connfd, struct message *msg) {
    simple_recv(connfd, msg, sizeof(struct message), 0);
    // update local lamport clock
    local_clock_lamport = maximum(get_clock_lamport(), msg->clock_lamport) + 1;
}