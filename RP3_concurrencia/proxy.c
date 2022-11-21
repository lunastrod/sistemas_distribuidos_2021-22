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

void close_socket(int sockfd) {
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

void send_request(int sockfd, enum operations action, unsigned int id) {
    struct request request;
    request.action = action;
    request.id = id;
    simple_send(sockfd, &request, sizeof(request), 0);
}

void recv_request(int sockfd, struct request *request) {
    simple_recv(sockfd, request, sizeof(*request), 0);
}

void send_response(int sockfd, enum operations action, unsigned int counter, long waiting_time) {
    struct response response;
    response.action = action;
    response.counter = counter;
    response.waiting_time = waiting_time;
    simple_send(sockfd, &response, sizeof(response), 0);
}

void recv_response(int sockfd, struct response *response, int client_id) {
    simple_recv(sockfd, response, sizeof(*response), 0);
}

void client_print(struct response *response, int id){
    if(response->action == READ){
        printf("[Cliente #%d] Lector, contador=%d, tiempo=%ld ns.\n", id, response->counter, response->waiting_time);
    }
    else if(response->action == WRITE){
        printf("[Cliente #%d] Escritor, contador=%d, tiempo=%ld ns.\n", id, response->counter, response->waiting_time);
    }
    else{
        warn("action not recognized");
    }
}