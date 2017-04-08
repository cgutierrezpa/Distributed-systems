#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "msg_list.h"

 
/* Returns 0 if the message is stored- -1 if malloc error */
int enqueueMsg(struct msg **head, char * message, unsigned int id, char * sender){
    struct msg *temp;
    //Allocate the space for the new message and 
    temp = (struct msg *) malloc(sizeof(struct msg));
    if(temp == NULL) return -1;
    strcpy(temp->body, message);
    strcpy(temp->sender, sender);
    temp->id = id;
    temp->next = NULL;

    if (*head == NULL){      //If list is empty
        printf("Antes de insertar, la lista esta vacia!\n");
        temp->next = *head;
        *head = temp;
    }
    else{
        struct msg *last = *head;
        printf("Llegando al final de la lista de mensajes...\n");
        while(last->next != NULL){
            last = last->next;
        }
        last->next = temp;
        printf("Hemos llegado al final de la lista!\n");
    }
    return 0;
}
/* Deletes the message at the head of the queue and 
returns the new head of the list */
struct msg * dequeueMsg(struct msg **head){
    struct msg* temp = *head;
    //Head pointing to the next element
    *head = temp->next;
    //Free the resources of the first message
    
    free(temp);
    if(*head == NULL) printf("Hemos vaciado la lista!\n");

    return *head;
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

/* Deletes the message at the head of the queue */
void deleteAllMsgs(struct msg ** head){
    struct msg *temp = *head;
    while(temp != NULL){
        //Head pointing to the next element
        *head = temp->next;
        //Free the resources of the first message
        free(temp);
        temp = *head;
    }
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

