#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <pthread.h>
#include "proxy.h"
#include <getopt.h>

enum{
    PRIORITY_READER,
    PRIORITY_WRITER
};


int main(int argc, char *argv[]){
    int priority = -1, ratio = -1, port = -1;

    int current_option, option_index;

    static struct option long_options[] = {
        {"priority ", required_argument, 0, 'm'},
        {"ratio ", required_argument, 0, 'r'},
        {"port ", required_argument, 0, 'p'},
        {0, 0, 0, 0}
    };


    while ((current_option =  getopt_long (argc, argv, "m:t:i:p::",long_options, &option_index)) != -1){
        switch (current_option){
            case 'm':
                if(strcmp(optarg, "writer") == 0) {
                    priority = PRIORITY_WRITER;
                }
                else if(strcmp(optarg, "reader") == 0) {
                    priority = PRIORITY_READER;
                }
                else{
                    fprintf(stderr, "ERROR: priority format not recognised, priority=writer\n");
                    fprintf(stderr, "usage: server --port PORT --priority writer/reader [--ratio RATIO]\n");
                    priority = PRIORITY_WRITER;
                }
                break;
            case 'r':
                ratio = strtol(optarg, NULL, 10);
                break;
            case 'p':
                port = strtol(optarg, NULL, 10);
                break;
        }
    }
    
    if(priority==-1){
        fprintf(stderr, "ERROR: --priority is required, priority=writer\n");
        fprintf(stderr, "usage: server --port PORT --priority writer/reader [--ratio RATIO]\n");
        priority = PRIORITY_WRITER;
    }
    if(port==-1){
        fprintf(stderr, "ERROR: --port is required, port=8080\n");
        fprintf(stderr, "usage: server --port PORT --priority writer/reader [--ratio RATIO]\n");
        port=8080;
    }

    printf("--priority %d --ratio %d --port %d\n", priority, ratio, port);

    return 0;
}