#include "proxy.h"
#include <signal.h> //SIGINT
#include "time.h"



void send_msg(int sockfd, int id)
{

}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        printf("Usage: %s <id> <ip> <port>\n", argv[0]);
        exit(1);
    }
    setbuf(stdout, NULL);
    int sockfd;
    sockfd = setup_client(argv[2], atoi(argv[3]));

    char buff[BUFF_SIZE];

    snprintf(buff, BUFF_SIZE, "Hello server! From client: %s", argv[1]);
    simple_send(sockfd, buff, BUFF_SIZE, 0);

    simple_recv(sockfd, buff, sizeof(buff) - 1, 0);
    printf("+++%s [client %s]\n", buff, argv[1]);
    fflush(stdout);

    close_client(sockfd);
}
