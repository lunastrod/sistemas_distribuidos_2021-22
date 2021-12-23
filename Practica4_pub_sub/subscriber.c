#include "proxy.h"
#include <getopt.h>


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

int main(int argc, char *argv[]){
    struct args args_data;
    manage_args(argc,argv,&args_data);
    printf("--ip %s --port %d topic %s\n",args_data.ip,args_data.port,args_data.topic);

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
    while(1){
        struct message msg;
        recv(my_sockfd,&msg,sizeof(msg),0);
        debug_print_msg(msg,"sub");
        //new_log("Recibido mensaje ");
        //printf("topic: %s - mensaje: %s - Generó: $time_generated_data - Recibido: $time_received_data - Latencia:$latency.\n", "$topic", "$mensaje");
    }



    //sub_close();//probablemente nunca haga falta cerrarlo
    /*
    sub_init("127.0.0.1",8080);
    int id=sub_register("hola");
    printf("id=%d\n", id);
    sub_unregister("hola", id);
    //sub_close();
    sleep(1);
    */

}