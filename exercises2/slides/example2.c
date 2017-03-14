#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, const char** argv){
	//u_int addr;
	char** p;
	char ** q;
	struct hostent *hp;
	struct in_addr in;

	if(argc != 2){
		printf("Use: %s <IP address>\n", argv[0]);
		exit(1);
	}

	int err = inet_aton(argv[1], &in);
	if(err == 0){
		printf("IP address in format a.b.c.d\n");
		exit(2);
	}

	hp = gethostbyaddr((char*) &in, sizeof(in), AF_INET);

	if(hp == NULL){
		printf("Error in gethostbyaddr\n");
		exit(3);
	}

	for (p = hp->h_addr_list; *p != 0; p++){
		memcpy(&in.s_addr, *p, sizeof(in.s_addr));
		printf("%s\t%s\n", inet_ntoa(in), hp->h_name);
	}

	for (q = hp->h_aliases; *q != 0; q++){
		printf("%s\n", *q);
	}

	exit(1);
}