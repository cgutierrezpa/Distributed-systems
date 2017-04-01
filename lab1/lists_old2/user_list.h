#define MAX_USERNAME 256
#define MAX_MSG 256

/*
struct pend_msg{
    char pend_msg[MAX_MSG];
    struct pend_msg *next;
} *msg_head;*/


struct user{
    char username[MAX_USERNAME];    /* Username that acts as ID */
    char status;                    /* Status of the client: 0 if "OFF"; 1 if "ON" */
    uint16_t port;
    struct pend_msg{
    	char pend_msg[MAX_MSG];
    	unsigned int id;
    	struct pend_msg *next;
	} *pend_msg_head;
    unsigned int last_msg_id;
    struct user *next;
} *user_head;

int isRegistered(char * username);
int registerUser(char * username);
int unregisterUser(char * username);
int connectUser(char * username, uint16_t port);
int disconnectUser(char * username);
void appendMsg(char * username, char* msg);
unsigned int updateID(struct user *usr);
void printPendMsgs(char * username);
void printUsers();
int enqueueMsg(struct user **usr, char * msg, unsigned int id);
void removeMsg();
void printMsgs(struct pend_msg ** msg_head);