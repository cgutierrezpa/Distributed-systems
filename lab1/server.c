#include <netinet/in.h>		/* For addresses in PF_INET */
#include <netdb.h>			/* Address-->Network and Network-->Address library; gethostbyname; gethostbyaddr */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <strings.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include "read_line.h"
#include "user_list.h"
#include "msg_list.h"
#include "server.h"

/* Handler for interrupts */
void interruptHandler(int sig){
	printf("[SERVER]: Handling interrupt. Closing server socket...\n");
    exit(0);
}

int main(int argc, char * argv[]){
	struct sockaddr_in server_addr, client_addr;
	int sd, sc;
	int val;
	int server_port;
	struct hostent *hp;


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

	/* Initializing mutex */
    if(pthread_mutex_init(&socket_mtx, NULL) != 0) {
        perror("[SERVER]: Error when initializing the mutex");
        exit(-1);
    }
    if(pthread_mutex_init(&users_list_mtx, NULL) != 0) {
        perror("[SERVER]: Error when initializing the mutex");
        exit(-1);
    }
    if(pthread_cond_init(&free_socket, NULL) != 0) {
        perror("[SERVER]: Error when initializing the mutex");
        exit(-1);
    }
    if(pthread_cond_init(&free_list, NULL) != 0) {
        perror("[SERVER]: Error when initializing the mutex");
        exit(-1);
    }

	/* Prepare thread conditions */
	pthread_attr_init(&thread_att);
    pthread_attr_setdetachstate(&thread_att, PTHREAD_CREATE_DETACHED);

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
	struct in_addr in;

	hp = gethostbyaddr((char*) &server_addr, sizeof(server_addr), AF_INET);

		char **p;
	
		for (p = hp->h_addr_list; *p != 0; p++){
			memcpy(&in.s_addr, *p, sizeof(in.s_addr));
			printf("List: %s\t%s\n", inet_ntoa(in), hp->h_name);
		}
	
	/* Initial prompt */
	//printf("s> init server %s:%d\n", inet_ntoa(in), ntohs(server_addr.sin_port));

	if(listen(sd, 5) == -1){
		perror("Error when listening to the socket");
		exit(-1);
	} /* Backlog is 5, maximum number of queued requests is 5 */

	socklen_t size = sizeof(client_addr);

	busy_socket = TRUE;
	busy_list = FALSE;
	
	signal(SIGINT, interruptHandler); /* Handles the ctrl+c signal to interrupt the server */

	while(1){
		fprintf(stderr, "%s", "s> ");	/* Prompt */

		sc = accept(sd, (struct sockaddr *) &client_addr, &size);
		if(sc == -1){
			perror("Error when accepting the connection");
			exit(-1);
		}
		thread = (pthread_t) malloc((sizeof(thread)));
		if(pthread_create(&thread, &thread_att, (void *) manageRequest, &sc) != 0) {
        	perror("[SERVER]: Error when creating the thread");
        	exit(-1);
    	}

		/* Wait for the thread to copy the socket descriptor locally */
		pthread_mutex_lock(&socket_mtx);
		while(busy_socket == TRUE) 
			pthread_cond_wait(&free_socket, &socket_mtx);
		busy_socket = TRUE;
		pthread_mutex_unlock(&socket_mtx);
		
		/* Close the created socket */
		printf("\n");
	}

	close(sd);
	exit(0);

}

