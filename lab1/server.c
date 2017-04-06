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
	thread = (pthread_t) malloc((sizeof(thread)));
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
	server_addr.sin_addr.s_addr		= htons(INADDR_ANY);	/* Listens to all addresses */
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
		/*
	char servername[16];
	inet_ntop(AF_INET, &server_addr.sin_addr, servername, sizeof(servername));
	printf("Client Adress = %s\n", servername);
	*/
	printf("s> init server %s:%d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));

	if(listen(sd, 5) == -1){
		perror("Error when listening to the socket");
		exit(-1);
	} /* Backlog is 5, maximum number of queued requests is 5 */

	socklen_t size = sizeof(client_addr);

	busy_socket = TRUE;
	
	signal(SIGINT, interruptHandler); /* Handles the ctrl+c signal to interrupt the server */
	fprintf(stderr, "%s", "s> ");	/* Prompt */
	while(1){

		sc = accept(sd, (struct sockaddr *) &client_addr, &size);
		if(sc == -1){
			perror("Error when accepting the connection");
			exit(-1);
		}
		
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

	}

	close(sd);
	exit(0);

}

void * manageRequest(int *sd){
	int s_local;
	char operation_buff[MAX_COMMAND];
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
	n = readLine(s_local, operation_buff, MAX_COMMAND);
	if(n == -1){
		perror("[SERVER_THREAD]: Error when reading from the socket");
		close(s_local);
		//close socket in father
		exit(-1);
	}
	/* Read the username and convert to uppercase */
	m = readLine(s_local, user_buff, MAX_USERNAME);
	if(m == -1){
		perror("[SERVER_THREAD]: Error when reading from the socket\n");
		close(s_local);
		//close socket in father
		exit(-1);
	}
	toUpperCase(user_buff);

	/* Check the operation */
	if (strcmp(operation_buff, "REGISTER") == 0){
		/* Register the user */
		pthread_mutex_lock(&users_list_mtx);
		out = registerUser(user_buff);
		pthread_mutex_unlock(&users_list_mtx);

	}
	else if (strcmp(operation_buff, "UNREGISTER") == 0){
		/* Unregister the user */
		pthread_mutex_lock(&users_list_mtx);
		out = unregisterUser(user_buff);
		pthread_mutex_unlock(&users_list_mtx);

	}
	else if(strcmp(operation_buff, "CONNECT") == 0){

		struct sockaddr_in client_addr_local;
		socklen_t addr_len = sizeof(client_addr_local);
		uint16_t client_port;

		n = readLine(s_local, msg_buff, MAX_MSG);
		if(n == -1){
			perror("[SERVER_THREAD]: Error when reading from the socket");
			close(s_local);
			//Close the father socket
		}

		client_port = (uint16_t) atoi(msg_buff);

		//client_addr_local = (struct sockaddr *) malloc(sizeof(struct sockaddr));

		int err = getpeername(s_local, (struct sockaddr *) &client_addr_local, &addr_len);
		if (err == -1){
			perror("Error when getting client address");
			out = 3;
			goto respond_to_client;
			//Send error 2 to client and close socket
		}

		printf("\nIP OF THE CLIENT: %s", inet_ntoa(client_addr_local.sin_addr));
		printf("\nPORT NUMBER OF THE CLIENT: %d\n", client_port);

		pthread_mutex_lock(&users_list_mtx);
		out = connectUser(user_buff, inet_ntoa(client_addr_local.sin_addr), client_port);
		pthread_mutex_unlock(&users_list_mtx);
	
	}
	else if(strcmp(operation_buff, "DISCONNECT") == 0){
		struct sockaddr_in client_addr_local;
		socklen_t addr_len = sizeof(client_addr_local);

		int err = getpeername(s_local, (struct sockaddr *) &client_addr_local, &addr_len);
		if (err == -1){
			perror("Error when getting client address");
			out = 3;
			goto respond_to_client;
			//Send error 2 to client and close socket
		}

		pthread_mutex_lock(&users_list_mtx);
		out = disconnectUser(user_buff, inet_ntoa(client_addr_local.sin_addr));
		pthread_mutex_unlock(&users_list_mtx);
	}
	else if(strcmp(operation_buff, "SEND") == 0){
		char dest_user_buff[MAX_USERNAME];
		//int client_socket;
		printf("Receive sender: %s\n", user_buff);

		m = readLine(s_local, dest_user_buff, MAX_USERNAME);
		if(m == -1){
			perror("[SERVER_THREAD]: Error when reading from the socket\n");
			close(s_local);
			//close socket in father
			exit(-1);
		}
		toUpperCase(dest_user_buff);
		printf("Receive receiver: %s\n", dest_user_buff);

		n = readLine(s_local, msg_buff, MAX_MSG);
		if(m == -1){
			perror("[SERVER_THREAD]: Error when reading from the socket\n");
			close(s_local);
			//close socket in father
			exit(-1);
		}
		printf("Receive message: %s\n", msg_buff);
		
		/* Check if one of the two users is not registered */
		pthread_mutex_lock(&users_list_mtx);
		if(!isRegistered(user_buff) || !isRegistered(dest_user_buff)){
			pthread_mutex_unlock(&users_list_mtx);
			out = 1; //Send code 1 to the client
			goto respond_to_client;
		}
		pthread_mutex_unlock(&users_list_mtx);

		/* Check the status of the destination user */
		pthread_mutex_lock(&users_list_mtx);
		char status = isConnected(dest_user_buff);
		unsigned int last_id = updateLastID(user_buff); //Update the last id of the sender message
		pthread_mutex_unlock(&users_list_mtx);
		printf("Last ID is: %d\n", last_id);
		printf("Status is: %d\n", status);
		if(status == 0){ //Not connected
				/* Store the message */
				pthread_mutex_lock(&users_list_mtx);
				storeMsg(dest_user_buff, msg_buff, last_id, user_buff);
				pthread_mutex_unlock(&users_list_mtx);
				
				fprintf(stderr, "MESSAGE %d FROM %s TO %s STORED", last_id,
													 user_buff, dest_user_buff);
				fprintf(stderr, "\n%s", "s> ");	/* Prompt */

				out = 0;
				send_msg(s_local, &out, sizeof(out));

				char id_string[11];
				sprintf(id_string, "%d", last_id);
				send_msg(s_local, id_string, sizeof(id_string));

				goto destroy_thread;
		}else if(status == 1){
				/* Open 2 connections. One for the destination user to send the message
				and another for the origin user to acknowledge that the message has been
				sent successfully */
				int s_origin_client, s_dest_client;
				struct sockaddr_in origin_addr, dest_addr;
				struct hostent *origin_hp, *dest_hp;

				s_dest_client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				if(s_dest_client == -1){
					out = 2;
					send_msg(s_local, &out, sizeof(out));
					goto destroy_thread;
				}

				bzero((char *) &dest_addr, sizeof(dest_addr));

				pthread_mutex_lock(&users_list_mtx);
				dest_hp = gethostbyname(getUserIP(dest_user_buff));
				pthread_mutex_unlock(&users_list_mtx);

				memcpy(&(dest_addr.sin_addr), dest_hp->h_addr, dest_hp->h_length);
				dest_addr.sin_family = AF_INET;
				pthread_mutex_lock(&users_list_mtx);
				dest_addr.sin_port = htons(getUserPort(dest_user_buff));
				pthread_mutex_unlock(&users_list_mtx);

				if (connect(s_dest_client, (struct sockaddr *) &dest_addr, sizeof(dest_addr)) == -1){
					/* If the connection with the receiver fails, assume the client 
					to be disconnected, disconnect it and store the message */
					pthread_mutex_lock(&users_list_mtx);
					disconnectUser(dest_user_buff, getUserIP(dest_user_buff)); // No need to check for output
					storeMsg(dest_user_buff, msg_buff, last_id, user_buff);
					pthread_mutex_unlock(&users_list_mtx);

					fprintf(stderr, "MESSAGE %d FROM %s TO %s STORED", last_id,
													 user_buff, dest_user_buff);
					fprintf(stderr, "\n%s", "s> ");	/* Prompt */

					out = 0;
					send_msg(s_local, &out, sizeof(out));

					char id_string[11];
					sprintf(id_string, "%d", last_id);
					send_msg(s_local, id_string, sizeof(id_string));
					goto destroy_thread;
				}

				char op[13];
				strcpy(op, "SEND_MESSAGE");

				send_msg(s_dest_client, op, sizeof(op));
				send_msg(s_dest_client, user_buff, MAX_USERNAME);
				/* Send the identifier of the message */
				char id_string[11];
				sprintf(id_string, "%d", last_id);
				printf("ID STRING AFTER CONVERSION IS: %s\n", id_string);
				send_msg(s_dest_client, id_string, sizeof(id_string));
				send_msg(s_dest_client, msg_buff, MAX_MSG);

				close(s_dest_client);

				/*IF NO ERRORS, SEND ACK BACK TO SENDER*/
				fprintf(stderr, "SEND MESSAGE %d FROM %s TO %s", last_id,
													 user_buff, dest_user_buff);
				fprintf(stderr, "\n%s", "s> ");	/* Prompt */

				out = 0;
				send_msg(s_local, &out, sizeof(out));

				//sprintf(id_string, "%d", last_id);
				send_msg(s_local, id_string, sizeof(id_string));

				s_origin_client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				//CHECK FOR ERROR AND SEND ERROR TO CLIENT

				char ack_op[17];
				strcpy(ack_op, "SEND_MESS_ACK");

				bzero((char *) &origin_addr, sizeof(origin_addr));

				pthread_mutex_lock(&users_list_mtx);
				origin_hp = gethostbyname(getUserIP(user_buff));
				pthread_mutex_unlock(&users_list_mtx);
				memcpy(&(origin_addr.sin_addr), origin_hp->h_addr, origin_hp->h_length);
				origin_addr.sin_family = AF_INET;
				pthread_mutex_lock(&users_list_mtx);
				origin_addr.sin_port = htons(getUserPort(user_buff));
				pthread_mutex_unlock(&users_list_mtx);

				connect(s_origin_client, (struct sockaddr *) &origin_addr, sizeof(origin_addr));
				int s = send_msg(s_origin_client, ack_op, sizeof(ack_op));
				printf("Number of bytes sent ack_op: %d\n", s);
				s = send_msg(s_origin_client, id_string, sizeof(id_string));
				printf("Number of bytes sent id_string: %d\n", s);

				close(s_origin_client);
				goto destroy_thread;
		}

		char prueba[11];
		strcpy(prueba,"4294967295");
		out = 0;
		send_msg(s_local, &out, sizeof(out));
		send_msg(s_local, prueba, sizeof(prueba));

		/* As the print is different format is different from the other commands
		it is performed in this section and the default print is skipped */
		switch(out){
			case 0:
				//OK
				break;
			default:
				//FAIL
				break;
		}

		goto destroy_thread; //Skips the default print below
	}

	/* Default print */
	switch(out){
		case 0:
			fprintf(stderr, "%s %s %s", operation_buff, user_buff, "OK");
			fprintf(stderr, "\n%s", "s> ");	/* Prompt */
			break;
		default:
			fprintf(stderr, "%s %s %s", operation_buff, user_buff, "FAIL");
			fprintf(stderr, "\n%s", "s> ");	/* Prompt */
	}

	//write(1, user_buff, m);


	/*
	

	struct sockaddr_in test_addr;

	getpeername(sc, (struct sockaddr *) &test_addr, (socklen_t *) sizeof(test_addr));
	//test_addr.sin_addr is of type struct in_addr
	char * ip = inet_ntoa(test_addr.sin_addr);
	printf("CLIENT ADDRESS: %s\n", ip);
	printf("CLIENT PORT NUMBER: %d\n", client_port);
	*/

	respond_to_client: 
		send_msg(s_local, &out, sizeof(out));

	destroy_thread:
	printf("COMPROBANDO CHECKPOINT 4\n");
	if(close(s_local) == -1){
		perror("[SERVER_THREAD]: Error when closing the socket in the thread");
		exit(-1);
	}
	printf("COMPROBANDO CHECKPOINT 1\n");
	printf("Destroying thread...!\n");
	pthread_exit(NULL);
}

void toUpperCase(char * string){
	/* Convert to uppercase */
	for(int i = 0; string[i]; i++){
  		string[i] = toupper(string[i]);
	}
}