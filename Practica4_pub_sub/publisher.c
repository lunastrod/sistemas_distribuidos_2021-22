#include "proxy.h"

int main(){
    pub_init("127.0.0.1",8080);
    pub_register("hola");
    pub_unregister("hola", 12);
}