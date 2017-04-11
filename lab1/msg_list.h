#define MAX_MSG 256

struct msg{
    char body[MAX_MSG];		/* Content of the message */
    char sender[MAX_MSG];	/* Sender of the message */
    unsigned int id;		/* ID assigned to the message */
    struct msg *next;		/* Pointer to the next message in the list */
};

/* ================FUNCTION HEADERS================ */
int enqueueMsg(struct msg **head, char * message, unsigned int id, char * sender);
struct msg * dequeueMsg(struct msg **head);
void deleteAllMsgs(struct msg ** head);