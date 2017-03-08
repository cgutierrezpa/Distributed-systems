#include <sys/stat.h>		/* Modes */
#include <fcntl.h>			/* Flags */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mqueue.h>
#include <string.h>
#include "protocol.h"

int main(void){
	int val;

	initializeClient("/CLIENT1");
	printf("INIT: %d\n", init("vector1", 100));
	printf("INIT: %d\n", init("vector2", 200));
	printf("SET: %d\n", set("vector1", 0, 40));
	printf("SET: %d\n", set("vector1", 120, 30));
	printf("INIT: %d\n", init("vector1", 200));
	printf("GET: %d\n", get("vector1", 0, &val));
	printf("GET result: %d\n", val);
	printf("DESTROY: %d\n", destroy("vector1"));
	printf("DESTROY: %d\n", destroy("vector"));

	exit(0);
}