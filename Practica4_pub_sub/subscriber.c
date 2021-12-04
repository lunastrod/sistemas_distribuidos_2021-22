#include "proxy.h"

int main(){
    sub_init("127.0.0.1",8080);
    int id=sub_register("hola");
    printf("id=%d\n", id);
    sub_unregister("hola", id);
    //sub_close();
    sleep(1);
}