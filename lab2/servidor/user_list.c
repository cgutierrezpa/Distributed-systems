#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "msg_list.h"
#include "user_list.h"

 
char isRegistered(char * username){
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
char registerUser(char * username){
    /* Check if the user already exists */
    if(isRegistered(username)) return 1;

    /* Prepare new user */ 
    struct user *temp;
    temp = (struct user *) malloc(sizeof(struct user));
    /* Initialize user values */
    strcpy(temp->username, username);
    temp->status = 0;
    temp->port = 0;
    temp->pend_msgs_head = NULL;
    temp->next = NULL;

    if (user_head == NULL){      //If list is empty
        /*Initialize the last assigned ID to a message if is the first user to register*/
        if(first_user){
            last_msg_id = 0;
            first_user = FALSE;
        } 
        temp->next = user_head;
        user_head = temp;
    }
    else{
        struct user *last = user_head;

        /* Iterate over the list */
        while(last->next != NULL){
            last = last->next;
        }
        last->next = temp;
    }

    return 0;
}

/* Return 0 if connect OK; 1 if user is not registered; 2 if registered but not connected */
char connectUser(char * username, char * ip, uint16_t port){
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
        temp = temp->next;
    }

    return 1;
}

/* Return 0 if disconnect OK;
     1 if user is not registered;
     2 if registered but not connected;
     3 if trying to disconnect from a different IP */
char disconnectUser(char * username, char * used_ip){
    struct user *temp = user_head;

    /* Iterate over the list */
    while(temp != NULL){
        if(strcmp(temp->username, username) == 0){  //User found
            //if(strcmp(temp->ip, used_ip) != 0) return 3; //Trying to disconnect from a different IP
            if (temp->status == 0) return 2;        //User already disconnected       
            /* Change status to 0 ("OFF") and delete IP and Port */
            temp->status = 0;
            //Free IP
            free(&(temp->port));
            return 0;
        }
        temp = temp->next;
    }

    return 1;
}
 

/* Returns 1 if the user does not exist. 0 if the user is deleted correctly */
char unregisterUser(char * username){
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
            /* Free the memory resources of the user structure */
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
            //Enqueue message
            enqueueMsg(&(temp->pend_msgs_head), msg, updateLastID(temp));
            return;     
        }
        temp = temp->next;
    }

    return;
}

unsigned int updateLastID(){
    //Increment the ID in 1
    last_msg_id = last_msg_id+1;
    //If it results in 0, then the maximum representable number is overflown
    if(last_msg_id == 0) last_msg_id = 1;

    return last_msg_id;
}

void printPendMsgs(char * username){
    struct user *temp = user_head;

    /* Iterate over the list */
    while(temp != NULL){
        if(strcmp(temp->username, username) == 0){  //User found
            printMsgs(&(temp->pend_msgs_head));     
        }
        temp = temp->next;
    }
}

void removePendMsg(char * username){
    struct user *temp = user_head;

    /* Iterate over the list */
    while(temp != NULL){
        if(strcmp(temp->username, username) == 0){  //User found
            removeMsg(&(temp->pend_msgs_head));     
        }
        temp = temp->next;
    }
}

