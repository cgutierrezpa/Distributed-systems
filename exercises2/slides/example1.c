#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv){
	struct hostent *hp;
	struct in_addr in;

	hp = gethostbyname(argv[1]);
	if(hp == NULL){
		printf("Error in gesthostbyname\n");
		exit(0);
	}
	memcpy(&in.s_addr, *(hp->h_addr_list), sizeof(in.s_addr));

	printf("%s is %s\n", hp->h_name, inet_ntoa(in));
	exit(1);
}