#include "proxy.h"
#include <getopt.h>


struct args{
    int port;
    enum broker_mode mode;
};

const char * mode_sequential_str="secuencial";
const char * mode_parallel_str="paralelo";
const char * mode_fair_str="justo";


void manage_args(int argc, char ** argv, struct args *data){
    int current_option, option_index;

    data->mode=-1;
    data->port=-1;


    static struct option long_options[] = {
        {"port ", required_argument, 0, 'p'},
        {"mode ", required_argument, 0, 'm'},
        {0, 0, 0, 0}
    };


    while ((current_option =  getopt_long (argc, argv, "m:t:i:p::",long_options, &option_index)) != -1){
        switch (current_option){
            case 'm':
                if(0==strcmp(mode_sequential_str,optarg)){
                    data->mode=SEQUENTIAL;
                }
                else if(0==strcmp(mode_parallel_str,optarg)){
                    data->mode=PARALLEL;
                }
                else if(0==strcmp(mode_fair_str,optarg)){
                    data->mode=FAIR;
                }
                else{
                    warnx("ERROR: invalid mode, mode=%s",mode_sequential_str);
                    warnx("valid modes: %s %s %s",mode_sequential_str,mode_parallel_str,mode_fair_str);
                    warnx("usage: broker --port $BROKER_PORT --mode $MODE");
                    data->mode=SEQUENTIAL;
                }

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
    if(data->mode ==-1){
        warnx("ERROR: invalid mode, mode=%s",mode_sequential_str);
        warnx("usage: publisher --ip $BROKER_IP --port $BROKER_PORT --topic $TOPIC");
        data->mode=SEQUENTIAL;
    }

    //printf("--port %d mode %d\n",data->port,data->mode);
}


/*
int main(){
    int port;
    for(int i=0; i<TOPICS_MAX; i++){
        topics[i].pubs=init_client_list(PUBLISHERS_MAX);
        topics[i].subs=init_client_list(SUBSCRIBERS_MAX);
    }

    for(int i=0; i<TOPICS_MAX; i++){
        delete_client_list(topics[i].pubs);
        delete_client_list(topics[i].subs);
    }
    return 0;
}
*/

/*
brok_init{
    B: topic_list_init();
    B: listen
}

P: connect connfd
B: accept connfd
P: P>B message REGISTER_PUBLISHER topic
B: recv
B: id=topic_list_new_pub(topic, connfd)
B: B>P response status=OK, id

brok_close{
    close sockfd()
    topic_list_delete()
}
*/

/*
es buena idea que broker solo tenga 1 hilo para control de database y recv? creo que si
ademas que el hilo del broker haga un join para que al salir de las funciones secuencial multithread sincronizado
solo quede 1 hilo y no pasen cosas raras. Creo que así además no necesito mutex raros.

broker pseudocode:

continuamente escucha mensajes
2 hilos: uno atiende clientes nuevos y otro atiende a los clientes ya registrados
las funciones de la base de datos necesitan mutex
    si: nuevo mensaje: lo mete en funcion con varios if. brok_recv()
        si: register/unregister: actualiza database. brok_new_register()
            creo nuevo hilo para esto? creo que es una operacion rapida asi que no hace falta
            pero recv funciona si no esta dentro de recv cuando hago el send? deberia probarlo creo que si
        si: publish data: envia data a subs
            secuencial:
            multithread:
            sincronizado:


./broker --port $BROKER_PORT --mode $MODE
*/

void * new_clients_thread_func(void * arg){
    while(1){
        
        int connfd=accept_new_client(my_sockfd);
        //printf("connfd=%d\n",connfd);
        //TDEB("brok new client");
        brok_recv(connfd);
        //TDEB("brok new client ok");
    }
    return NULL;
}


int main(int argc, char *argv[]){
    struct args args_data;
    manage_args(argc,argv, &args_data);
    //printf("--port %d mode %d\n",args_data.port,args_data.mode);

    //TDEB("brok init");
    brok_init(args_data.port);
    //TDEB("brok init ok");

    pthread_t new_clients_thread;
    pthread_create(&new_clients_thread,NULL,new_clients_thread_func,NULL);

    //printf("hilo creado\n");
    while(1){
        brok_seq_recv(args_data.mode);
    }

    pthread_join(new_clients_thread,NULL);


    /*
    int c1=accept_new_client(my_sockfd);
    brok_recv(c1);
    int c2=accept_new_client(my_sockfd);
    brok_recv(c2);

    while(1){
        brok_recv(c1);
    }


    sleep(2);
    */

    /*
    brok_init(8080);
    while(1){
        topic_list_print();
        printf("\n\n");
        int c=accept_new_client(my_sockfd);

        brok_recv(c);//register
        topic_list_print();
        printf("\n\n");

        brok_recv(c);//unregister
        //sleep(1);
    }
    brok_close();
    */
}