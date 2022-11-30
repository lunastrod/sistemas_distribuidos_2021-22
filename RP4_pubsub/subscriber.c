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



int main(int argc, char **argv) {
    struct main_args args;
    parse_args(argc, argv, &args);

    int connfd = setup_subscriber(args.ip, args.port);
    int id = send_config_msg(connfd, REGISTER_SUBSCRIBER, args.topic, 0);

    struct publish msg;
    recv_subscriber_msg(connfd, &msg);

    send_config_msg(connfd, UNREGISTER_SUBSCRIBER, args.topic, id);
    return 0;
}