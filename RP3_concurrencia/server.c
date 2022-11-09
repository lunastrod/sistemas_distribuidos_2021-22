#include <getopt.h>

#include "counter.h"
#include "proxy.h"

/*
make fresh && ./server --port 8080 --priority reader --ratio 1
make fresh && ./server --port 8080 --priority writer --ratio 1
*/

struct main_args {
    int port;      // required
    int priority;  // required
    int ratio;     // oprtional
};

struct thread_args {
    int sockfd;
    int priority;
    int ratio;
    int id;
};

void parse_args(int argc, char **argv, struct main_args *args) {
    // Default values
    args->port = -1;
    args->priority = -1;
    args->ratio = -1;
    // Parse arguments
    int c;
    while (1) {
        static struct option long_options[] = {
            {"port", required_argument, 0, 'p'},
            {"priority", required_argument, 0, 'r'},
            {"ratio", required_argument, 0, 't'},
            {0, 0, 0, 0}};
        int option_index = 0;
        c = getopt_long(argc, argv, "p:r:t:", long_options, &option_index);
        if (c == -1) break;
        switch (c) {
            case 'p':
                args->port = atoi(optarg);
                if (args->port < 1024 || args->port > 65535) {
                    printf("Invalid port: %s (valid ports: 1024-65535)\n", optarg);
                    args->port = -1;
                }
                break;
            case 'r':
                // modes: reader, writer
                if (strcmp(optarg, "reader") == 0) {
                    args->priority = READ;
                } else if (strcmp(optarg, "writer") == 0) {
                    args->priority = WRITE;
                } else {
                    printf("Invalid priority: %s (valid priorities: reader, writer)\n", optarg);
                }
                break;
            case 't':
                args->ratio = atoi(optarg);
                if (args->ratio < 1) {
                    printf("Invalid ratio: %s (valid ratios: > 0)\n", optarg);
                    printf("Using default ratio\n");
                    args->ratio = -1;
                }
                break;
            default:
                abort();
        }
    }
    // Check arguments
    if (args->port == -1 || args->priority == -1) {
        printf("Usage: %s --port <port> --priority <priority> [--ratio <ratio>]\n", argv[0]);
        exit(1);
    }
}

void *server_thread(void *args) {
    struct thread_args *thread_args = (struct thread_args *)args;
    printf("Thread %d started\n", thread_args->id);
    int connfd = accept_new_client(thread_args->sockfd);
    struct request req;
    recv_request(connfd, &req);
    long time_waiting;
    int counter;
    if (req.action == READ) {
        counter = read_counter(connfd, &time_waiting);
    } else if (req.action == WRITE) {
        counter = increment_counter(connfd, &time_waiting);
    } else {
        printf("Invalid request\n");
    }

    send_response(connfd, req.action, counter, time_waiting);
    printf("Thread %d finished\n", thread_args->id);
    return NULL;
}

int main(int argc, char **argv) {
    struct main_args args;
    parse_args(argc, argv, &args);
    printf("port: %d, priority: %d, ratio: %d\n", args.port, args.priority, args.ratio);
    int sockfd = setup_server(args.port);

    // Create threads
    pthread_t threads[MAX_SERVER_THREADS];
    struct thread_args thread_args[MAX_SERVER_THREADS];
    for (int i = 0; i < MAX_SERVER_THREADS; i++) {
        thread_args[i].sockfd = sockfd;
        thread_args[i].priority = args.priority;
        thread_args[i].ratio = args.ratio;
        thread_args[i].id = i;
        pthread_create(&threads[i], NULL, server_thread, &thread_args[i]);
    }
    printf("Created %d threads\n", MAX_SERVER_THREADS);
    // Wait for threads
    for (int i = 0; i < MAX_SERVER_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("All threads finished\n");
    return 0;
}