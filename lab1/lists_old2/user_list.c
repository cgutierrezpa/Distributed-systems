#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "user_list.h"


 
 int isRegistered(char * username){
    struct user *temp;
    temp = user_head;

    /* Iterate over the list */
    while (temp != NULL){
        if (strcmp(temp->username, username) == 0){     //Check if the username exists
            return 1;
        }
        temp = temp->next;
    }

    return 0;
 }
 
/* Returns 1 if already registered; 0 if registered correctly */
int registerUser(char * username){
    /* Check if the user already exists */
    if(isRegistered(username)) return 1;

    struct user *temp;
    temp = (struct user *) malloc(sizeof(struct user));
    strcpy(temp->username, username);

    //Allocate space for pending messages queue
    //struct pend_msg *msg_node = (struct pend_msg *) malloc(sizeof(struct pend_msg));
    //struct pend_msg *msg_node = NULL;
    //temp->pend_msg_head = msg_node;
    temp->last_msg_id = 0;  //Initialize the last message ID to 0
    temp->pend_msg_head = (struct pend_msg *) malloc(sizeof(struct pend_msg));
    //temp->pend_msg_head = NULL;
    if (user_head == NULL){      //If list is empty
        user_head = temp;
        user_head->next = NULL;
    }
    else{
        struct user *last = user_head;
        while(last->next != NULL){
            last = last->next;
        }
        last->next = temp;
        temp->next = NULL;
    }

    return 0;
}

/* Return 0 if disconnect OK; 1 if user is not registered;
    If the statusFlag is 0, it tries to disconnect the user but if already disconnected returns 2
    If the statusFlag is 1, it tries to connect the user but if already connected returns 1*/
int updateUserStatus(char * username, char statusFlag){
    struct user *temp = user_head;

    /* Iterate over the list */
    while(temp != NULL){
        if(strcmp(temp->username, username) == 0){  //User found
            if (temp->status == statusFlag) return 2;        
            temp->status = statusFlag;
            return 0;
        }
    }

    /* If the code reaches this point, the user was not found registered in the system */
    return 1;
}

/* Return 0 if disconnect OK; 1 if user is not registered; 2 if registered but not connected */
int connectUser(char * username, uint16_t port){
    struct user *temp = user_head;

    /* Iterate over the list */
    while(temp != NULL){
        if(strcmp(temp->username, username) == 0){  //User found
            if (temp->status == 1) return 2;        //User already connected       
            /* Change status to 1 ("ON") and update IP and Port */
            temp->status = 1;
            //Set IP
            temp->port = port;
            return 0;
        }
    }

    return 1;
}

/* Return 0 if disconnect OK; 1 if user is not registered; 2 if registered but not connected */
int disconnectUser(char * username){
    struct user *temp = user_head;

    /* Iterate over the list */
    while(temp != NULL){
        if(strcmp(temp->username, username) == 0){  //User found
            if (temp->status == 0) return 2;        //User already disconnected       
            /* Change status to 0 ("OFF") and delete IP and Port */
            temp->status = 0;
            //Free IP
            free(&(temp->port));
            return 0;
        }
    }

    return 1;
}
/*
 
 
void insert(int num){
    int c = 0;
    struct user *temp;
    temp = user_head;
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
 
 
 */
/* Returns 1 if the user does not exist. 0 if the user is deleted correctly */
int unregisterUser(char * username){
    /* Check if the user is not registered */
    //if(!isRegistered(username)) return 1;

    struct user *temp, *prev; //temp is the current user, prev is the previous user in the list
    temp = user_head;

    /* Iterate over the list */
    while(temp != NULL){
        if(strcmp(temp->username, username) == 0){  //User found
            if(temp == user_head){       //If the user is at the user_head of the list
                user_head = temp->next;  //change the user_head to the next element
                free(temp);         //Free the resources of the user
                return 0;
            }
            else{                   //User is not at the user_head
            prev->next = temp->next;
            free(temp);
            return 0;
            }
        }
        else{
            prev = temp;
            temp = temp->next;
        }
    }
    //If we reach this point, no user was found
    return 1;
}
 

void printUsers(){
    struct user *temp;
    temp = user_head;
    if(temp == NULL){
        return;
    }
    while(temp != NULL){
        printf("%s ", temp->username);
        temp = temp->next;
        printf("\n");
    }
}

void appendMsg(char * username, char* msg){
    struct user *temp = user_head;

    /* Iterate over the list */
    while(temp != NULL){
        if(strcmp(temp->username, username) == 0){  //User found
            int id = updateID(temp);
            //Enqueue message
            struct pend_msg *temp_msg = temp->pend_msg_head;
            enqueueMsg(&temp, msg, 3);
            return;     
        }
        temp = temp->next;
    }

    return;
}

unsigned int updateID(struct user *usr){
    //Increment the ID in 1
    usr->last_msg_id = usr->last_msg_id+1;
    //If it results in 0, then the maximum representable number is overflown
    if(usr->last_msg_id == 0) usr->last_msg_id = 1;
    printf("LAST PENDING MESSAGE ID IS: %d", usr->last_msg_id);

    return usr->last_msg_id;
}

void printPendMsgs(char * username){
    struct user *temp = user_head;

    /* Iterate over the list */
    while(temp != NULL){
        if(strcmp(temp->username, username) == 0){  //User found
            printMsgs(&(temp->pend_msg_head));
            return;     
        }
        temp = temp->next;
    }
}
 
 /*
int count(){
    struct user *n;
    int c = 0;
    n = user_head;

    while(n != NULL){
        n = n->next;
        c++;
    }

    return c;
}*/



///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
/* Returns 1 if already registered; 0 if registered correctly */
int enqueueMsg(struct user **msg_head, char * msg, unsigned int id){
    //struct pend_msg *msg_head = *(usr->pend_msg_head);
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

    }
    else{
        printf("MSG_HEAD IS NOT NULL!\n");
        temp->next = msg_head;
        msg_head = temp;
        printf("MESSAGE ENQUEUED\n");

    }

    printf("RESULT OF ENQUEUEING:\n");
    struct pend_msg *aux = usr->pend_msg_head;
    if(aux == NULL){
        printf("LA LISTA NO CONTIENE NADA DESPUES DEL ENQUEUE!!!!!!\n");
    }
    while(aux != NULL){
        printf("Message ID: %d", aux->id);
        printf("Message body: %s", aux->pend_msg);
        aux = aux->next;
    }
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

void printMsgs(struct pend_msg ** msg_head){
    printf("INSIDE THE PRINT MSGS FUNCTION!\n");
    struct pend_msg *temp = msg_head;
    if(temp == NULL){
        printf("NOTHING TO PRINT!");
        return;
    }

    while(temp != NULL){
        printf("Message ID: %d", temp->id);
        printf("Message body: %s", temp->pend_msg);
        temp = temp->next;
    }

    return;
}