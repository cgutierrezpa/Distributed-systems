#define MAX_USERNAME 256
#define TRUE 	1
#define FALSE 	0

/*
struct pend_msg{
    char pend_msg[MAX_MSG];
    struct pend_msg *next;
} *msg_head;*/

struct user{
    char username[MAX_USERNAME];    /* Username that acts as ID */
    char status;                    /* Status of the client: 0 if "OFF"; 1 if "ON" */
    uint16_t port;
    struct msg *pend_msgs_head;
    struct user *next;
} *user_head;

int first_user;


char isRegistered(char * username);
char registerUser(char * username);
char unregisterUser(char * username);
char connectUser(char * username, char * ip, uint16_t port);
char disconnectUser(char * username, char * used_ip);
void appendMsg(char * username, char* msg);
unsigned int updateLastID();
void printPendMsgs(char * username);
void printUsers();
void removePendMsg(char * username);