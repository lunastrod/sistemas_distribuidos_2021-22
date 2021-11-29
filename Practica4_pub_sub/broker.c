
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

#include <sys/queue.h>

int main(){
    LIST_HEAD(subshead, entry) head;
    struct subshead *headp;

    struct entry {
    LIST_ENTRY(entry) entries;          //List.
    } *n1, *n2, *np;



    LIST_INIT(&head);                       //Initialize the list.

    n1 = malloc(sizeof(struct entry));      //Insert at the head.
    LIST_INSERT_HEAD(&head, n1, entries);

    for (np = head.lh_first; np != NULL; np = np->entries.le_next){
        np->
    }

    while (head.lh_first != NULL){           //Delete.
        LIST_REMOVE(head.lh_first, entries);
    }


}