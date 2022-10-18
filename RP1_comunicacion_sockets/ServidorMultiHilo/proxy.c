#include "proxy.h"

// connects client to server
// returns: int sockfd
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

// closes client
void close_client(int sockfd) {
    if (close(sockfd) == 1) {
        err(1, "Close failed\n");
    }
}

// setup server listen
// returns: int sockfd
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
    if ((listen(sockfd, 100)) != 0) {
        err(1, "Listen failed\n");
    } else {
        printf("Server listening\n");
    }
    return sockfd;
}

// returns: connfd
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

// closes server
void close_server(int sockfd) {
    if (close(sockfd) == 1) {
        err(1, "Close failed\n");
    }
}

void send_recv(int sockfd) {
    char buff[BUFF_SIZE];

    struct timeval tv = {0, 500000};  // tiempo de espera en el select
    fd_set fds;     // mascara, hay que resetearla cada vez que llamo a select
    FD_ZERO(&fds);  // pongo a 0 la mascara
    FD_SET(STDIN_FILENO, &fds);  // stdin
    FD_SET(sockfd, &fds);        // socket

    if (select(sockfd + 1, &fds, NULL, NULL, &tv) <
        0) {  // sokfd+1 comprueba todos los valores menores que ese, comprueba
              // stdin tambien
        err(1, "select error");
    }
    printf("\n>");
    fflush(stdout);
    if (FD_ISSET(STDIN_FILENO, &fds)) {  // if stdin
        int ret_read = read(STDIN_FILENO, buff, BUFF_SIZE - 1);
        if (ret_read < 0) {
            warn("read error");
            return;
        }
        buff[ret_read] = '\0';
        printf("send: %s", buff);
        simple_send(sockfd, buff, BUFF_SIZE, 0);
    }
    if (FD_ISSET(sockfd, &fds)) {  // if socket
        simple_recv(sockfd, buff, BUFF_SIZE - 1, MSG_DONTWAIT);
        printf("+++%s", buff);
        fflush(stdout);
    }
}

void simple_send(int sockfd, char *buffer, int buffer_size, int send_flags) {
    int count = 0;
    int total = 0;
    while ((count = send(sockfd, &buffer[total], buffer_size - total, 0)) > 0) {
        total += count;
        return;
    }
    if (count == -1) {
        perror("recv");  // error
    } else if (count == 0) {
        close_server(sockfd);  // socket cerrado, cierro yo tambien
    }
}

void simple_recv(int sockfd, char *buffer, int buffer_size, int recv_flags) {
    int count = 0;
    int total = 0;
    while ((count = recv(sockfd, &buffer[total], buffer_size - total, 0)) > 0) {
        total += count;
        return;
    }
    if (count == -1) {
        perror("recv");
        // close(sockfd);
    } else if (count == 0) {
        close_server(sockfd);
    }
}