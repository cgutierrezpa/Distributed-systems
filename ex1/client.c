#include <sys/time.h>
#include <sys/stat.h>		/* Modes */
#include <fcntl.h>			/* Flags */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <mqueue.h>
#include "responses.h"

//#define MAX_BUFFER		1024
#define NUM_MSGS		1

int main(int argc, char *argv[]){
	mqd_t clientq;
	mqd_t serverq;
	struct mq_attr att;
	struct request req;
	struct response res;

	req.name = "vector1";
	//sprintf(req.queue, "vector1");
	req.Ni = 20;
	req.op = 0;
	att.mq_maxmsg = NUM_MSGS;
	att.mq_msgsize = sizeof(struct request);
	//sprintf(req.queue, "/CLIENT1");
	//clientq = mq_open("/CLIENT1", O_RDONLY|O_CREAT, 777, &att);

	printf("%s\n", req.name);
	printf("%d\n", req.Ni);
	printf("%d\n", req.op);

	//mq_close(clientq);
	return 1;
}