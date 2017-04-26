#define MAX_MSG 256

struct msg{
    char body[MAX_MSG];
    unsigned int id;
    struct msg *next;
};

unsigned int last_msg_id;

void enqueueMsg(struct msg **head, char * msg, unsigned int id);
void removeMsg(struct msg ** head);
void printMsgs(struct msg ** head);
