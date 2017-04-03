#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "read_line.h"
#include "user_list.h"
#include "msg_list.h"


int main(void){
    char line[MAX_MSG];
    char ip [1];
    //ip = "DISCONNECT";
    strcpy(ip, "DISCONNECT");
    printf("String: %s\n", ip);
    printf("Size: %d\n", sizeof(ip));

    //struct user * user_head = (struct user *) malloc(sizeof(struct user));
    while(1){
        printf("USER LISTS. CHOOSE ACTION:\n");
        printf("\t1: Register\n");
        printf("\t2: Unegister\n");
        printf("\t3: Connect\n");
        printf("\t4: Disconnect\n");
        printf("\t5: Send message\n");
        printf("\t5: Delete message\n");
        readLine(0, line, MAX_MSG);
        if(strcmp(line, "1") == 0){
        	printf("Username: \n");
            readLine(0, line, MAX_MSG);
                int res = registerUser(line);
                if(!res){
                    printf("USER ADDED\n");
                    printUsers();
                }
                else{
                    printf("ERROR: ALREADY EXISTS!\n");
                    printUsers();
                }
            
        }
        else if(strcmp(line, "2") == 0){
            readLine(0, line, MAX_MSG);
                int res = unregisterUser(line);
                if(!res){
                    printf("USER DELETED\n");
                    printUsers();
                }
                else{
                    printf("ERROR: DOES NOT EXIST!\n");
                    printUsers();
                }
        }
        else if(strcmp(line, "5") == 0){
        	readLine(0, line, MAX_MSG);
        	char user[MAX_MSG];
        	strcpy(user, line);
        	readLine(0, line, MAX_MSG);
        	char msg[MAX_MSG];
        	strcpy(msg, line);
        	printf("Going to send to %s the msg %s", user, msg);
        	appendMsg(user, msg);
        	printPendMsgs(user);
        }
        else if(strcmp(line, "6") == 0){
            readLine(0, line, MAX_MSG);
            char user[MAX_MSG];
            strcpy(user, line);
            removePendMsg(user);
            printPendMsgs(user);
        }
    }

    return 0;
}