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

enum{
    MAX_COUNTER_LENGTH=11,
    MAX_THREADS=2
}; 

unsigned int counter=0;
FILE* server_output;

sem_t max_threads_sem;



void * client_func(void *ptr){
    accept_new_client();

    enum operations req = recv_request();
    fprintf(stderr,"debug req: %d\n", req);
    if(req==WRITE){
        fprintf(stderr,"debug write\n");
        counter++;
        fprintf(server_output,"%u ",counter);
        fflush(server_output);
        send_response(req,counter,50);
    }
    if(req==READ){
        send_response(req,counter,50);
    }
    
    sem_post(&max_threads_sem);
    return NULL;
}

void attend_clients(){
    pthread_t ignore=0;//ignoro el id del thread porque nunca voy a salir del bucle

    sem_init(&max_threads_sem,0,MAX_THREADS);
    
    while(1){
        sem_wait(&max_threads_sem);
        int sval;
        sem_getvalue(&max_threads_sem, &sval);
        fprintf(stderr,"Semaphore value: %d\n", sval);
        pthread_create(&ignore, NULL, client_func, NULL);
    }
}

//input must be null terminated
int last_int_from_string(char * input){
    //Scan the line for tokens until you get to the end and then convert the last token into a number
    int result=0;
    input=strtok(input," ");
    while(input != NULL){
        result=strtol(input, NULL, 10);
        input=strtok(NULL," ");
        //printf("%s:%d",input,result);
        fflush(stdout);
    }
    return result;
}


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

    setup_server("127.0.0.1",port);

    //Open for both reading and appending. If the file does not exist, it will be created.
    server_output = fopen("server_output.txt", "a+");

    //Read the last int from the file and asign its value to counter
    fseek(server_output,-MAX_COUNTER_LENGTH,SEEK_END);
    char a[MAX_COUNTER_LENGTH+2];
    fgets(a,MAX_COUNTER_LENGTH+2,server_output);
    counter=last_int_from_string(a);

    //now the server is ready to recv msgs

    

    attend_clients();
    //close and exit
    close_server();
    fclose(server_output);
    return 0;
}