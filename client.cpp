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

#define SERVERMPORT 25264
#define MAXSIZE 1024
#define localhost "127.0.0.1"

int main(int argc, char *argv[]){

    if(argc != 2 && argc != 4){
        fprintf(stderr, "wrong command line arguments\n");
        exit(1);
    }

    cout << "The client is up and running." << endl;

    int sockfd, numbytes;
    char buf[MAXSIZE];
    struct sockaddr_in serverM_addr;
    string request = to_string(argc-1) + " ";

    for(int i = 1; i< argc; i++){
        request = request  + argv[i] + " ";
    }
    
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

    if(argc == 2){
        cout << argv[1] << " sent a balance enquiry request to the main server." << endl;
    }else{
        cout << argv[1] << " has requested to transfer " << argv[3] << " txcoins to " << argv[2] << "." << endl;
    }

    if ((numbytes=recv(sockfd, buf, MAXSIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';

    if(argc == 2){
        cout << "The current balance of " << argv[1] << " is: " << buf << " txcoins." << endl;
    }else{
        // 0 - successful, 1 - insufficient, 2 (1||2) - 1 of clients, 3 - both clients
        if(buf[0] == '1'){
            cout<< argv[1] << " was unable to transfer " << argv[3] << " txcoins to " << argv[2] << " because of insufficient balance." << endl;
        } 
        else if(buf[0] == '2'){
            if(buf[1] == '1'){
                cout << "Unable to proceed with the transaction as " << argv[1] << " is not part of the network." << endl;
            }else{
                cout << "Unable to proceed with the transaction as " << argv[2] << " is not part of the network." << endl;
            }
        }
        else if(buf[0] == '3'){
            cout << "Unable to proceed with the transaction as " << argv[1] << " and " << argv[2] << " are not part of the network." << endl;
        }else{
            cout << argv[1] << " successfully transfered " << argv[3] << " txcoins to " << argv[2] << "." << endl;

            string message = buf;
            string balance = message.substr(2);
            int bal = stoi(balance);
            cout << "The current balance of " << argv[1] << " is: " << bal << " txcoins." << endl;
        }
    }
    close(sockfd);

    return 0;
}