void * manageRequest(int *sd){
	int s_local;
	char operation_buff[MAX_OP];
	char user_buff[MAX_USERNAME];
	char msg_buff[MAX_MSG];
	int n;
	int m;
	char out;

	/* Copy locally the socket descriptor */
	pthread_mutex_lock(&socket_mtx);
	s_local = *sd;
	busy_socket = FALSE;
	pthread_cond_signal(&free_socket);
	pthread_mutex_unlock(&socket_mtx);

	/* Read the operation */
	n = readLine(s_local, operation_buff, MAX_OP);
	if(n == -1){
		perror("[SERVER_THREAD]: Error when reading from the socket\n");
		/* Send Fail code to client side */
		/*
		out ='2';
		send_msg(s_local, &out, sizeof(out));
		*/
		exit(-1);
	}
	/* Read the username and convert to uppercase */
	m = readLine(s_local, user_buff, MAX_USERNAME);
	if(m == -1){
		perror("[SERVER_THREAD]: Error when reading from the socket\n");
		/* Send Fail code to client side */
		/*
		out ='2';
		send_msg(s_local, &out, sizeof(out));
		*/
		exit(-1);
	}
	toUpperCase(user_buff);

	/* Check the operation */
	if (strcmp(operation_buff, "REGISTER") == 0){
		/* Register the user */
		pthread_mutex_lock(&users_list_mtx);
		//////////////////////////////////////////////////////
		/*                      DOUBT                       */
		//////////////////////////////////////////////////////
		/* Wait while the list of users is being accessed */
		/*
		while(free_list == TRUE)
			pthread_cond_wait(&free_list, &users_list_mtx);
		free_list = TRUE;
		*/
		out = registerUser(user_buff);
		pthread_mutex_unlock(&users_list_mtx);
		/*
		switch(result){
			case 0:
				out = '0';
				break;
			case 1:
				out = '1';
				break;
			default:
				out = '2';
		}*/
	}
	else if (strcmp(operation_buff, "UNREGISTER") == 0){
		/* Unregister the user */
		pthread_mutex_lock(&users_list_mtx);
		//////////////////////////////////////////////////////
		/*                      DOUBT                       */
		//////////////////////////////////////////////////////
		/* Wait while the list of users is being accessed */
		/*
		while(free_list == TRUE)
			pthread_cond_wait(&free_list, &users_list_mtx);
		free_list = TRUE;
		*/
		out = unregisterUser(user_buff);
		pthread_mutex_unlock(&users_list_mtx);
		/*switch(result){
			case 0:
				out = '0';
				break;
			case 1:
				out = '1';
				break;
			default:
				out = '2';
		}*/
	}
	else if(strcmp(operation_buff, "CONNECT") == 0){

		struct sockaddr_in client_addr_local;
		socklen_t addr_len = sizeof(client_addr_local);
		struct hostent *hp;
		uint16_t client_port;
		char * client_ip;

		n = readLine(s_local, msg_buff, MAX_MSG);

		client_port = (uint16_t) atoi(msg_buff);

		//client_addr_local = (struct sockaddr *) malloc(sizeof(struct sockaddr));

		int bla = getpeername(s_local, (struct sockaddr *) &client_addr_local, &addr_len);
		if (bla == -1) perror("Errooor");
		struct in_addr in;

		hp = gethostbyaddr((char*) &client_addr_local, sizeof(&addr_len), AF_INET);

		char **p;
	
		for (p = hp->h_addr_list; *p != 0; p++){
			memcpy(&in.s_addr, *p, sizeof(in.s_addr));
			printf("List: %s\t%s\n", inet_ntoa(in), hp->h_name);
		}


		//client_port = client_addr_local.sin_port;

		printf("\nPORT NUMBER OF THE CLIENT: %d\n", client_port);

		pthread_mutex_lock(&users_list_mtx);
		out = connectUser(user_buff, client_port);
		pthread_mutex_unlock(&users_list_mtx);
		printf("HEY I'M HERE!!\n");
	}
	else if(strcmp(operation_buff, "DISCONNECT") == 0){
		pthread_mutex_lock(&users_list_mtx);
		out = disconnectUser(user_buff);
		pthread_mutex_unlock(&users_list_mtx);
	}



	write(1, user_buff, m);

	/*
	

	struct sockaddr_in test_addr;

	getpeername(sc, (struct sockaddr *) &test_addr, (socklen_t *) sizeof(test_addr));
	//test_addr.sin_addr is of type struct in_addr
	char * ip = inet_ntoa(test_addr.sin_addr);
	printf("CLIENT ADDRESS: %s\n", ip);
	printf("CLIENT PORT NUMBER: %d\n", client_port);
	*/

	send_msg(s_local, &out, sizeof(out));

	if(close(s_local) == -1){
		perror("[SERVER_THREAD]: Error when closing the socket in the thread");
		exit(-1);
	}
	pthread_exit(NULL);
}

void toUpperCase(char * string){
	/* Convert to uppercase */
	for(int i = 0; string[i]; i++){
  		string[i] = toupper(string[i]);
	}
}