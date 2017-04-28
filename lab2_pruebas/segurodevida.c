/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "store_service.h"

/* Define the structure of the message list */
struct msg{
    char body[MAX_SIZE];		/* Content of the message */
    char md5[MAX_MD5];		/* MD5 of the message */
    char sender[MAX_SIZE];	/* Sender of the message */
    char receiver[MAX_SIZE];	/* Receiver of the message */
    unsigned int id;		/* ID assigned to the message */
    struct msg *next;		/* Pointer to the next message in the list */
};

struct user{
	char name[MAX_SIZE];
	unsigned long num_msgs;
	struct user *next;
	struct msg *sent_msgs_head;
};

/* ========================================================== */
/* ======================== HEADERS ========================= */
/* ========================================================== */

struct msg * msg_head;
struct user * usr_head;

int addMsg(struct msg **head,char * message, char * md5, unsigned int id, char * receiver);

bool_t
init_1_svc(void *result, struct svc_req *rqstp)
{
	bool_t retval = TRUE;
	/* Initialize the lists to NULL */
	usr_head = NULL;
	usr_head->next = NULL;
	usr_head->sent_msgs_head = NULL;

	return retval;
}

bool_t
store_1_svc(char *sender, char *receiver, u_int msg_id, char *msg, char *md5, int *result,  struct svc_req *rqstp)
{
	bool_t retval = TRUE;
	printf("1\n");

	struct user *temp = usr_head;
	/* Iterate through the list of users that sent at least one message */
	while(temp != NULL){
		printf("2\n");
		if(strcmp(temp->name, sender) == 0){	//User found in the list
			printf("3\n");
			/* Append the message to the list of sent messages by that user */
			*result = addMsg(&(temp->sent_msgs_head), msg, md5, msg_id, receiver);
			printf("4\n");
			if(*result == -1) return retval;
			/* Update the message counter */
			temp->num_msgs = temp->num_msgs + 1;
			return retval;
		}
		temp = temp->next;
	}
	/* If the code reaches this point, no user was found, so add it to the list and set 
	the message counter to 1 */
	temp = (struct user *) malloc(sizeof(struct user));
	/* If malloc returns error (full memory or other) */
	if(temp == NULL) return -1;
	strcpy(temp->name, sender);
	printf("5\n");
	*result = addMsg(&(temp->sent_msgs_head), msg, md5, msg_id, receiver);
	printf("6\n");
	if(*result == -1) return retval;
	temp->num_msgs = 1;		/* Set the message counter to 1 */

	temp->next = usr_head;
	usr_head = temp;

	return retval;
}

bool_t
getnummessages_1_svc(char *user, int *result,  struct svc_req *rqstp)
{
	bool_t retval = TRUE;

	struct user *temp = usr_head;
	*result = 0;

	while(temp != NULL){
		if(strcmp(temp->name, user) == 0){	//Sender is found in the list
			/* Search for the message with that ID */
			struct msg *msg_temp = temp->sent_msgs_head;
			/* Iterate through the list of sent messages */
			while(msg_temp != NULL){
				*result = *result + 1;
				msg_temp = msg_temp->next;
			}
			return retval;
		}
		temp = temp->next;
	}
	*result = -1;	//User was not found

	return retval;
}

bool_t
getmessage_1_svc(char *user, u_int msg_id, response *result,  struct svc_req *rqstp)
{
	bool_t retval = TRUE;
	
	struct user *temp = usr_head;

	while(temp != NULL){
		if(strcmp(temp->name, user) == 0){	//Sender is found in the list
			/* Search for the message with that ID */
			struct msg *msg_temp = temp->sent_msgs_head;
			/* Iterate through the list of sent messages */
			while(msg_temp != NULL){
				if(msg_temp->id == msg_id){	//Message ID found
					strcpy(result->msg, msg_temp->body);
					strcpy(result->md5, msg_temp->md5);
					return retval;
				}
				msg_temp = msg_temp->next;
			}
			/* At this point, no message with such ID was found for that user. Stop iterating */
			result = NULL;
			return retval;
		}
		temp = temp->next;
	}
	result = NULL; //User was not found, thus message does not exist

	return retval;
}

int
store_service_1_freeresult (SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result)
{
	xdr_free (xdr_result, result);

	/*
	 * Insert additional freeing code here, if needed
	 */

	return 1;
}

/* Creates a new message struct and adds it to the message queue
    Returns 0 if the message is stored
        -1 if malloc error */
int addMsg(struct msg **head, char * message, char * md5, unsigned int id, char * receiver){
    struct msg *temp;
    /* Allocate the space for the new message */
    temp = (struct msg *) malloc(sizeof(struct msg));
    /* If malloc returns error (full memory or other) */
    if(temp == NULL) return -1;
    strcpy(temp->body, message);
    strcpy(temp->md5, md5);
    strcpy(temp->receiver, receiver);
    temp->id = id;
    temp->next = *head; //If msg_head is null, then the list is empty
    *head = temp;
    // if (*msg_head == NULL){      /* Queue is empty */
    //     temp->next = *msg_head;
    //     *msg_head = temp;
    // }
    // else{
    //     /* If the queue is not empty, append it to the beginning */
    //     temp->next = *msg_head;
    //     *msg_head = temp;
    // }
    
    return 0;
}
