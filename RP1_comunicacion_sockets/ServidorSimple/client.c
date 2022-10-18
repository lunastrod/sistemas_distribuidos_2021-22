#include "proxy.h"
#include <signal.h> //SIGINT

enum
{
    PORT = 8080
};

static volatile int running = 1;
void int_handler(int sig)
{
    running = 0;
}

void recv_print_str(int sockfd)
{
    char buff[BUFF_SIZE];
    simple_recv(sockfd, buff, BUFF_SIZE);
    printf("+++%s\n", buff);
}

void stdin_send_str(int sockfd)
{
    char buff[BUFF_SIZE];
    printf(">");
    fgets(buff, BUFF_SIZE, stdin);
    simple_send(sockfd, buff, BUFF_SIZE);
}

int main()
{
    int sockfd;
    sockfd = setup_client("212.128.254.141", PORT);

    while (running)
    {

        stdin_send_str(sockfd);
        signal(SIGINT, int_handler);
        if (!running)
        {
            break;
        }

        recv_print_str(sockfd);
        signal(SIGINT, int_handler);
        if (!running)
        {
            break;
        }
    }
    close_client(sockfd);
}
