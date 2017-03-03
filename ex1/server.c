#include <sys/stat.h>		/* Modes */
#include <fcntl.h>			/* Flags */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <mqueue.h>
#include <string.h>

mqd_t serverq;
mqd_t clientq;

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

/* Handler for interrupts */
void interruptHandler(int sig){
	printf("[SERVER]: Handling interrupt. Closing server queue...\n");
	mq_close(serverq);
    mq_unlink(serverQname);
    exit(0);
}

int main(void){
	struct request req;
	struct response res;

	/* Set queue attributes for server queue */
	struct mq_attr att;
	att.mq_maxmsg = 5;
	att.mq_msgsize = sizeof(struct request);

	printf("[SERVER]: Waiting for messages...\n");

	signal(SIGINT, interruptHandler); /* Handles the ctrl+c signal to interrupt the server */

	while(1){

		/* Open the queue for the server */
		if((serverq = mq_open(serverQname, O_RDONLY|O_CREAT, 0777, &att)) == -1){
			perror("[SERVER]: Error when opening the server queue\n");
			exit(-1);
		}

		/* Receive Request */
        if(mq_receive(serverq, (char *) &req, sizeof(struct request), 0) == -1) {
            perror("[SERVER]: Error when receiving message from the server queue");
            exit(-1);
	    }
	    req = (struct request) req;
	    //struct request aux;
	    //memcpy((char *) &aux, (char *) &req, sizeof(struct request));

	    printf("[SERVER]: I hear from the client:\n");
	    printf("%s\n", req.name);
	    printf("%d\n", req.Ni);
	    printf("%d\n", req.op);
	    printf("%s\n", req.queue);

	    /* Open the queue for the client */
		if((clientq = mq_open(req.queue, O_WRONLY, 0777, NULL)) == -1){
			perror("[SERVER]: Error when opening the client queue\n");
			mq_close(clientq);
			exit(-1);
		}
		res.value = 9;

	    /* Send Response */
	    if(mq_send(clientq, (char *) &res, sizeof(struct response), 0) == -1){
	    	perror("[SERVER]: Error when sending message to the client queue");
	    	mq_close(clientq);
            exit(-1);
	    }

	    mq_close(clientq);
	}

	return 0;
}