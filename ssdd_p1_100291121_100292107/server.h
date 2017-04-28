#define MAX_COMMAND 	11

/* Mutex & Threads */
pthread_mutex_t socket_mtx;
pthread_mutex_t list_mtx;
pthread_t thread;
pthread_attr_t thread_att;
/* Controls the access to the socket generated by the connection 'accept' */
pthread_cond_t free_socket;
pthread_cond_t free_list;

/* Variable that controls the state of the socket created by the connection 'accept' */
int busy_socket;

/* Declare the server socket as global variable */
int s_server;

/* Declare the list of users */
struct user *users;

/* ================FUNCTION HEADERS================ */
void interruptHandler(int sig);
void * manageRequest(int *sd);
void toUpperCase(char * string);
int sendMessage(char * sender, char * receiver, char * msg, unsigned int msg_id, char stored);
void sendAck(char * sender, unsigned int msg_id);
int storeMessage(char * sender, char * receiver, char * msg, unsigned int mgs_id);