#include <netinet/in.h>		/* For addresses in PF_INET */
#include <netdb.h>			/* Address-->Network and Network-->Address library; gethostbyname; gethostbyaddr */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include "read_line.h"
#include "user_list.h"
#include "msg_list.h"

#define MAX_OP 	11
#define TRUE 	1
#define FALSE 	0

int main(int argc, char * argv[]){
	struct sockaddr_in server_addr, client_addr;
	int sd, sc;
	int val;
	int server_port;

	/* Check command */
	if(argc != 3 || strcmp(argv[1],"-p") != 0){
		printf("Usage: ./server -p <port>\n");
		exit(-1);
	} 

	/* Check if the port number passed as parameter is valid */
	server_port = atoi(argv[2]);
	if ((server_port < 1024) || (server_port > 65535)) {
			printf("Error: Port must be in the range 1024 <= port <= 65535\n");
			exit(-1);
	}

	//printf();

	sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	/* This socket has no address assigned */
	if(sd == -1){
		perror("Error when creating the socket");
		exit(-1);
	}

	val = 1;
	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char*) &val, sizeof(int)); /* Makes the address of the socket reusable */
	
	bzero((char*) &server_addr, sizeof(server_addr));	/* Initialize the socket address of the server to 0 */
	server_addr.sin_family			= AF_INET;
	server_addr.sin_addr.s_addr		= INADDR_ANY;	/* Listens to all addresses */
	server_addr.sin_port			= htons(server_port);	/* Port number */

	if((bind(sd, (struct sockaddr*) &server_addr, sizeof(server_addr))) == -1){
		perror("Error when binding the address to the socket");
		exit(-1);
	}

	if(listen(sd, 5) == -1){
		perror("Error when listening to the socket");
		exit(-1);
	} /* Backlog is 5, yo maximum number of queued requests is 5 */

	socklen_t size = sizeof(client_addr);

	//Server will receive
	char operation_buff[MAX_OP];
	char user_buff[MAX_USERNAME];
	char msg_buff[MAX_MSG];
	int n;
	int m;
	

	while(1){
		printf("s> ");

		sc = accept(sd, (struct sockaddr *) &client_addr, &size);
		if(sc == -1){
			perror("Error when accepting the connection");
			exit(-1);
		}

		n = readLine(sc, operation_buff, MAX_OP);
		if (n!=-1){
			if (strcmp(operation_buff, "REGISTER") == 0){
			}
			else{
				printf("OPERATION NOT KNOWN...");
			}
		}

		m = readLine(sc, user_buff, MAX_USERNAME);

		uint16_t client_port = client_addr.sin_port;

		struct sockaddr_in test_addr;

		getpeername(sc, (struct sockaddr *) &test_addr, (socklen_t *) sizeof(test_addr));
		//test_addr.sin_addr is of type struct in_addr
		char * ip = inet_ntoa(test_addr.sin_addr);
		printf("CLIENT ADDRESS: %s\n", ip);
		printf("CLIENT PORT NUMBER: %d\n", client_port);

		char res = '0';

		send_msg(sc, &res, sizeof(res));
		close(sc);
		
	}

	close(sd);
	exit(0);

}
