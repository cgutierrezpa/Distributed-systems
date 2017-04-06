#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "msg_list.h"

 
/* Returns 1 if already registered; 0 if registered correctly */
void enqueueMsg(struct msg **head, char * message, unsigned int id, char * sender){
    struct msg *temp;
    //Allocate the space for the new message and 
    temp = (struct msg *) malloc(sizeof(struct msg));
    strcpy(temp->body, message);
    strcpy(temp->sender, sender);
    temp->id = id;
    temp->next = NULL;

    if (*head == NULL){      //If list is empty
        temp->next = *head;
        *head = temp;
    }
    else{
        struct msg *last = *head;
        
        while(last->next != NULL){
            last = last->next;
        }
        last->next = temp;
    }
}

/* Deletes the message at the head of the queue */
void removeMsg(struct msg ** head){
    struct msg *temp = *head;
    //Head pointing to the next element
    *head = temp->next;
    //Free the resources of the first message
    free(temp);
    return;
}

void printMsgs(struct msg ** head){
    printf("INSIDE THE PRINT MSGS FUNCTION!\n");
    struct msg *temp = *head;
    if(temp == NULL){
        return;
    }

    while(temp != NULL){
        printf("Message ID: %u", temp->id);
        printf("Message body: %s", temp->body);
        printf("\n");
        temp = temp->next;
    }

    return;
}
