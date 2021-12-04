#include "proxy.h"

int main(){
    pub_init("127.0.0.1",8080);
    int id=pub_register("hola");
    printf("id=%d\n", id);
    pub_unregister("hola", id);
}

