#include "proxy.h"
#include <getopt.h>
#include <signal.h>//SIGINT


// ./publisher --ip $BROKER_IP --port $BROKER_PORT --topic $TOPIC

//0, 10, 0.999732, 0.1302432, 0.437446679241908, 0.3144539741580212
//0, 500, 2.980898152, 0.1000113, 1.042614694735484, 0.940989385289
//1, 500, 2.8298132, 0.10000029, 1.95887623496, 0.157721239
//2, 500, 1.128376134, 0.1000321, 1.394944606673, 0.03211231
//2, 500, 1.8298132, 0.10000036, 1.394944606673, 0.023583598

struct args{
    char ip[IP_SIZE];
    int port;
    char topic[TOPIC_NAME_SIZE];
};

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
        warnx("usage: publisher --ip $BROKER_IP --port $BROKER_PORT --topic $TOPIC");
        data->port=8080;
    }
    if(data->ip[0]==0){
        warnx("ERROR: --ip is required, ip=127.0.0.1");
        warnx("usage: publisher --ip $BROKER_IP --port $BROKER_PORT --topic $TOPIC");
        strcpy(data->ip,"127.0.0.1");
    }
    if(data->topic[0]==0){
        warnx("ERROR: --topic is required, topic=default");
        warnx("usage: publisher --ip $BROKER_IP --port $BROKER_PORT --topic $TOPIC");
        strcpy(data->topic,"default");
    }

    //printf("--ip %s --port %d topic %s\n",data->ip,data->port,data->topic);
}

static volatile int publishing=1;
void int_handler(int sig) {
    publishing = 0;
}

int main(int argc, char *argv[]){
    struct args args_data;
    manage_args(argc,argv,&args_data);
    printf("--ip %s --port %d topic %s\n",args_data.ip,args_data.port,args_data.topic);

    pub_init(args_data.ip,args_data.port);
    new_log("Publisher conectado con el broker correctamente.");
    printf("(%s:%d)\n",args_data.ip,args_data.port);

    int id=pub_register(args_data.topic);
    if(id<0){
        new_log("Error al hacer el registro: ");
        printf("%d\n",id);
        errx(-1, "register error");
    }
    else{
        new_log("Registrado correctamente");
        printf(" con ID: %d para topic %s\n",id,args_data.topic);
    }
    char data[100];
    int i=0;
    while(publishing){
        snprintf(data,sizeof(data),"data %d",i);
        pub_publish_data(data,args_data.topic);
        //new_log("Publicado mensaje ");
        //printf("topic: %s - mensaje: %s - Generó: [%lu.%lu]\n", args_data.topic, data, spec.tv_sec, spec.tv_nsec);
        usleep(1000000);
        signal(SIGINT, int_handler);
        i++;
    }

    pub_close(args_data.topic,id);
    new_log("De-Registrado ");
    printf("(%d) correctamente del broker.\n", id);
}

