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
    if(pthread_mutex_init(&list_mtx, NULL) != 0) {
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
		pthread_mutex_lock(&list_mtx);
		out = registerUser(user_buff);
		pthread_mutex_unlock(&list_mtx);

	}
	else if (strcmp(operation_buff, "UNREGISTER") == 0){
		/* Unregister the user */
		pthread_mutex_lock(&list_mtx);
		out = unregisterUser(user_buff);
		pthread_mutex_unlock(&list_mtx);

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

		pthread_mutex_lock(&list_mtx);
		out = connectUser(user_buff, inet_ntoa(client_addr_local.sin_addr), client_port);
		pthread_mutex_unlock(&list_mtx);

		/* If result is 0, then heck for the pending messages and send them */
		if(out == 0){
			/* Send Pending Messages */
			pthread_mutex_lock(&list_mtx);
			struct msg **pend_msg = getPendMsgHead(user_buff);
			while(*pend_msg != NULL){
				pthread_mutex_unlock(&list_mtx);
				char sender[MAX_USERNAME];
				char msg_body[MAX_MSG];

				pthread_mutex_lock(&list_mtx);
				strcpy(sender, (*pend_msg)->sender);
				int msg_id = (*pend_msg)->id;
				strcpy(msg_body, (*pend_msg)->body);
				pthread_mutex_unlock(&list_mtx);

				/* Try to send the message. The 'stored' flag is set to 1 because the message
				is already stored in the server */
				int err = sendMessage(sender, user_buff, msg_body, msg_id, 1);

				/* If the message could not be delivered, then exit the loop */
				if(err == 1) break;
				sendAck(sender, msg_id);

				/* Remove the message from the pending message queue and iterate with the next message */
				pthread_mutex_lock(&list_mtx);
				*pend_msg = dequeueMsg(&(*pend_msg));
			}
			pthread_mutex_unlock(&list_mtx);
		}
	
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

		pthread_mutex_lock(&list_mtx);
		out = disconnectUser(user_buff, inet_ntoa(client_addr_local.sin_addr));
		pthread_mutex_unlock(&list_mtx);
	}
	else if(strcmp(operation_buff, "SEND") == 0){
		char dest_user_buff[MAX_USERNAME];
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
		pthread_mutex_lock(&list_mtx);
		if(!isRegistered(user_buff) || !isRegistered(dest_user_buff)){
			pthread_mutex_unlock(&list_mtx);
			out = 1; //Send code 1 to the client
			goto respond_to_client;
		}
		pthread_mutex_unlock(&list_mtx);

		/* Check the status of the destination user */
		pthread_mutex_lock(&list_mtx);
		char status = isConnected(dest_user_buff);
		unsigned int last_id = updateLastID(user_buff); //Update the last id of the sender message
		pthread_mutex_unlock(&list_mtx);

		if(status == 0){ //Not connected
			/* Store the message */
			if (storeMessage(user_buff, dest_user_buff, msg_buff, last_id) != 0){
				out = 2;
				goto respond_to_client;
			}
			/* Message was stored successfully, send code 0 and message ID to the client */
			out = 0;
			send_msg(s_local, &out, sizeof(out));

			char id_string[11];
			sprintf(id_string, "%d", last_id);
			send_msg(s_local, id_string, strlen(id_string)+1);

		}else if(status == 1){
			/* Try to send the message to the receiver. We set the 'stored' flag to 0 because the message
			is being sent for the first time and was not previously stored int he server */
			int err = sendMessage(user_buff, dest_user_buff, msg_buff, last_id, 0);

			/* If any server error occurred and the message was not stored or sent, then send code 2
			back to the client */
			if(err == -1){
				out = 2;
				goto respond_to_client;
			}

			/* If no server error occured, then the message was either sent or stored, so we send back
			the code 0 (OK) to the client */
			out = 0;
			send_msg(s_local, &out, sizeof(out));

			char id_string[11];
			sprintf(id_string, "%d", last_id);
			send_msg(s_local, id_string, strlen(id_string)+1);

			/* If the message was stored in the  */
			if(err == 0) sendAck(user_buff, last_id);
		}

		goto destroy_thread; 
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
		if(close(s_local) == -1){
			perror("[SERVER_THREAD]: Error when closing the socket in the thread");
			exit(-1);
		}
		printf("Destroying thread...!\n");
		pthread_exit(NULL);
}

void toUpperCase(char * string){
	/* Convert to uppercase */
	for(int i = 0; string[i]; i++){
  		string[i] = toupper(string[i]);
	}
}

/* Return 0: Message is stored OK
   Return -1: Server error (Memory space error) */
