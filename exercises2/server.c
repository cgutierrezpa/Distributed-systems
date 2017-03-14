#include <netinet/in.h>		/* For addresses in PF_INET */
#include <netdb.h>			/* Address-->Network and Network-->Address library; gethostbyname; gethostbyaddr */
#include <sys/types.h>
#include <sys/socket.h>

int main(void){
	struct sockaddr_in server_addr, client_addr;
	int sd;
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
	server_addr.sin_port			= htons(4200);	/* Port number */

	bind(sd, &server_addr, sizeof(server_addr));

	listen(sd, 5); /* Backlog is 5, yo maximum number of queued requests is 5 */

	size = sizeof(client_addr);

	while(1){
		printf("Waiting for connection...\n");

		sc = accept(sd, (struct sockaddr *) &client_addr, &size);

		recv(sc, (char*) num, 2*sizeof(int), 0);

		res = num[0] + num[1];

		send(sc, &res, sizeof(int), 0);

		close(sc);
	}

	close(sd);
	exit(0);

}