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

int main(){
    brok_init(8080);
    /*
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
    */


    brok_close();

    

    
}