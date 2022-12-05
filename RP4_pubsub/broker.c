// std
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <getopt.h>
// threads
#include <pthread.h>

#include "proxy.h"



//./broker --port $BROKER_PORT --mode $MODE

struct main_args {
    int port;
    enum broker_mode mode;
};

void parse_args(int argc, char **argv, struct main_args *args) {
    // Default values
    args->port = -1;
    args->mode = -1;

    // Parse arguments
    int c;
    while (1) {
        static struct option long_options[] = {
            {"port", required_argument, 0, 'p'},
            {"mode", required_argument, 0, 'm'},
            {0, 0, 0, 0}};
        int option_index = 0;
        c = getopt_long(argc, argv, "i:p:t:", long_options, &option_index);
        if (c == -1) break;
        switch (c) {
            case 'p':
                args->port = atoi(optarg);
                if(args->port < 1024 || args->port > 65535){
                    warnx("Invalid port: %s (valid ports: 1024-65535)\n", optarg);
                    args->port = -1;
                }
                break;
            case 'm':
                if (strcmp(optarg, "secuencial") == 0) {
                    args->mode = MODE_SEQUENTIAL;
                } 
                else if (strcmp(optarg, "paralelo") == 0) {
                    args->mode = MODE_PARALLEL;
                } 
                else if (strcmp(optarg, "justo") == 0) {
                    args->mode = MODE_FAIR;
                } 
                else {
                    warnx("Invalid mode: %s (valid modes: secuencial, paralelo, justo)", optarg);
                    args->mode = -1;
                }
                break;
            default:
                break;
        }
    }
    if (args->port == -1 || args->mode == -1) {
        warnx("Usage: ./broker --port <BROKER_PORT> --mode <MODE>");
        exit(1);
    }
}

int main(int argc, char **argv) {
    struct main_args args;
    parse_args(argc, argv, &args);

    int sockfd=setup_broker(args.port);

    struct client_list clients;
    init_client_list(&clients);
    
    struct broker_threads threads;
    //create the accept thread
    //this thread will accept new connections and add them to the client list
    struct accept_thread_args accept_args;
    accept_args.sockfd = sockfd;
    accept_args.cl = &clients;
    pthread_create(&threads.accept_thread, NULL, accept_thread, &accept_args);

    //create the fordwarder thread
    //this thread will forward messages from the publishers to the subscribers
    struct fordwarder_thread_args fordwarder_args;
    fordwarder_args.cl = &clients;
    fordwarder_args.mode = args.mode;
    pthread_create(&threads.fordwarder_thread, NULL, fordwarder_thread, &fordwarder_args);

    //create the subscriber thread
    //this thread will be in charge of attending the UNREGISTER_SUBSCRIBER messages
    struct subscriber_thread_args subscriber_args;
    subscriber_args.cl = &clients;
    pthread_create(&threads.subscriber_thread, NULL, subscriber_thread, &subscriber_args);

    pthread_join(threads.accept_thread, NULL);
    pthread_join(threads.fordwarder_thread, NULL);
    pthread_join(threads.subscriber_thread, NULL);
    warnx("threads joined");

    close_connection(sockfd);
    return 0;
}