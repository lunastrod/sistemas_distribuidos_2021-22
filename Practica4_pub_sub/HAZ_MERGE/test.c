
/*
LIST_ENTRY(TYPE);
LIST_HEAD(HEADNAME, TYPE);
LIST_INIT(LIST_HEAD *head);
LIST_INSERT_AFTER(LIST_ENTRY *listelm,
                TYPE *elm, LIST_ENTRY NAME);
LIST_INSERT_HEAD(LIST_HEAD *head,
                TYPE *elm, LIST_ENTRY NAME);
LIST_REMOVE(TYPE *elm, LIST_ENTRY NAME);

*/
/*
LIST_ENTRY(TYPE);

LIST_HEAD(listhead, entry) head;
struct listhead *headp;                 //List head.
struct entry {
    ...
    LIST_ENTRY(entry) entries;          //List.
    ...
} *n1, *n2, *np;

LIST_INIT(&head);                       //Initialize the list.

n1 = malloc(sizeof(struct entry));      //Insert at the head.
LIST_INSERT_HEAD(&head, n1, entries);

n2 = malloc(sizeof(struct entry));      //Insert after.
LIST_INSERT_AFTER(n1, n2, entries);
                                        //Forward traversal.
for (np = head.lh_first; np != NULL; np = np->entries.le_next)
    np-> ...

while (head.lh_first != NULL)           //Delete.
    LIST_REMOVE(head.lh_first, entries);
*/
/*
enum{
    MAX_SUBSCRIBERS=1000,
    MAX_PUBLISHERS=100,
    MAX_TOPICS=10,
    MAX_TOPIC_NAME_SIZE=100
};
*/
/*
struct publisher{
    int connfd;
};

struct subscriber{
    int connfd;
};

struct queue_pubs{
    int npubs;
    struct publisher pubs[MAX_PUBLISHERS];
};

void insert_queue();

struct queue_subs{
    int nsubs;
    struct subscriber subs[MAX_SUBSCRIBERS];
};

struct topic{
    char name[MAX_TOPIC_NAME_SIZE];
    struct queue_pubs;
    struct queue_subs;
};
*/
/*
struct topic topics[MAX_TOPICS];



//Cada vez que reciba un mensaje de un publicador
void attend_clients_seq(struct &topic,){
    //mandara secuencialmente ese mensaje a los suscriptores registrados a ese topic

}
*/
/*
//std
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>

//sockets
#include <arpa/inet.h>
//queue
#include <sys/queue.h>

#include <pthread.h>
#include <string.h>

int main(){
    LIST_HEAD(subshead, node) head;
    struct subshead *headp;

    struct node {
        LIST_ENTRY(node) nodes;          //List.
        int data;
    };

    LIST_INIT(&head);
    struct node * n1 = malloc(sizeof(struct node));
    n1->data=13;
    LIST_INSERT_HEAD(&head, n1, nodes);

    struct node * n2 = malloc(sizeof(struct node));
    n2->data=57;
    LIST_INSERT_AFTER(n1, n2, nodes);
    
    struct node * n3 = malloc(sizeof(struct node));
    n3->data=127;
    LIST_INSERT_AFTER(n2, n3, nodes);

    LIST_REMOVE(n2, nodes);
    free(n2);
    
    for (struct node * np = head.lh_first; np != NULL; np = np->nodes.le_next){
        printf("%d\n",np->data);
    }

    while (head.lh_first != NULL){
        LIST_REMOVE(head.lh_first, nodes);
        free(head.lh_first);
    }
    free(head);
}

*/

#include <stdio.h>
#include <string.h>

int numbers[100];
int ocup;

void print_list(){
    for(int i=0; i<100;i++){
        if(i==ocup){
            printf("| ");
        }
        printf("%d ",numbers[i]);
    }
    printf("\n");
}

void remove_number(int index){
    for(int i=index; i<ocup-1; i++){
        memcpy(&numbers[i],&numbers[i+1],sizeof(numbers[i+1]));
        
    }
    ocup--;
}


int main(){
    ocup=0;
    numbers[ocup]=16;
    ocup++;
    numbers[ocup]=32;
    ocup++;
    numbers[ocup]=64;
    ocup++;
    numbers[ocup]=128;
    ocup++;

    print_list();
    remove_number(1);
    print_list();
    remove_number(1);
    print_list();
    remove_number(1);
    print_list();
    remove_number(0);
    print_list();
}