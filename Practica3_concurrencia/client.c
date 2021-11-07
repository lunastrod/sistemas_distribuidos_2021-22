#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <pthread.h>
#include <getopt.h>

#include "proxy.h"


int main(int argc, char *argv[]){
    
    char * ip = NULL;
    enum operations mode = -1;
    int threads = -1, port = -1;

    int current_option, option_index;

    static struct option long_options[] = {
        {"mode ", required_argument, 0, 'm'},
        {"threads ", required_argument, 0, 't'},
        {"ip ", required_argument, 0, 'i' },
        {"port ", required_argument, 0, 'p'},
        {0, 0, 0, 0}
    };


    while ((current_option =  getopt_long (argc, argv, "m:t:i:p::",long_options, &option_index)) != -1){
        switch (current_option){
            case 'm':
                if(strcmp(optarg, "writer") == 0) {
                    mode = WRITE;
                }
                else if(strcmp(optarg, "reader") == 0) {
                    mode = READ;
                }
                else{
                    fprintf(stderr, "ERROR: mode format not recognised, mode=reader\n");
                    fprintf(stderr, "usage: client --mode writer/reader --threads N_THREADS --ip IP_ADDRESS --port PORT\n");
                    mode = READ;
                }
                break;
            case 't':
                threads = strtol(optarg, NULL, 10);
                break;
            case 'i':
                ip = optarg;
                break;
            case 'p':
                port = strtol(optarg, NULL, 10);
                break;
        }
    }
    
    if(mode==-1){
        fprintf(stderr, "ERROR: --mode is required, mode=reader\n");
        fprintf(stderr, "usage: client --mode writer/reader --threads N_THREADS --ip IP_ADDRESS --port PORT\n");
        mode = READ;
    }
    if(threads==-1){
        fprintf(stderr, "ERROR: --threads is required, threads=1\n");
        fprintf(stderr, "usage: client --mode writer/reader --threads N_THREADS --ip IP_ADDRESS --port PORT\n");
        threads=1;
    }
    if(ip==NULL){
        fprintf(stderr, "ERROR: --ip is required, ip=127.0.0.1\n");
        fprintf(stderr, "usage: client --mode writer/reader --threads N_THREADS --ip IP_ADDRESS --port PORT\n");
        ip="127.0.0.1";
    }
    if(port==-1){
        fprintf(stderr, "ERROR: --port is required, port=8080\n");
        fprintf(stderr, "usage: client --mode writer/reader --threads N_THREADS --ip IP_ADDRESS --port PORT\n");
        port=8080;
    }

    printf("--mode %d --threads %d --ip %s --port %d\n", mode, threads, ip, port);

    setup_client(ip, port);
    fprintf(stderr,"debug req: %d\n", mode);
    printf("counter=%d\n",send_request(mode).counter);
    close_client();

    return 0;
}