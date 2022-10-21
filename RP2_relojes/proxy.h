//std
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <string.h>

//sockets
#include <arpa/inet.h>

/*
MACRO PARA PONER TRAZAS
usage: TDEB("ip:%s port%d",ip,port);
asi son mas faciles de quitar para hacer la entrega
*/
#define TDEB(...) fprintf(stderr,"DEBUG: "__VA_ARGS__); fprintf(stderr,"\n")

enum operations {
    READY_TO_SHUTDOWN = 0,
    SHUTDOWN_NOW,
    SHUTDOWN_ACK
};
enum{
    NAME_SIZE=20,
    IP_SIZE=16,
    N_CLIENTS=2
};

#define SERVER_NAME "p2"
#define CLIENT0_NAME "p1"
#define CLIENT1_NAME "p2"

const char client_names[NAME_SIZE][N_CLIENTS]={CLIENT0_NAME,CLIENT1_NAME};




/*
El campo origin contendrá el nombre del proceso que envía el mensaje.
El campo action podrá contener valores del enumerado operations:
· READY_TO_SHUTDOWN: la máquina notifica que está lista para apagarse.
· SHUTDOWN_NOW: Al recibir este mensaje la máquina sabe que debe apagarse.
· SHUTDOWN_ACK: La máquina manda este mensaje antes justo de realizar el shutdown.
El campo clock_lamport es utilizado para enviar el contador de lamport.
*/
struct message {
    char origin[20];
    enum operations action;
    unsigned int clock_lamport;
};


extern char my_name[NAME_SIZE];
extern char my_ip[16];
extern unsigned int my_port;
extern int my_sockfd;
extern int client_connfds[N_CLIENTS];//para guardar connfd de p1 y p3 en ese orden

// Establece el nombre del proceso (para los logs y trazas)
void set_name (char name[2]);
// Establecer ip y puerto
void set_ip_port (char* ip, unsigned int port);
// Obtiene el valor del reloj de lamport.
// Utilizalo cada vez que necesites consultar el tiempo.
int get_clock_lamport();
// Notifica que está listo para realizar el apagado (READY_TO_SHUTDOWN)
void notify_ready_shutdown();
// Notifica que va a realizar el shutdown correctamente (SHUTDOWN_ACK)
void notify_shutdown_ack();


//el servidor responde que el cliente puede apagarse (SHUTDOWN_NOW)
void send_shutdown_now();

//connects client to server returns: int sockfd
int setup_client(char* ip, int port);
//setup server listen returns: int sockfd
int setup_server(int port);
//returns: connfd
int accept_new_client(int sockfd);
void close_client();
void close_server();



void print_event(char*p_name, int lamport ,int8_t is_recv, enum operations action);