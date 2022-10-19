#include "proxy.h"
#include <signal.h> //SIGINT

void recv_print_str(int sockfd)
{
    char buff[BUFF_SIZE];
    recv(sockfd, buff, BUFF_SIZE, 0);
    printf("+++%s\n", buff);
    fflush(stdout);
}

void stdin_send_str(int sockfd)
{
    char buff[BUFF_SIZE];
    printf(">");
    fflush(stdout);
    fgets(buff, BUFF_SIZE, stdin);
    send(sockfd, buff, BUFF_SIZE, 0);
}

enum
{
    PORT = 8080
};

static volatile int running = 1;
void int_handler(int sig)
{
    running = 0;
}

int main()
{
    setbuf(stdout, NULL);
    int sockfd = setup_server(PORT);
    int connfd = accept_new_client(sockfd);
    while (running)
    {
        recv_print_str(connfd);
        signal(SIGINT, int_handler);
        if (!running)
        {
            break;
        }

        stdin_send_str(connfd);
        signal(SIGINT, int_handler);
        if (!running)
        {
            break;
        }
    }
    close(sockfd);
}