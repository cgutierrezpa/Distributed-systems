#include "user_list.h"
#define MAX_MSG 256

/*
struct pend_msg{
    char pend_msg[MAX_MSG];
    unsigned int id;
    struct pend_msg *next;
} *msg_head;*/
struct pend_msg{
    char pend_msg[MAX_MSG];
    unsigned int id;
    struct pend_msg *next;
};


int enqueueMsg(struct user *usr, char * msg, unsigned int id);
void removeMsg();
void printMsgs(struct pend_msg * msg_head);
