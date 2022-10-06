#include "proxy.h"

#include <pthread.h>
#include <string.h>

enum
{
    PORT = 8081,
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
        simple_recv(connfd, buff, sizeof(buff) - 1, 0);
        simple_send(connfd, "Hello client!", sizeof("Hello client!"), 0);
        close(connfd);
    }

    return NULL;
}

int main()
{
    int sockfd = setup_server(PORT);
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