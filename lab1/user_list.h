#define MAX_USERNAME 256
#define TRUE 	1
#define FALSE 	0

struct user{
    char username[MAX_USERNAME];    /* Username that acts as ID */
    char status;                    /* Status of the client: 0 if "OFF"; 1 if "ON" */
    char ip[16];	
    uint16_t port;
    struct msg *pend_msgs_head;
    struct user *next;
    unsigned int last_id;
} *user_head;

char isRegistered(char * username);
char registerUser(char * username);
char unregisterUser(char * username);
char connectUser(char * username, char * ip, uint16_t port);
char disconnectUser(char * username, char * used_ip);
int storeMsg(char * username, char* msg, unsigned int msg_id, char * sender);
unsigned int updateLastID(char * username);
void printPendMsgs(char * username);
void printUsers();
void removePendMsg(char * username);
char isConnected(char * username);
char * getUserIP(char * username);
uint16_t getUserPort(char * username);
struct msg ** getPendMsgHead(char * username);