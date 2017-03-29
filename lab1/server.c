#include <netinet/in.h>		/* For addresses in PF_INET */
#include <netdb.h>			/* Address-->Network and Network-->Address library; gethostbyname; gethostbyaddr */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "read_line.h"

#define MAX_LINE	256

int main(void){
	struct sockaddr_in server_addr, client_addr;
	int sd, sc;
	int val;

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
	server_addr.sin_port			= htons(10200);	/* Port number */

	if((bind(sd, (struct sockaddr*) &server_addr, sizeof(server_addr))) == -1){
		perror("Error when binding the address to the socket");
		exit(-1);
	}

	if(listen(sd, 5) == -1){
		perror("Error when listening to the socket");
		exit(-1);
	} /* Backlog is 5, yo maximum number of queued requests is 5 */

	socklen_t size = sizeof(client_addr);

	char req_buffer[MAX_LINE];
	//char res_buffer[MAX_LINE];
	int n;
	

	while(1){
		printf("Waiting for connection...\n");

		sc = accept(sd, (struct sockaddr *) &client_addr, &size);
		if(sc == -1){
			perror("Error when accepting the connection");
			exit(-1);
		}

		n = read(sc, req_buffer, MAX_LINE);
		printf("Recieived from client\n");
		if (n!=-1)
		write(1, req_buffer, n);

		

		send_msg(sc, req_buffer, MAX_LINE);
		close(sc);
		
	}

	close(sd);
	exit(0);

}
