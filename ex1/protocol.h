#ifndef PROTOCOL_H
#define PROTOCOL_H
#define NUM_MSGS		3

struct request{
	char name[10];				/* For the name of the array */
	int Ni;						/* To indicate the value of N in case of calling init, or 'i' in case of calling set or get */
	int value;					/* For the input parameter of set and get */
	char op;					/* To indicate the operation that we are calling. 0: init; 1: set; 2: get; 3: destroy*/
	char queue[10];				/* Queue name for the server to respond */
};

struct response{
	char value;					/* Value returned by the operations of the server */
	int get_value;				/* To store the value of the get operation */
	char error;					/* Error value returned by the server */
};

/* Functions */
void initializeClient(char* clientID);
int init(char* name, int N);
int set(char* name, int i, int value);
int get(char* name, int i, int * value);
int destroy(char* name);

#endif