int storeMessage(char * sender, char * receiver, char * msg, unsigned int msg_id){
	pthread_mutex_lock(&list_mtx);
	printf("Storing the message...\n");
	int err = storeMsg(receiver, msg, msg_id, sender);
	printf("After storing: %d\n", err);
	pthread_mutex_unlock(&list_mtx);

	if(err == -1) return -1;

	fprintf(stderr, "MESSAGE %d FROM %s TO %s STORED", msg_id,
									 sender, receiver);
	fprintf(stderr, "\n%s", "s> ");	/* Prompt */

	return 0;
}

/* Return 0: Message is sent OK
   Return 1: Message is stored, or not stored if was already stored 
   Return -1: Server error */
int sendMessage(char * sender, char * receiver, char * msg, unsigned int msg_id, char stored){
	int s_receiver; //Socket for the receiver of the message
	struct sockaddr_in recv_addr; //Receiver address
	struct hostent *recv_hp; //Host entity structure for the receiver

	s_receiver = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(s_receiver == -1){
		return -1; //Send error message. -1 is internally encoded as server error
	}

	bzero((char *) &recv_addr, sizeof(recv_addr)); //Reserve space for the address of the receiver

	pthread_mutex_lock(&list_mtx);
	recv_hp = gethostbyname(getUserIP(receiver)); //Get the IP of the receiver
	pthread_mutex_unlock(&list_mtx);

	memcpy(&(recv_addr.sin_addr), recv_hp->h_addr, recv_hp->h_length); //Get the IP addres in network format
	recv_addr.sin_family = AF_INET;
	pthread_mutex_lock(&list_mtx);
	recv_addr.sin_port = htons(getUserPort(receiver)); //Get the port number of the receiver listening thread
	pthread_mutex_unlock(&list_mtx);

	if (connect(s_receiver, (struct sockaddr *) &recv_addr, sizeof(recv_addr)) == -1){
		/* If the connection with the receiver fails, assume the client 
		to be disconnected, disconnect it and store the message */
		pthread_mutex_lock(&list_mtx);
		disconnectUser(receiver, getUserIP(receiver)); // No need to check for output
		pthread_mutex_unlock(&list_mtx);
		/* If the stored parameter is set to 0, it means that the message was not prevoiusly stored by the
		server so we need to push it to the end of the queue. If it was stored, then nothing is done */
		if(!stored){
			if(storeMessage(sender, receiver, msg, msg_id) == -1) return -1; //Return -1 if store error
		}
		close(s_receiver); //Close the socket
		return 1;
	}
	/* Send the SEND_MESSAGE string to the receiver to detect an incoming message */
	char op[13];
	strcpy(op, "SEND_MESSAGE");
	send_msg(s_receiver, op, strlen(op)+1);
	/* Send the sender name */
	send_msg(s_receiver, sender, strlen(sender)+1);
	/* Send the identifier of the message */
	char id_string[11];
	sprintf(id_string, "%d", msg_id);
	send_msg(s_receiver, id_string, strlen(id_string)+1);
	/* Send the message */
	send_msg(s_receiver, msg, strlen(msg)+1);

	close(s_receiver); //Close the socket

	fprintf(stderr, "SEND MESSAGE %d FROM %s TO %s", msg_id,
										 sender, receiver);
	fprintf(stderr, "\n%s", "s> ");	/* Prompt */
	return 0;
}

/* Return 0: Message is sent OK
   Return 1: Message is stored, or not stored if was already stored 
   Return -1: Server error */
int sendAck(char * sender, unsigned int msg_id){
	int s_sender; //Socket for the receiver of the message
	struct sockaddr_in sender_addr; //Sender address
	struct hostent *sender_hp; //Host entity structure for the sender

	s_sender = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(s_sender == -1){
		return -1; //Send error message. -1 is internally encoded as server error
	}

	char ack_op[14];
	strcpy(ack_op, "SEND_MESS_ACK");

	bzero((char *) &sender_addr, sizeof(sender_addr));

	pthread_mutex_lock(&list_mtx);
	sender_hp = gethostbyname(getUserIP(sender));
	pthread_mutex_unlock(&list_mtx);

	memcpy(&(sender_addr.sin_addr), sender_hp->h_addr, sender_hp->h_length);
	sender_addr.sin_family = AF_INET;

	pthread_mutex_lock(&list_mtx);
	sender_addr.sin_port = htons(getUserPort(sender));
	pthread_mutex_unlock(&list_mtx);

	connect(s_sender, (struct sockaddr *) &sender_addr, sizeof(sender_addr));


	char id_string[11];
	sprintf(id_string, "%d", msg_id);
	send_msg(s_sender, ack_op, strlen(ack_op)+1);
	send_msg(s_sender, id_string, strlen(id_string)+1);

	close(s_sender);
	return 0;
}