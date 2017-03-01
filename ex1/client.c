#include <sys/time.h>
#include <sys/stat.h>		/* Modes */
#include <fcntl.h>			/* Flags */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <mqueue.h>
#include <string.h>
#include "protocol.h"

int main(void){
	mqd_t clientq;
	mqd_t serverq;
	struct mq_attr att;
	struct request req;
	struct response res;

	/* Initialize request parameters */
	req.name = "vector1";
	req.Ni = 20;			//N
	req.op = 0;				//Init
	req.queue = "/CLIENT1";

	/* Set queue attributes */
	att.mq_maxmsg = NUM_MSGS;
	att.mq_msgsize = sizeof(struct request);
	
	/* Open the queue */
	if((clientq = mq_open(req.queue, O_RDONLY|O_CREAT, 0777, &att)) == -1){
		perror("Error when opening the queue\n");
		exit(-1);
	}


	mq_close(clientq);
	
	exit(0);
}