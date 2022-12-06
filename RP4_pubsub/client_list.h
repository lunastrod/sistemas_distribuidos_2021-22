#define TOPIC_NAME_SIZE 100
#define TOPICS_MAX 10
#define PUBLISHERS_MAX 100
#define SUBSCRIBERS_MAX 900

struct client{
    int id;
    int socket;
};

struct topic{
    char name[TOPIC_NAME_SIZE];
    struct client subs[SUBSCRIBERS_MAX];
    struct client pubs[PUBLISHERS_MAX];
};

struct client_list{
    struct topic topics[TOPICS_MAX];
    int pub_counter;
    int sub_counter;
    int id_counter;
    int readers;
    pthread_mutex_t client_list_mutex;
    pthread_mutex_t readers_mutex;
};

enum client_type{
    PUBLISHER = 0,
    SUBSCRIBER
};

void init_client_list(struct client_list *cl);

//thread safe:
int add_client(struct client_list *cl, enum client_type ct, char *topic, int socket);
void remove_client(struct client_list *cl, enum client_type ct, char *topic, int id);
int get_subscribers(struct client_list *cl, char *topic, int *connfds);//connfds is an array of SUBSCRIBERS_MAX sockets, returns the number of subscribers
//int get_all_publishers(struct client_list *cl, int *connfds);//connfd is an array of PUBLISHERS_MAX sockets, returns the number of publishers
int get_all_subscribers(struct client_list *cl, int *connfds);//connfd is an array of SUBSCRIBERS_MAX sockets, returns the number of subscribers

//private:
void print_client_list(struct client_list *cl);
int n_clients(struct client *clients, int size);
int get_new_id(struct client_list *cl, enum client_type ct);