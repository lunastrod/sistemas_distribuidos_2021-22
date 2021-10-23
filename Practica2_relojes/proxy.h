enum operations {
    READY_TO_SHUTDOWN = 0,
    SHUTDOWN_NOW,
    SHUTDOWN_ACK
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

// Establece el nombre del proceso (para los logs y trazas)
void set_name (char name[2]);
// Establecer ip y puerto
void set_ip_port (char* ip, unsigned int port);
// Obtiene el valor del reloj de lamport.
// Utilízalo cada vez que necesites consultar el tiempo.
int get_clock_lamport();
// Notifica que está listo para realizar el apagado (READY_TO_SHUTDOWN)
void notify_ready_shutdown();
// Notifica que va a realizar el shutdown correctamente (SHUTDOWN_ACK)
void notify_shutdown_ack();



//el servidor responde que el cliente puede apagarse (SHUTDOWN_NOW)
void reply_shutdown_now();
//setup del servidor
void setup_server();
//setup del cliente
void connect_to_server();