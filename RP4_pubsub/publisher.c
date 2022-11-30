#include <getopt.h>

#include "proxy.h"

//./publisher --ip $BROKER_IP --port $BROKER_PORT --topic $TOPIC

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

    int connfd = setup_publisher(args.ip, args.port);
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    printf("[%ld.%ld] Publisher conectado con el broker (%s:%d).\n", ts.tv_sec, ts.tv_nsec, args.ip, args.port);

    send_config_msg(connfd, REGISTER_PUBLISHER, args.topic, 0);
    int id = recv_response_msg(connfd);
    clock_gettime(CLOCK_REALTIME, &ts);
    if (id < 0) {
        fprintf(stderr,"[%ld.%ld] Error al hacer el registro: error=%s\n", ts.tv_sec, ts.tv_nsec, id == -1 ? "LIMIT" : "ERROR");
        exit(1);
    }
    printf("[%ld.%ld] Registrado correctamente con ID: %d para topic %s\n", ts.tv_sec, ts.tv_nsec, id, args.topic);

    char * text = "Hello World!";
    send_publisher_msg(connfd, args.topic, text, strlen(text));
    clock_gettime(CLOCK_REALTIME, &ts);
    printf("[%ld.%ld] Publicado mensaje topic: %s - mensaje: %s - Generó: %ld - %ld \n", ts.tv_sec, ts.tv_nsec, args.topic, text, ts.tv_sec, ts.tv_nsec);

    send_config_msg(connfd, UNREGISTER_PUBLISHER, args.topic, id);
    clock_gettime(CLOCK_REALTIME, &ts);
    printf("[%ld.%ld] De-Registrado (%d) correctamente del broker.\n", ts.tv_sec, ts.tv_nsec, id);

    close_connection(connfd);
    return 0;
}