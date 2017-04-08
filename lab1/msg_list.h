#define MAX_MSG 256

struct msg{
    char body[MAX_MSG];
    char sender[256];
    unsigned int id;
    struct msg *next;
};

int enqueueMsg(struct msg **head, char * message, unsigned int id, char * sender);
void deleteAllMsgs(struct msg ** head);
void removeMsg(struct msg ** head);
void printMsgs(struct msg ** head);
struct msg * dequeueMsg(struct msg **head);

