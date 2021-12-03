#include "proxy.h"

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

int main(){
    //brok_init(8080);
    //brok_recv(accept_new_client(my_sockfd));
    topic_list_init();
    topic_list_print();
    printf("\n");
    
    topic_list_new_topic("hola");
    
    topic_list_new_pub("hola", 80);
    topic_list_new_pub("hola", 80);
    topic_list_new_sub("hola", 80);
    topic_list_print();
    printf("\n");

    topic_list_remove_topic("hola");


    topic_list_print();
    printf("\n");
    
    topic_list_delete();
}