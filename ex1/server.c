#include <sys/stat.h>		/* Modes */
#include <fcntl.h>			/* Flags */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <mqueue.h>
#include <string.h>

mqd_t serverq;

/* Define the nodes of the list of arrays */
typedef struct node {
    char name[10];
    int * v;
    int N;				/* Stores the size of the array */
    struct node * next;
} node_t;

/* Declare the pointer to the head of the list. Visible for every thread */
node_t * list_head;

/* Structure for Requests */
struct request{
	char name[10];					/* For the name of the array */
	int Ni;						/* To indicate the value of N in case of calling init, or 'i' in case of calling set or get */
	int value;					/* For the input parameter of set and get */
	char op;					/* To indicate the operation that we are calling. 0: init; 1: set; 2: get; 3: destroy*/
	char queue[10];				/* Queue name for the server to respond */
};

/* Structure for Responses */
struct response{
	char value;					/* Value returned by the operations of the server */
	char error;					/* Error value returned by the server */
};

/* Global queue variables */
char* serverQname = "/SERVER"; /* There is only one server with one queue */

/* Operations: ------------------------------------------------------------- */
/* 0 = init();                                                               */
/* 1 = set();                                                                */
/* 2 = get();                                                                */
/* 3 = destroy();                                                            */
/* ------------------------------------------------------------------------- */

/* Function headers */
int init(char * name, int N);
int set(char * name, int i, int value);
int get(char * name, int i, int * value);
int destroy(char * name);

/* Handler for interrupts */
void interruptHandler(int sig){
	printf("[SERVER]: Handling interrupt. Closing server queue...\n");
	mq_close(serverq);
    mq_unlink(serverQname);
    exit(0);
}

/* Mutex & Threads */
pthread_mutex_t mtx;

void prepareThread(struct request * req){
	/* Declare the client queue variable */
	mqd_t clientq;

	/* Create a private local copy of the request struct for each thread */
	struct request local_req;
	strcpy(local_req.name, req->name);
	local_req.Ni = req->Ni;
	local_req.value = req->value;
	local_req.op = req->op;
	strcpy(local_req.queue, req->queue);

	/*printf("[THREAD]: I hear from the client:\n");
	    printf("%s\n", local_req.name);
	    printf("%d\n", local_req.Ni);
	    printf("%d\n", local_req.op);
	    printf("%s\n", local_req.queue);*/

	/* Declare the response variable */
	struct response res;

	switch(local_req.op){
		case 0:
			/* Open the queue for the client */
			if((clientq = mq_open(local_req.queue, O_WRONLY, 0777, NULL)) == -1){
				perror("[SERVER]: Error when opening the client queue\n");
				mq_close(clientq);
				exit(-1);
			}
			//pthread_mutex_lock(&mtx);
			res.value = init(local_req.name, local_req.Ni);
			//pthread_mutex_unlock(&mtx);

	    	/* Send Response */
	    	if(mq_send(clientq, (char *) &res, sizeof(struct response), 0) == -1){
	    		perror("[SERVER]: Error when sending message to the client queue");
	    		mq_close(clientq);
            	exit(-1);
	    	}

	    	mq_close(clientq);
			break;

		case 1:
			/* Open the queue for the client */
			if((clientq = mq_open(local_req.queue, O_WRONLY, 0777, NULL)) == -1){
				perror("[SERVER]: Error when opening the client queue\n");
				mq_close(clientq);
				exit(-1);
			}
			//pthread_mutex_lock(&mtx);
			res.value = set(local_req.name, local_req.Ni, local_req.value);
			//pthread_mutex_unlock(&mtx);

	    	/* Send Response */
	    	if(mq_send(clientq, (char *) &res, sizeof(struct response), 0) == -1){
	    		perror("[SERVER]: Error when sending message to the client queue");
	    		mq_close(clientq);
            	exit(-1);
	    	}

	    	mq_close(clientq);
			break;

		case 2:
			/* Open the queue for the client */
			if((clientq = mq_open(local_req.queue, O_WRONLY, 0777, NULL)) == -1){
				perror("[SERVER]: Error when opening the client queue\n");
				mq_close(clientq);
				exit(-1);
			}
			//pthread_mutex_lock(&mtx);
			res.value = get(local_req.name, local_req.Ni, &local_req.value);
			//pthread_mutex_unlock(&mtx);

	    	/* Send Response */
	    	if(mq_send(clientq, (char *) &res, sizeof(struct response), 0) == -1){
	    		perror("[SERVER]: Error when sending message to the client queue");
	    		mq_close(clientq);
            	exit(-1);
	    	}

	    	mq_close(clientq);
			break;

		case 3:
			/* Open the queue for the client */
			if((clientq = mq_open(local_req.queue, O_WRONLY, 0777, NULL)) == -1){
				perror("[SERVER]: Error when opening the client queue\n");
				mq_close(clientq);
				exit(-1);
			}
			//pthread_mutex_lock(&mtx);
			res.value = destroy(local_req.name);
			//pthread_mutex_unlock(&mtx);

	    	/* Send Response */
	    	if(mq_send(clientq, (char *) &res, sizeof(struct response), 0) == -1){
	    		perror("[SERVER]: Error when sending message to the client queue");
	    		mq_close(clientq);
            	exit(-1);
	    	}

	    	mq_close(clientq);
			break;
	}
	pthread_exit(NULL);
}

