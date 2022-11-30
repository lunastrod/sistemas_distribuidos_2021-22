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
                    warnx("Invalid port: %s (valid ports: 1024-65535)\n", optarg);
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
                    warnx("Invalid priority: %s (valid priorities: reader, writer)\n", optarg);
                }
                break;
            case 't':
                args->ratio = atoi(optarg);
                if (args->ratio < 1) {
                    warnx("Invalid ratio: %s (valid ratios: > 0)\n", optarg);
                    warnx("Using default ratio\n");
                    args->ratio = -1;
                }
                break;
            default:
                abort();
        }
    }
    // Check arguments
    if (args->port == -1 || args->priority == -1) {
        warnx("Usage: %s --port <port> --priority <priority> [--ratio <ratio>]\n", argv[0]);
        exit(1);
    }
}

void *server_thread(void *args) {
    //socket
    struct thread_args *thread_args = (struct thread_args *)args;
    while (1) {
        int connfd = accept_new_client(thread_args->sockfd);
        struct request req;
        recv_request(connfd, &req);
        long time_waiting;
        int counter = safe_access_counter(&time_waiting, req.action, req.id);
        send_response(connfd, req.action, counter, time_waiting);
        close_socket(connfd);
    }

    warnx("Thread %d finished\n", thread_args->id);
    return NULL;
    
}

int main(int argc, char **argv) {
    struct main_args args;
    parse_args(argc, argv, &args);
    //printf("port: %d, priority: %d, ratio: %d\n", args.port, args.priority, args.ratio);
    int sockfd = setup_server(args.port);

    init_counter(args.ratio, args.priority);

    // Create threads
    pthread_t threads[MAX_SERVER_THREADS];
    struct thread_args thread_args[MAX_SERVER_THREADS];
    for (int i = 0; i < MAX_SERVER_THREADS; i++) {
        thread_args[i].sockfd = sockfd;
        thread_args[i].id = i;
        pthread_create(&threads[i], NULL, server_thread, &thread_args[i]);
    }
    // Wait for threads
    for (int i = 0; i < MAX_SERVER_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}