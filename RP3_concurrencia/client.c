#include <getopt.h>
#include "proxy.h"
#include "counter.h"


/*
make && ./client --ip 0.0.0.0 --port 8080 --mode writer --threads 1
make && ./client --ip 0.0.0.0 --port 8080 --mode reader --threads 1
*/

struct main_args {
    char *ip;
    int port;
    int mode;
    int threads;
};

struct thread_args {
    char *ip;
    int port;
    int mode;
    int id;
};

void parse_args(int argc, char **argv, struct main_args *args) {
    // Default values
    args->ip = NULL;
    args->port = -1;
    args->mode = -1;
    args->threads = -1;
    // Parse arguments
    int c;
    while (1) {
        static struct option long_options[] = {
            {"ip", required_argument, 0, 'i'},
            {"port", required_argument, 0, 'p'},
            {"mode", required_argument, 0, 'm'},
            {"threads", required_argument, 0, 'n'},
            {0, 0, 0, 0}};
        int option_index = 0;
        c = getopt_long(argc, argv, "i:p:m:n:", long_options, &option_index);
        if (c == -1) break;
        switch (c) {
            case 'i':
                args->ip = optarg;
                break;
            case 'p':
                args->port = atoi(optarg);
                if(args->port < 1024 || args->port > 65535){
                    printf("Invalid port: %s (valid ports: 1024-65535)\n", optarg);
                    args->port = -1;
                }
                break;
            case 'm':
                // modes: reader, writer
                if (strcmp(optarg, "reader") == 0) {
                    args->mode = READ;
                } 
                else if (strcmp(optarg, "writer") == 0) {
                    args->mode = WRITE;
                } 
                else {
                    printf("Invalid mode: %s (valid modes: reader, writer)\n", optarg);
                }
                break;
            case 'n':
                args->threads = atoi(optarg);
                if(args->threads < 1){
                    printf("Invalid number of threads: %s (valid numbers: > 0)\n", optarg);
                    printf("Using 1 thread\n");
                    args->threads = 1;
                }
                break;
            default:
                abort();
        }
    }
    // Check arguments
    if (args->ip == NULL || args->port == -1 || args->mode == -1 || args->threads == -1) {
        printf("Usage: %s --ip <ip> --port <port> --mode <mode> --threads <threads>", argv[0]);
        exit(1);
    }
}

void * client_thread(void *arg) {
    struct thread_args *args = (struct thread_args *) arg;
    printf("Thread %d started with mode %d\n", args->id, args->mode);
    int connfd = setup_client(args->ip, args->port);

    send_request(connfd, args->mode, args->id);

    struct response res;
    recv_response(connfd,&res, args->id);

    printf("Thread %d finished\n", args->id);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    struct main_args args;
    parse_args(argc, argv, &args);
    printf("ip: %s, port: %d, mode: %d, threads: %d\n", args.ip, args.port, args.mode, args.threads);
    
    pthread_t * threads = (pthread_t *) malloc(args.threads * sizeof(pthread_t));
    struct thread_args * thread_args = (struct thread_args *) malloc(args.threads * sizeof(struct thread_args));
    for (int i = 0; i < args.threads; i++) {
        thread_args[i].ip = args.ip;
        thread_args[i].port = args.port;
        thread_args[i].mode = args.mode;
        thread_args[i].id = i;
        pthread_create(&threads[i], NULL, client_thread, &thread_args[i]);
    }
    printf("Threads created\n");
    for (int i = 0; i < args.threads; i++) {
        pthread_join(threads[i], NULL);
    }
    free(threads);
    free(thread_args);
    printf("Threads joined\n");
    return 0;
}