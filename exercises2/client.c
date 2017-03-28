#include <netinet/in.h>		/* For addresses in PF_INET */
#include <netdb.h>			/* Address-->Network and Network-->Address library; gethostbyname; gethostbyaddr */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "read_line.h"

#define MAX_LINE	256

int main(int argc, const char** argv){
	struct sockaddr_in server_addr;
	int sd;
	int val;
	struct hostent *hp;
	struct in_addr in;

	if(argc != 3){
		printf("Use: %s <IP address> <Port number>\n", argv[0]);
		exit(1);
	}

	int err = inet_aton(argv[1], &in);
	if(err == 0){
		printf("IP address in format a.b.c.d\n");
		exit(2);
	}

	sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	/* This socket has no address assigned */
	if(sd == -1){
		perror("Error when creating the socket");
		exit(-1);
	}

	//val = 1;
	//setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char*) &val, sizeof(int)); /* Makes the address of the socket reusable */
	bzero((char *)&server_addr, sizeof(server_addr));
	hp = gethostbyaddr((char*) &in, sizeof(in), AF_INET);
	
	memcpy(&(server_addr.sin_addr), hp->h_addr, hp->h_length);	/* Initialize the socket address of the server to 0 */
	server_addr.sin_family			= AF_INET;
	//server_addr.sin_addr.s_addr		= INADDR_ANY;	/* Listens to all addresses */
	server_addr.sin_port			= htons(10200);	/* Port number */

	if((connect(sd, (struct sockaddr*) &server_addr, sizeof(server_addr))) == -1){
		perror("Error when connecting to the server");
		exit(-1);
	}

	char req_buffer[MAX_LINE];
	char res_buffer[MAX_LINE];
	int n;
	int m;

	while (1) {
		n = readLine(0, req_buffer, MAX_LINE);
		if (n!=-1)
		//send_msg(sd, req_buffer, n);
		write(sd, req_buffer, n);
/*
		m = readLine(sd, res_buffer, MAX_LINE);
		printf("Recieived from server\n");
		if (n!=-1)
		write(1, res_buffer, m);*/
	}

	close(sd);
	exit(0);

}