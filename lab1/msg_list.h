#define MAX_MSG 256

struct msg{
    char body[MAX_MSG];
    char sender[256];
    unsigned int id;
    struct msg *next;
};

unsigned int last_msg_id;

void enqueueMsg(struct msg **head, char * message, unsigned int id, char * sender);
void removeMsg(struct msg ** head);
void printMsgs(struct msg ** head);
