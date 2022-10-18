#include "proxy.h"

#include <pthread.h>
#include <string.h>

enum
{
    MAX_THREADS = 100
};

void *server_thread(void *arg)
{
    int sockfd = *(int *)arg;
    free(arg);
    arg = NULL;

    while (1)
    {
        int connfd = accept_new_client(sockfd);

        char buff[BUFF_SIZE];
        while(1)
        {
            simple_recv(connfd, buff, sizeof(buff) - 1, 0);
            printf("+++%s\n", buff);
            fflush(stdout);
            simple_send(connfd, "Hello client!", sizeof("Hello client!"), 0);
        }
    }

    return NULL;
}

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    int sockfd = setup_server(atoi(argv[1]));

    pthread_t threads[MAX_THREADS];

    for (int i = 0; i < MAX_THREADS; i++)
    {
        int *arg = malloc(sizeof(arg));
        *arg = sockfd;

        if (pthread_create(&threads[i], NULL, server_thread, (void *)arg))
        {
            warn("error pthread_create");
        }
    }

    for (int i = 0; i < MAX_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }
}