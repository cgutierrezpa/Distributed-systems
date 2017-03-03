#include <sys/stat.h>		/* Modes */
#include <fcntl.h>			/* Flags */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mqueue.h>
#include <string.h>
#include "protocol.h"

int main(void){
	initializeClient("/CLIENT1");
	printf("INIT: %d\n", init("vector1", 8));

	exit(0);
}