int main(void){

	/* Initialize the head of the list of arrays to NULL, as the list is initially empty */
	list_head = malloc(sizeof(node_t)); 	/* First node is an empty node */

	/* Initializing mutex */
    if(pthread_mutex_init(&mtx, NULL)!=0) {
        perror("[SERVER]: Error when initializing the mutex");
        exit(-1);
    }

    /* Prepare thread conditions */
    pthread_t thread;
    pthread_attr_t thread_att;
    pthread_attr_init(&thread_att);
    pthread_attr_setdetachstate(&thread_att, PTHREAD_CREATE_DETACHED);

    /* Prepare request and response variables */
	struct request req;

	/* Set queue attributes for server queue */
	struct mq_attr att;
	att.mq_maxmsg = 5;
	att.mq_msgsize = sizeof(struct request);

	signal(SIGINT, interruptHandler); /* Handles the ctrl+c signal to interrupt the server */

	while(1){

		/* Open the queue for the server */
		if((serverq = mq_open(serverQname, O_RDONLY|O_CREAT, 0777, &att)) == -1){
			perror("[SERVER]: Error when opening the server queue\n");
			exit(-1);
		}

		printf("[SERVER]: Waiting for messages...\n");

		/* Receive Request */
        if(mq_receive(serverq, (char *) &req, sizeof(struct request), 0) == -1) {
            perror("[SERVER]: Error when receiving message from the server queue");
            exit(-1);
	    }
	    req = (struct request) req;
	    //struct request aux;
	    //memcpy((char *) &aux, (char *) &req, sizeof(struct request));

	    /*printf("[SERVER]: I hear from the client:\n");
	    printf("%s\n", req.name);
	    printf("%d\n", req.Ni);
	    printf("%d\n", req.op);
	    printf("%s\n", req.queue);*/

	    /* Initializing mutex */
    	if(pthread_create(&thread, &thread_att, (void *) prepareThread, &req)!=0) {
        	perror("[SERVER]: Error when creating the thread");
        	exit(-1);
    	}

	}

	return 0;
}

int init(char * name, int N){
	
	/* Iterate over the list to check if the vector already exists */
	node_t * current = list_head;
	printf("[INIT]\n");
	pthread_mutex_lock(&mtx);
	while(current->next != NULL){
		printf("Current name: %s\n", current->name);
		printf("Current N: %d\n", current->N);
		/* Compare if the name of the vector already exists in the list */
		if(strcmp(current->name, name) == 0 && current->N == N){
			pthread_mutex_unlock(&mtx);
			return 0;
		}
		else if(strcmp(current->name, name) == 0 && current->N != N){
			pthread_mutex_unlock(&mtx);
			return -1;
		}
		current = current->next;
	}

	/* At this point no vector was found with the same name, so create it */
	current->next = malloc(sizeof(node_t));
	strcpy(current->next->name, name);
	current->next->v = malloc(sizeof(int)*N);
	current->next->N = N;
	current->next->next = NULL;

	pthread_mutex_unlock(&mtx);
	return 1;
}

int set(char * name, int i, int value){
	
	/* Iterate over the list to look for the vector */
	node_t * current = list_head;
	pthread_mutex_lock(&mtx);
	while(current != NULL){
		printf("[SET]\n");
		printf("Current name: %s\n", current->name);
		printf("Current N: %d\n", current->N);
		printf("Value of i: %d\n", i);
		printf("Value of 'value': %d\n", value);
		if(strcmp(current->name, name) == 0){
			/* Check the size of the array */
			if((current->N-1 < i) || (i < 0)){
				printf("ESTOY DENTROOOO\n");
				pthread_mutex_unlock(&mtx);
				return -1;
			}

			current->v[i] = value;
			printf("HE AÑADIDO EL VALOOOR\n");
			pthread_mutex_unlock(&mtx);
			return 0;
		}
		current = current->next;
	}

	/* If the function reaches this point, it means that the vector did not exist in the list */
	printf("ESTOY FUERAAAA\n");
	pthread_mutex_unlock(&mtx);
	return -1;
}

int get(char * name, int i, int * value){

	/* Iterate over the list to look for the vector */
	node_t * current = list_head;
	pthread_mutex_lock(&mtx);
	while(current != NULL){
		printf("[GET]\n");
		printf("Current name: %s\n", current->name);
		printf("Current N: %d\n", current->N);
		printf("Value of i: %d\n", i);
		if(strcmp(current->name, name) == 0){
			/* Check the size of the array */
			if((current->N-1 < i) || (i < 0)){
				printf("NO PUEDO COGER UN VALOR DE UNA POSICION QUE NO EXISTE MAQUINA!!\n");
				pthread_mutex_unlock(&mtx);
				return -1;
			}

			*value = current->v[i];
			printf("HE PILLADO EL VALOOOOR!!!\n");
			printf("Value is: %d\n", *value);
			pthread_mutex_unlock(&mtx);
			return 0;
		}
		current = current->next;
	}
	/* If the function reaches this point, it means that the vector did not exist in the list */
	printf("NO EXISTE EL VECTOR MAQUINAAA!\n");
	pthread_mutex_unlock(&mtx);
	return -1;
}

int destroy(char * name){

	/* Iterate over the list to look for the vector */
	node_t * current = list_head;
	node_t * temp_node = NULL;		/* Temporal node */
	pthread_mutex_lock(&mtx);
	printf("[DESTROY]\n");
	while(current != NULL){
		printf("Current name: %s\n", current->name);
		if(current->next != NULL){
			temp_node = current->next;
				if(strcmp(temp_node->name, name) == 0){
					printf("HE PILLADO EL VECTOOOR!!!\n");
					printf("DESTRUIR VECTOR: %s\n", temp_node->name);
					/* Delete the node */
					current->next = temp_node->next;
					free(temp_node);
					pthread_mutex_unlock(&mtx);
					return 1;
				}
			}
		current = current->next;
	}
	printf("NO SE ENCONTRO VECTOR PARA DESTRUIR");
	pthread_mutex_unlock(&mtx);
	return -1;
}

