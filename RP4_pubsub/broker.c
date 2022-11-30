#include <getopt.h>

#include "proxy.h"

//./broker --port $BROKER_PORT --mode $MODE

enum broker_mode {
    MODE_SECUENTIAL=0,
    MODE_PARALLEL=1,
    MODE_FAIR=2
};

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
                // modes: secuencial, paralelo, justo
                if (strcmp(optarg, "secuencial") == 0) {
                    args->mode = MODE_SECUENTIAL;
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
    int pub_connfd=accept_new_client(sockfd);
    int sub_connfd=accept_new_client(sockfd);

    struct message msg;
    struct timespec ts;
    recv_config_msg(pub_connfd, &msg);
    send_response_msg(pub_connfd, STATUS_OK, 0);

    recv_config_msg(sub_connfd, &msg);
    send_response_msg(sub_connfd, STATUS_OK, 1);

    recv_publisher_msg(pub_connfd, &msg);
    clock_gettime(CLOCK_REALTIME, &ts);
    printf("[%ld.%ld] Recibido mensaje para publicar en topic: %s - mensaje: %s - Generó: %ld.%ld \n", ts.tv_sec, ts.tv_nsec, msg.topic, msg.data.data, msg.data.time_generated_data.tv_sec, msg.data.time_generated_data.tv_nsec);
    send_subscriber_msg(sub_connfd, &msg.data);
    clock_gettime(CLOCK_REALTIME, &ts);
    printf("[%ld.%ld] Enviando mensaje en topic %s a %d suscriptores. \n", ts.tv_sec, ts.tv_nsec, msg.topic, 1);

    recv_config_msg(pub_connfd, &msg);
    close_connection(pub_connfd);

    recv_config_msg(sub_connfd, &msg);
    close_connection(sub_connfd);

    close_connection(sockfd);
    return 0;
}