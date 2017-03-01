#ifndef PROTOCOL_H
#define PROTOCOL_H
#define NUM_MSGS		1

struct request{
	char *name;					/* For the name of the array */
	int Ni;						/* To indicate the value of N in case of calling init, or 'i' in case of calling set or get */
	int value;					/* For the input parameter of set and get */
	char op;					/* To indicate the operation that we are calling. 0: init; 1: set; 2: get; 3: destroy*/
	char *queue;				/* Queue name for the server to respond */
};

struct response{
	char value;					/* Value returned by the operations of the server */
	char error;					/* Error value returned by the server */
};

#endif