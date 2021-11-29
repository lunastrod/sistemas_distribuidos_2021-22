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
    PNAME_SIZE=20,
    IP_SIZE=16
};


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

//VARIABLES PARA TRAZAS Y LOGS:
char my_name[3];
char* my_ip[16];
unsigned int my_port;

// Establece el nombre del proceso (para los logs y trazas)
void set_name (char name[2]);
// Establecer ip y puerto (para los logs y trazas)
// prefiero usar mis funciones setup_client y setup_server para hacer las conexiones
void set_ip_port (char* ip, unsigned int port);


// Obtiene el valor del reloj de lamport.
// Utilizalo cada vez que necesites consultar el tiempo.
int get_clock_lamport();

/*
    Se que no deberia cambiar estas 2 funciones, pero prefiero pasar el connfd/sockfd
    desde mi main en vez de tener varias variables globales, por eso pongo un argumento
    mas, que es la connfd. Si esto fuera C++ crearia una clase con estas funciones.

    saco los sockfd y connfd de las funciones setup_client, setup_server y accept_new_client
*/

// Notifica que está listo para realizar el apagado (READY_TO_SHUTDOWN)
void notify_ready_shutdown(int connfd);
// Notifica que va a realizar el shutdown correctamente (SHUTDOWN_ACK)
void notify_shutdown_ack(int connfd);
//el servidor responde que el cliente puede apagarse (SHUTDOWN_NOW)
void send_shutdown_now();

//connects client to server returns: int sockfd
int setup_client(char* ip, int port);
//setup server listen returns: int sockfd
int setup_server(int port);
//returns: connfd
int accept_new_client(int sockfd);
//encapsulate a close() call mainly to debug.
void close_client(int sockfd);
//encapsulate a close() call mainly to debug.
void close_server(int sockfd);



void print_event(char*p_name, int lamport ,int8_t is_recv, enum operations action);