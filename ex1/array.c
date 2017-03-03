#include <sys/stat.h>		/* Modes */
#include <fcntl.h>			/* Flags */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mqueue.h>
#include <string.h>
#include "protocol.h"

/* Global queue variables */
char* serverQname = "/SERVER"; /* There is only one server with one queue */
char* clientQname; /* To store the name of the client queue */

void initializeClient(char* clientID) {
    clientQname = clientID;
}

int init(char* name, int N){
	mqd_t clientq;
	mqd_t serverq;

	struct request req;
	struct response res;

	/* Initialize request parameters */
	strcpy(req.name, name);
	req.Ni = N;				//N
	req.op = 0;				//Init
	strcpy(req.queue, clientQname);

	/* Set queue attributes for client queue */
	struct mq_attr att;
	att.mq_maxmsg = NUM_MSGS;
	att.mq_msgsize = sizeof(struct response);
	
	/* Open the queue for the client */
	if((clientq = mq_open(req.queue, O_RDONLY|O_CREAT, 0777, &att)) == -1){
		perror("[CLIENT]: Error when opening the client queue\n");
		exit(-1);
	}

	/* Open the queue for the server */
	if((serverq = mq_open(serverQname, O_WRONLY, 0777, NULL)) == -1){
		perror("[CLIENT]: Error when opening the server queue\n");
		exit(-1);
	}

	/* Send Request */
    if(mq_send(serverq, (char *) &req, sizeof(struct request), 0) == -1) {
        perror("[CLIENT]: Error when sending message to the server queue");
        mq_close(clientq);
        mq_unlink(req.queue);
        exit(-1);
	}

    /* Receive Response */
    if(mq_receive(clientq, (char *) &res, sizeof(struct response), 0) == -1) {
        perror("[CLIENT]: Error when receiving response from the server at the client queue");
        mq_close(clientq);
        mq_unlink(req.queue);
        exit(-1);
	}

	mq_close(clientq);
    mq_unlink(req.queue);
    mq_unlink(serverQname);
    res = (struct response) res;
    return ((int) res.value);
}