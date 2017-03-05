#include <sys/stat.h>		/* Modes */
#include <fcntl.h>			/* Flags */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mqueue.h>
#include <string.h>
#include "protocol.h"

int main(void){
	//int * getValue = malloc(sizeof(int));

	initializeClient("/CLIENT1");
	printf("INIT: %d\n", init("vector1", 100));
	printf("INIT: %d\n", init("vector2", 200));
	printf("SET: %d\n", set("vector1", 0, 40));
	printf("SET: %d\n", set("vector1", 120, 30));
	printf("INIT: %d\n", init("vector1", 200));
	printf("DESTROY: %d\n", destroy("vector1"));
	printf("DESTROY: %d\n", destroy("vector"));
	/*printf("SET: %d\n", init("vector1", 200));
	printf("GET: %d\n", get("vector1", 0, getValue));
	printf("Value is: %d\n", *getValue);
	printf("SET: %d\n", set("vector2", 0, 40));
	printf("SET: %d\n", set("vector1", 50, 40));
	printf("DESTROY: %d\n", destroy("vector1"));
	printf("SET: %d\n", set("vector1", 20, 40));
	printf("INIT: %d\n", init("vector2", 8));
	printf("INIT: %d\n", init("vector1", 8));
	printf("INIT: %d\n", init("vector1", 10));
	printf("INIT: %d\n", init("vector1", 10));
	printf("INIT: %d\n", init("vector1", 12));
	printf("INIT: %d\n", init("vector3", 12));
	printf("INIT: %d\n", init("vector3", 2));
	printf("INIT: %d\n", init("vector3", 12));*/

	exit(0);
}