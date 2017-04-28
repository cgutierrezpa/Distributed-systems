
#include "rpc_store_service/store_service.h"

void
store_service_1(char *host)
{
	CLIENT *clnt;
	/*
	enum clnt_stat retval_1;
	void *result_1;
	enum clnt_stat retval_2;
	int result_2;
	char *store_1_sender;
	char *store_1_receiver;
	u_int  store_1_msg_id;
	char *store_1_msg;
	char *store_1_md5;
	enum clnt_stat retval_3;
	int result_3;
	char *getnummessages_1_user;
	enum clnt_stat retval_4;
	response result_4;
	char *getmessage_1_user;
	u_int  getmessage_1_msg_id;

#ifndef	DEBUG
	clnt = clnt_create (host, STORE_SERVICE, STORE_VERSION, "tcp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	*//* DEBUG */
	/*
	retval_1 = init_1(&result_1, clnt);
	if (retval_1 != RPC_SUCCESS) {
		clnt_perror (clnt, "call failed");
	}
	retval_2 = store_1(store_1_sender, store_1_receiver, store_1_msg_id, store_1_msg, store_1_md5, &result_2, clnt);
	if (retval_2 != RPC_SUCCESS) {
		clnt_perror (clnt, "call failed");
	}
	retval_3 = getnummessages_1(getnummessages_1_user, &result_3, clnt);
	if (retval_3 != RPC_SUCCESS) {
		clnt_perror (clnt, "call failed");
	}
	retval_4 = getmessage_1(getmessage_1_user, getmessage_1_msg_id, &result_4, clnt);
	if (retval_4 != RPC_SUCCESS) {
		clnt_perror (clnt, "call failed");
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 *//* DEBUG */
	
	clnt = clnt_create (host, STORE_SERVICE, STORE_VERSION, "tcp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}

	int result;
	// init_1(&result, clnt);
	//printf("Result of init: %d\n",result);
	store_1("sender1", "receiver1", 1, "blablabla", "blablamd5", &result, clnt);
	// printf("Result of the send: %d\n", result);
	// store_1("sender1", "receiver2", 2, "blablabla2", "blablamd52", &result, clnt);
	// printf("Result of the send: %d\n", result);
	// store_1("sender2", "receiver1", 1, "blablabla11", "blablamd511", &result, clnt);
	// printf("Result of the send: %d\n", result);
	// store_1("sender2", "receiver1", 2, "blablabla22", "blablamd522", &result, clnt);
	// printf("Result of the send: %d\n", result);
	// store_1("sender2", "receiver1", 3, "blablabla22", "blablamd533", &result, clnt);
	// printf("Result of the send: %d\n", result);
	// store_1("sender1", "receiver1", 3, "blablabla3", "blablamd53", &result, clnt);
	// printf("Result of the send: %d\n", result);
	// store_1("sender1", "receiver1", 4, "blablabla4", "blablamd54", &result, clnt);
	// printf("Result of the send: %d\n", result);
	// store_1("sender1", "receiver1", 5, "blablabla5", "blablamd55", &result, clnt);
	// printf("Result of the send: %d\n", result);

	response *res = malloc(sizeof(response));
	/*int size;
	getmessage_1("sender1", 1, &res, clnt);
	printf("Result of the receive: %d\n", result);
	size = strlen(res.msg);
		printf("Length msg: %d\n", size);
	printf("Mess: %s\n", res.msg);
	printf("MD5: %s\n", res.md5);
	getmessage_1("sender2", 2, &res, clnt);
	printf("Result of the receive: %d\n", result);
	size = strlen(res.msg);
		printf("Length msg: %d\n", size);
	printf("Mess: %s\n", res.msg);
	printf("MD5: %s\n", res.md5);
	getmessage_1("sender1", 4, &res, clnt);
	printf("Result of the receive: %d\n", result);
	size = strlen(res.msg);
		printf("Length msg: %d\n", size);
	printf("Mess: %s\n", res.msg);
	printf("MD5: %s\n", res.md5);
	getmessage_1("sender3", 1, &res, clnt);
	printf("Result of the receive: %d\n", result);
	size = strlen(res.msg);
		printf("Length msg: %d\n", size);

	printf("Mess: %s\n", res.msg);
	printf("MD5: %s\n", res.md5);
	getmessage_1("sender1", 6, &res, clnt);
	printf("Result of the receive: %d\n", result);
	size = strlen(res.msg);
		printf("Length msg: %d\n", size);

	printf("Mess: %s\n", res.msg);
	printf("MD5: %s\n", res.md5);
	*/
	getmessage_1("hello", 7, res, clnt);//) printf("ERROR , SERVICE NOT AVAILABLE\n");
	printf("After\n");
	if(strlen(res->msg) == 0) printf ("ERROR , MESSAGE DOES NOT EXIST\n");
	printf("MESS: %s\n", res->msg);
	printf("MD5: %s\n", res->md5);



	clnt_destroy (clnt);
}


int
main (int argc, char *argv[])
{
	char *host;

	if (argc < 3) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}

	host = "127.0.0.1";
	char *stopstring;
	unsigned int id = strtoul(argv[2], &stopstring, 10);

	CLIENT *clnt;

	clnt = clnt_create (host, STORE_SERVICE, STORE_VERSION, "tcp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
	/* Allocate resources for the response */
	response *res = malloc(sizeof(response));
	res->msg = calloc(MAX_SIZE, sizeof(char));
	res->md5 = calloc(MAX_MD5, sizeof(char));
	getmessage_1(argv[1], id, res, clnt);//) printf("ERROR , SERVICE NOT AVAILABLE\n");

	if(strlen(res->msg) == 0) printf ("ERROR , MESSAGE DOES NOT EXIST\n");
	else{
		printf("MESS: %s\n", res->msg);
		printf("MD5: %s\n", res->md5);
	}
	

	clnt_destroy (clnt);
exit (0);
}
