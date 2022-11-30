#include "proxy.h"

int setup_client(char *ip, int port) {
    // remove stdout buffer
    setbuf(stdout, NULL);
    // create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        err(1, "Socket creation failed\n");
    } else {
        //printf("Socket successfully created\n");
    }
    // assign ip and port
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port = htons(port);

    // connect client to server
    if(CONNECT_RETRY){
        while ((connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) < 0) {
            warn("Connection with the server failed, retrying");
            sleep(1);
        }
    }
    else{
        if ((connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) < 0) {
            err(1, "Connection with the server failed");
        }
    }

    //printf("Client conected to server\n");
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
        //printf("Socket successfully created\n");
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
        //printf("Socket successfully binded\n");
    }

    // listen to clients
    if ((listen(sockfd, 1000)) != 0) {
        err(1, "Listen failed\n");
    } else {
        //printf("Server listening\n");
    }
    return sockfd;
}

int accept_new_client(int sockfd) {
    int connfd = accept(sockfd, (struct sockaddr *)NULL,NULL);  // Acepta un nuevo cliente
    if (connfd < 0) {
        warn("Server accept failed\n");
    } else {
        //printf("Server accepts the client\n");
    }
    return connfd;
}

void close_connection(int sockfd) {
    if (close(sockfd) < 0) {
        warn("Socket close failed\n");
    } else {
        //printf("Socket successfully closed\n");
    }
}

void simple_recv(int sockfd, void *buffer, int buffer_size, int recv_flags) {
    int bytes_received = 0;
    while (bytes_received < buffer_size) {
        int bytes = recv(sockfd, buffer + bytes_received, buffer_size - bytes_received, recv_flags);
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
        int bytes = send(sockfd, buffer + bytes_sent, buffer_size - bytes_sent, send_flags);
        if (bytes == -1) {
            err(1, "Send failed\n");
        } else if (bytes == 0) {
            err(1, "Connection closed\n");
        }
        bytes_sent += bytes;
    }
}

int setup_broker(int port) {
    int sockfd = setup_server(port);
    return sockfd;
}

int setup_subscriber(char *ip, int port) {
    int connfd = setup_client(ip, port);
    return connfd;
}

int setup_publisher(char *ip, int port) {
    int connfd = setup_client(ip, port);

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    printf("[%ld.%ld] Publisher conectado con el broker correctamente.\n", ts.tv_sec, ts.tv_nsec);
    
    return connfd;
}

void send_config_msg(int sockfd, enum operations action, char *topic, int id) {
    struct message msg;
    if(action == PUBLISH_DATA){
        //error
        errx(1, "Error: send_config_msg() called with PUBLISH_DATA action");
    }
    msg.action = action;
    msg.id = id;
    strncpy(msg.topic, topic, TOPIC_SIZE);
    simple_send(sockfd, &msg, sizeof(msg), 0);
}

void recv_client_msg(int sockfd, struct message *message) {
    simple_recv(sockfd, message, sizeof(struct message), 0);
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    //printf("[%ld.%ld] Nuevo cliente (%d) Publicador conectado : %s \n", ts.tv_sec, ts.tv_nsec, 0, msg.topic);
    if(message->action == REGISTER_PUBLISHER){
        printf("[%ld.%ld] Nuevo cliente (%d) Publicador conectado : %s \n", ts.tv_sec, ts.tv_nsec, 0, message.topic);
    }
    else if(message->action == REGISTER_SUBSCRIBER){
        printf("[%ld.%ld] Nuevo cliente (%d) Suscriptor conectado : %s \n", ts.tv_sec, ts.tv_nsec, 1, message.topic);
    }
    else if(message->action == UNREGISTER_PUBLISHER){
        printf("[%ld.%ld] Eliminado cliente (%d) Publicador : %s \n", ts.tv_sec, ts.tv_nsec, 0, message.topic);
    }
    else if(message->action == UNREGISTER_SUBSCRIBER){
        printf("[%ld.%ld] Eliminado cliente (%d) Suscriptor : %s \n", ts.tv_sec, ts.tv_nsec, 1, message.topic);
    }
    else if(message->action == PUBLISH_DATA){
        printf("[%ld.%ld] Recibido mensaje para publicar en topic: %s - mensaje: %s - Generó: %ld.%ld \n", ts.tv_sec, ts.tv_nsec, msg.topic, msg.data.data, msg.data.time_generated_data.tv_sec, msg.data.time_generated_data.tv_nsec);
    }
    else{
        warnx(1, "Error: recv_client_msg() recv invalid action");
    }
}

void send_response_msg(int sockfd, enum status response_status, int id) {
    struct response msg;
    msg.response_status = response_status;
    msg.id = id;
    simple_send(sockfd, &msg, sizeof(msg), 0);
}

int recv_response_msg(int sockfd) {
    struct response message;
    simple_recv(sockfd, &message, sizeof(struct response), 0);

    if(message.response_status == STATUS_LIMIT){
        return -1;
    }
    else if(message.response_status == STATUS_ERROR){
        return -2;
    }
    return message.id;
}

void send_publisher_msg(int sockfd, char *topic, char *data, int data_size) {
    struct message msg;
    msg.action = PUBLISH_DATA;
    msg.id = 0;
    strncpy(msg.topic, topic, TOPIC_SIZE);
    strncpy(msg.data.data, data, data_size);
    clock_gettime(CLOCK_REALTIME, &msg.data.time_generated_data);
    simple_send(sockfd, &msg, sizeof(msg), 0);
}
void recv_publisher_msg(int sockfd, struct message *message) {
    simple_recv(sockfd, message, sizeof(struct message), 0);
}

void send_subscriber_msg(int sockfd, struct publish *publish){
    simple_send(sockfd, &publish, sizeof(publish), 0);
}

void recv_subscriber_msg(int sockfd, struct publish *publish){
    simple_recv(sockfd, publish, sizeof(struct publish), 0);
}