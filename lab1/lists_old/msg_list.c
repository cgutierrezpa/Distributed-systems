#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "msg_list.h"

 
/* Returns 1 if already registered; 0 if registered correctly */
int enqueueMsg(struct user *usr, char * msg, unsigned int id){
    struct pend_msg *msg_head = usr->pend_msg_head;
    struct pend_msg *temp;
    temp = (struct pend_msg *) malloc(sizeof(struct pend_msg));
    strcpy(temp->pend_msg, msg);
    temp->id = id;
    /*
    printf("MESSAGE TO BE QUEUED: \n");
    printf("\tBODY: %s", temp->pend_msg);
    printf("\tID: %d", temp->id);
    printf("MESSAGE HEAD POINTER IS:\n");
    printf("\tBODY: %s", msg_head->pend_msg);
    printf("\tID: %d", msg_head->id);
    if(msg_head->next == NULL){
        printf("HEAD->NEXT IS NULL!\n");  
    }
    else{
        printf("HEAD->NEXT IS NOT NULL\n");
    }*/

    if (msg_head == NULL){      //If list is empty
        printf("MSG_HEAD IS NULL!\n");
        msg_head = temp;
        msg_head->next = NULL;
        printf("MESSAGE ENQUEUED\n");
        return 1;

    }
    else{
        printf("MSG_HEAD IS NOT NULL!\n");
        temp->next = msg_head;
        msg_head = temp;
        printf("MESSAGE ENQUEUED\n");
        return 1;

    }

    printf("MESSAGE NOT ENQUEUED\n");
    return 0;
}

/* Deletes the message at the head of the queue 
void removeMsg(){
    struct pend_msg *temp = msg_head;
    //Head pointing to the next element
    msg_head = temp->next;
    //Free the resources of the first message
    free(temp);
    return;
}*/

/* Retrieves the message at the head of the queue */
/*
struct pend_msg * peekMsg(struct pend_msg * msg_head){
    struct user *temp = user_head;

    while(temp != NULL){
        if(strcmp(temp->username, username) == 0){  //User found
            return temp->msg_head;     
        }
        temp = temp->next;
    }

    return NULL;
}
*/

void printMsgs(struct pend_msg * msg_head){
    printf("INSIDE THE PRINT MSGS FUNCTION!\n");
    struct pend_msg *temp = msg_head;
    if(temp == NULL){
        return;
    }

    while(temp != NULL){
        printf("Message ID: %d", temp->id);
        printf("Message body: %s", temp->pend_msg);
        temp = temp->next;
    }

    return;
}
/*
void addafter(int num, int loc){
    int i;
    struct pend_msg *temp,*left,*right;
    right = msg_head;
    for(i=1;i<loc;i++){
        left = right;
        right = right->next;
    }
    temp = (struct pend_msg *)malloc(sizeof(struct pend_msg));
    temp->data = num;
    left->next = temp;
    left = temp;
    left->next = right;
    return;
}
 
 
 
void insert(int num){
    int c = 0;
    struct pend_msg *temp;
    temp = msg_head;
    if(temp == NULL){
        add(num);
    }
    else{
        while(temp != NULL){
            if(temp->data < num)
                c++;
                temp = temp->next;
        }
    if(c == 0)
        add(num);
    else if(c < count())
        addafter(num,++c);
    else
        append(num);
    }
}
 

void  printMsgs(){
    struct pend_msg *temp;
    temp = msg_head;
    if(temp == NULL){
        return;
    }
    while(temp != NULL){
        printf("%s ", temp->pend_msg);
        temp = temp->next;
        printf("\n");
    }
}
 
 /*
int count(){
    struct pend_msg *n;
    int c = 0;
    n = msg_head;

    while(n != NULL){
        n = n->next;
        c++;
    }

    return c;
}*/
