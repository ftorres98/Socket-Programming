/**
 * Name: Fernando Torres
 * Socket Programming Project
 * 
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <string>

using namespace std;

#define SERVERMPORT 26264
#define MAXSIZE 1024
#define localhost "127.0.0.1"

int main(int argc, char *argv[]){

    if(argc != 2){
        fprintf(stderr, "wrong command line arguments\n");
        exit(1);
    }

    cout << "The monitor is up and running." << endl;

    int sockfd, numbytes;
    char buf[MAXSIZE];
    struct sockaddr_in serverM_addr;
    string request = argv[1];
    
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    serverM_addr.sin_family = AF_INET;
    serverM_addr.sin_port = htons(SERVERMPORT);
    serverM_addr.sin_addr.s_addr = inet_addr(localhost);
    memset(&(serverM_addr.sin_zero), '\0', 8);

    if(connect(sockfd, (struct sockaddr *)&serverM_addr, sizeof(struct sockaddr)) == -1){
        perror("connect");
        exit(1);
    }

    if (send(sockfd, &request, request.length(), 0) == -1){
        perror("send");
        exit(1);
    }

    cout << "Monitor sent a sorted list request to the main server." << endl;

    if ((numbytes=recv(sockfd, buf, MAXSIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';

    cout << "Sucessfully received a sorted list of transactions from the main server." <<endl;

    printf("%s\n",buf);
    close(sockfd);
    return 0;
}