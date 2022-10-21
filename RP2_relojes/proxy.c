#include "proxy.h"

const char client_names[NAME_SIZE][N_CLIENTS] = {CLIENT0_NAME, CLIENT1_NAME};
char my_name[NAME_SIZE];
char my_ip[16];
unsigned int my_port;
int my_sockfd;
int client_connfds[N_CLIENTS];

/*
void print_event(char*p_name, int lamport ,int8_t is_recv, enum operations
action){ char action_name[50]; switch (action){ case READY_TO_SHUTDOWN:
            strncpy(action_name,"READY_TO_SHUTDOWN",50);
            break;
        case SHUTDOWN_NOW:
            strncpy(action_name,"SHUTDOWN_NOW",50);
            break;
        case SHUTDOWN_ACK:
            strncpy(action_name,"SHUTDOWN_ACK",50);
            break;
    }

    if(is_recv){
        //PX, contador_lamport, SEND, operations
        printf("%s, %d, SEND, %s",my_name,lamport,action_name);
    }
    else{
        //PX, contador_lamport, RECV (PY), operations
        printf("%s, %d, RECV (%s), %s",my_name,lamport,p_name,action_name);
    }
}
*/

// Establece el nombre del proceso (para los logs y trazas)
void set_name(char name[2]) {
    bzero(my_name, NAME_SIZE);
    memcpy(my_name, name, 2);
}
// Establecer ip y puerto (para los logs y trazas)
void set_ip_port(char *ip, unsigned int port) {
    strncpy(my_ip, ip, IP_SIZE - 1);
    my_name[IP_SIZE - 1] = '\0';
    my_port = port;
    if (0 == strncmp(my_name, SERVER_NAME, NAME_SIZE)) {
        my_sockfd = setup_server(port);
    } else {
        my_sockfd = setup_client(ip, port);
    }
}

int setup_client(char *ip, int port) {
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