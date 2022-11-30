#include "proxy.h"

int setup_client(char *ip, int port) {
    // remove stdout buffer
    setbuf(stdout, NULL);
    // create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        err(1, "Socket creation failed");
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
        err(1, "Socket creation failed");
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
        err(1, "Socket bind failed");
    } else {
        //printf("Socket successfully binded\n");
    }

    // listen to clients
    if ((listen(sockfd, 1000)) != 0) {
        err(1, "Listen failed");
    } else {
        //printf("Server listening\n");
    }
    return sockfd;
}

int accept_new_client(int sockfd) {
    int connfd = accept(sockfd, (struct sockaddr *)NULL,NULL);  // Acepta un nuevo cliente
    if (connfd < 0) {
        warn("Server accept failed");
    } else {
        //printf("Server accepts the client\n");
    }
    return connfd;
}

void close_connection(int sockfd) {
    if (close(sockfd) < 0) {
        warn("Socket close failed");
    } else {
        //printf("Socket successfully closed\n");
    }
}

void simple_recv(int sockfd, void *buffer, int buffer_size, int recv_flags) {
    int bytes_received = 0;
    while (bytes_received < buffer_size) {
        int bytes = recv(sockfd, buffer + bytes_received, buffer_size - bytes_received, recv_flags);
        if (bytes == -1) {
            err(1, "Recv failed");
        } else if (bytes == 0) {
            errx(1, "Connection closed");
        }
        bytes_received += bytes;
    }
}

