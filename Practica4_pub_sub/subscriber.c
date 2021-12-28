#include "proxy.h"
#include <getopt.h>

#include <signal.h>//SIGINT


struct args{
    char ip[IP_SIZE];
    int port;
    char topic[TOPIC_NAME_SIZE];
};

//./subscriber --ip $BROKER_IP --port $BROKER_PORT --topic $TOPIC

void manage_args(int argc, char ** argv, struct args *data){
    int current_option, option_index;

    data->port=-1;
    data->ip[0]=0;
    data->topic[0]=0;

    static struct option long_options[] = {
        {"ip ", required_argument, 0, 'i'},
        {"port ", required_argument, 0, 'p'},
        {"topic ", required_argument, 0, 't'},
        {0, 0, 0, 0}
    };


    while ((current_option =  getopt_long (argc, argv, "m:t:i:p::",long_options, &option_index)) != -1){
        switch (current_option){
            case 'i':
                strncpy(data->ip,optarg,IP_SIZE-1);
                data->ip[IP_SIZE-1]='\0';
                break;
            case 't':
                strncpy(data->topic,optarg,TOPIC_NAME_SIZE-1);
                data->topic[TOPIC_NAME_SIZE-1]='\0';
                break;
            case 'p':
                data->port = strtol(optarg, NULL, 10);
                break;
        }
    }

    if(data->port ==-1){
        warnx("ERROR: --port is required, port=8080");
        warnx("usage: subscriber --ip $BROKER_IP --port $BROKER_PORT --topic $TOPIC");
        data->port=8080;
    }
    if(data->ip[0]==0){
        warnx("ERROR: --ip is required, ip=127.0.0.1");
        warnx("usage: subscriber --ip $BROKER_IP --port $BROKER_PORT --topic $TOPIC");
        strcpy(data->ip,"127.0.0.1");
    }
    if(data->topic[0]==0){
        warnx("ERROR: --topic is required, topic=default");
        warnx("usage: subscriber --ip $BROKER_IP --port $BROKER_PORT --topic $TOPIC");
        strcpy(data->topic,"default");
    }

    //printf("--ip %s --port %d topic %s\n",data->ip,data->port,data->topic);
}

void timespec_diff(struct timespec *start, struct timespec *stop, struct timespec *result){
    if ((stop->tv_nsec - start->tv_nsec) < 0) {
        result->tv_sec = stop->tv_sec - start->tv_sec - 1;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000;
    } else {
        result->tv_sec = stop->tv_sec - start->tv_sec;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec;
    }
    return;
}

static volatile int publishing=1;
void int_handler(int sig) {
    publishing = 0;
}

int main(int argc, char *argv[]){
    struct args args_data;
    manage_args(argc,argv,&args_data);
    //printf("--ip %s --port %d topic %s\n",args_data.ip,args_data.port,args_data.topic);

    sub_init(args_data.ip,args_data.port);
    new_log("Subscriber conectado con broker ");
    printf("(%s:%d)\n",args_data.ip,args_data.port);

    int id=sub_register(args_data.topic);
    if(id<0){
        new_log("Error al hacer el registro: ");
        printf("%d\n",id);
    }
    else{
        new_log("Registrado correctamente");
        printf(" con ID: %d para topic %s\n",id,args_data.topic);
    }

    struct pollfd fd;
    fd.fd = my_sockfd;
	fd.events = POLLIN;

    while(publishing){
        signal(SIGINT, int_handler);
        poll(&fd, 1, 1000);
        if (fd.revents & POLLIN){
            struct message msg;
            recv(my_sockfd,&msg,sizeof(msg),0);

            new_log("Recibido mensaje ");
            struct timespec now;
            clock_gettime(CLOCK_REALTIME, &now);
            struct timespec diff;
            timespec_diff(&msg.data.time_generated_data,&now,&diff);
            printf("topic: %s - mensaje: %s - Generó: [%ld.%ld] - Recibido: [%ld.%ld] - Latencia: [%ld.%ld].\n", msg.topic,msg.data.data,now.tv_sec,now.tv_nsec, msg.data.time_generated_data.tv_sec,msg.data.time_generated_data.tv_nsec, diff.tv_sec, diff.tv_nsec);
        }



    }


    sub_close(args_data.topic,id);
    new_log("De-Registrado ");
    printf("(%d) correctamente del broker.\n", id);

}