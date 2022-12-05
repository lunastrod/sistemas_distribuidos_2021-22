// std
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <signal.h>
#include <getopt.h>

#include "proxy.h"
//./publisher --ip $BROKER_IP --port $BROKER_PORT --topic $TOPIC

struct main_args {
    char *ip;
    int port;
    char *topic;
};

void get_cpu_load(char * load) {
  FILE* file = fopen("/proc/loadavg", "r");

  float avg1, avg5, avg15;

  fscanf(file, "%f %f %f", &avg1, &avg5, &avg15);
  fclose(file);

  snprintf(load, 100, "cpu load:  1m: %.2f%%  5m: %.2f%%  15: %.2f%%", avg1, avg5, avg15);
}

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
void sigint_handler(int signum) {
  sigint_received = 1;
}

int main(int argc, char **argv) {
    struct main_args args;
    parse_args(argc, argv, &args);

    int connfd = setup_publisher(args.ip, args.port);
    int id = send_config(connfd, REGISTER_PUBLISHER, args.topic, 0);

    signal(SIGINT, sigint_handler);
    while(!sigint_received){
        char load[100];
        get_cpu_load(load);
        publish(connfd, args.topic, load, strlen(load));
        for(int i=0; i<3 && !sigint_received; i++){//sleep 3 seconds checking for SIGINT
            sleep(1);
        }
    }

    send_config(connfd, UNREGISTER_PUBLISHER, args.topic, id);
    return 0;
}