void simple_send(int sockfd, void *buffer, int buffer_size, int send_flags) {
    int bytes_sent = 0;
    while (bytes_sent < buffer_size) {
        int bytes = send(sockfd, buffer + bytes_sent, buffer_size - bytes_sent, send_flags);
        if (bytes == -1) {
            err(1, "Send failed");
        } else if (bytes == 0) {
            errx(1, "Connection closed");
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
    clock_gettime(CLOCK_REALTIME, &ts);
    printf("[%ld.%ld] Publisher conectado con el broker correctamente.\n", ts.tv_sec, ts.tv_nsec);
    
    return connfd;
}

struct timespec diff_timespec(const struct timespec *time1, const struct timespec *time0) {
    struct timespec diff = {
        .tv_sec = time1->tv_sec - time0->tv_sec,
        .tv_nsec = time1->tv_nsec - time0->tv_nsec
    };
    if (diff.tv_nsec < 0) {
        diff.tv_nsec += 1000000000;
        diff.tv_sec--;
    }
    return diff;
}

int send_config_msg(int sockfd, enum operations action, char *topic, int id) {
    struct message msg;
    msg.action = action;
    msg.id = id;
    strncpy(msg.topic, topic, TOPIC_SIZE);
    int return_value=0;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    if(msg.action == REGISTER_PUBLISHER){
        simple_send(sockfd, &msg, sizeof(msg), 0);
        return_value=recv_response_msg(sockfd);
        if (return_value < 0) {
            errx(1, "[%ld.%ld] Error al hacer el registro: %s\n", ts.tv_sec, ts.tv_nsec, id == -1 ? "LIMIT" : "ERROR");
        }
        printf("[%ld.%ld] Registrado correctamente con ID: %d para topic %s\n", ts.tv_sec, ts.tv_nsec, id, topic);
    }
    else if(msg.action == REGISTER_SUBSCRIBER){
        simple_send(sockfd, &msg, sizeof(msg), 0);
        return_value=recv_response_msg(sockfd);
        if (return_value < 0) {
            errx(1, "[%ld.%ld] Error al hacer el registro: %s\n", ts.tv_sec, ts.tv_nsec, id == -1 ? "LIMIT" : "ERROR");
        }
        printf("[%ld.%ld] Registrado correctamente con ID: %d para topic %s \n", ts.tv_sec, ts.tv_nsec, id, topic);
    }
    else if(msg.action == UNREGISTER_PUBLISHER){
        simple_send(sockfd, &msg, sizeof(msg), 0);
        close_connection(sockfd);
        printf("[%ld.%ld] De-Registrado (%d) correctamente del broker.\n", ts.tv_sec, ts.tv_nsec, id);
    }
    else if(msg.action == UNREGISTER_SUBSCRIBER){
        simple_send(sockfd, &msg, sizeof(msg), 0);
        close_connection(sockfd);
        printf("[%ld.%ld] De-Registrado (%d) correctamente del broker.\n", ts.tv_sec, ts.tv_nsec, id);
    }
    else{
        errx(1,"[%ld.%ld] Error al enviar el mensaje de configuracion: invalid action %d\n", ts.tv_sec, ts.tv_nsec, msg.action);
    }
    return return_value;

}

void recv_client_msg(int sockfd, struct message *message) {
    simple_recv(sockfd, message, sizeof(struct message), 0);
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    if(message->action == REGISTER_PUBLISHER){
        send_response_msg(sockfd, STATUS_OK, 0);
        printf("[%ld.%ld] Nuevo cliente (%d) Publicador conectado : %s \n", ts.tv_sec, ts.tv_nsec, 0, message->topic);
    }
    else if(message->action == REGISTER_SUBSCRIBER){
        send_response_msg(sockfd, STATUS_OK, 0);
        printf("[%ld.%ld] Nuevo cliente (%d) Suscriptor conectado : %s \n", ts.tv_sec, ts.tv_nsec, 1, message->topic);
    }
    else if(message->action == UNREGISTER_PUBLISHER){
        close_connection(sockfd);
        printf("[%ld.%ld] Eliminado cliente (%d) Publicador : %s \n", ts.tv_sec, ts.tv_nsec, 0, message->topic);
    }
    else if(message->action == UNREGISTER_SUBSCRIBER){
        close_connection(sockfd);
        printf("[%ld.%ld] Eliminado cliente (%d) Suscriptor : %s \n", ts.tv_sec, ts.tv_nsec, 1, message->topic);
    }
    else if(message->action == PUBLISH_DATA){
        printf("[%ld.%ld] Recibido mensaje para publicar en topic: %s - mensaje: %s - Generó: %ld.%ld \n", ts.tv_sec, ts.tv_nsec, message->topic, message->data.data, message->data.time_generated_data.tv_sec, message->data.time_generated_data.tv_nsec);
    }
    else{
        warnx("Error: recv_client_msg() recv invalid action");
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
    if(data_size >= DATA_SIZE){
        data_size = DATA_SIZE-1;
        warnx("Error: send_publisher_msg() data_size too big, truncating to %d", data_size);
    }
    bzero(msg.data.data, DATA_SIZE);
    strncpy(msg.data.data, data, data_size);
    clock_gettime(CLOCK_REALTIME, &msg.data.time_generated_data);
    simple_send(sockfd, &msg, sizeof(msg), 0);
    printf("[%ld.%ld] Publicado mensaje topic: %s - mensaje: %s - Generó: %ld - %ld \n", msg.data.time_generated_data.tv_sec, msg.data.time_generated_data.tv_nsec, msg.topic, msg.data.data, msg.data.time_generated_data.tv_sec, msg.data.time_generated_data.tv_nsec);

}

void send_subscriber_msg(int sockfd, struct message *message){
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    printf("[%ld.%ld] Enviando mensaje en topic %s a %d suscriptores. \n", ts.tv_sec, ts.tv_nsec, message->topic, 1);
    simple_send(sockfd, &message->data, sizeof(message->data), 0);
}

void recv_subscriber_msg(int sockfd, struct publish *msg){
    simple_recv(sockfd, msg, sizeof(struct publish), 0);
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    struct timespec latency = diff_timespec(&ts, &msg->time_generated_data);
    printf("[%ld.%ld] Recibido mensaje topic: %s - mensaje: %s - Generó: %ld.%ld - Recibido: %ld.%ld - Latencia: %ld.%ld.\n", ts.tv_sec, ts.tv_nsec, "TODO", msg->data, msg->time_generated_data.tv_sec, msg->time_generated_data.tv_nsec, ts.tv_sec, ts.tv_nsec, latency.tv_sec, latency.tv_nsec);
}