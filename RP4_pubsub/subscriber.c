// std
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <signal.h>
#include <getopt.h>

#include "proxy.h"

//./subscriber --ip $BROKER_IP --port $BROKER_PORT --topic $TOPIC

struct main_args {
    char *ip;
    int port;
    char *topic;
};

void parse_args(int argc, char **argv, struct main_args *args) {
    // Default values
    args->ip = NULL;
    args->port = -1;
    args->topic = NULL;
    // Parse arguments
    int c;
    while (1) {
        static struct option long_options[] = {
            {"ip", required_argument, 0, 'i'},
            {"port", required_argument, 0, 'p'},
            {"topic", required_argument, 0, 't'},
            {0, 0, 0, 0}};
        int option_index = 0;
        c = getopt_long(argc, argv, "i:p:t:", long_options, &option_index);
        if (c == -1) break;
        switch (c) {
            case 'i':
                args->ip = optarg;
                break;
            case 'p':
                args->port = atoi(optarg);
                if(args->port < 1024 || args->port > 65535){
                    warnx("Invalid port: %s (valid ports: 1024-65535)\n", optarg);
                    args->port = -1;
                }
                break;
            case 't':
                args->topic = optarg;
                break;
            default:
                break;
        }
    }
    if (args->ip == NULL || args->port == -1 || args->topic == NULL) {
        warnx("Usage: ./subscriber --ip <BROKER_IP> --port <BROKER_PORT> --topic <TOPIC>");
        exit(1);
    }
}



// Flag to indicate that SIGINT has been received
volatile sig_atomic_t sigint_received = 0;


// Signal handler for SIGINT
void sigint_handler(int signum){
    sigint_received=1;
}

int main(int argc, char **argv) {
    struct main_args args;
    parse_args(argc, argv, &args);

    int connfd = setup_subscriber(args.ip, args.port);
    int id = send_config(connfd, REGISTER_SUBSCRIBER, args.topic, 0);

    signal(SIGINT, sigint_handler);

    struct publish msg;
    while(!sigint_received){
        // Set up the timeout for select()
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        // Set up the file descriptor sets for select()
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(connfd, &read_fds);
        // Call select() to see if connfd has data available to be read
        int num_ready_fds = select(connfd + 1, &read_fds, NULL, NULL, &timeout);
        if (num_ready_fds > 0) {
            subscribe(connfd, args.topic, &msg);
        }
    }

    send_config(connfd, UNREGISTER_SUBSCRIBER, args.topic, id);
    return 0;
}