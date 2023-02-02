/**
 * Name: Fernando Torres
 * Socket Programming Project
 * 
 */

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
#include <signal.h>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include<cstdlib>
#include<time.h>
#include <map>

using namespace std;

#define MYUDPPORT 24264
#define SERVERAUDPPORT 21264
#define SERVERBUDPPORT 22264
#define SERVERCUDPPORT 23264
#define MYTCPCLIENTPORT 25264
#define MYTCPMONITORPORT 26264
#define BACKLOG 10
#define MAXSIZE 2048
#define localhost "127.0.0.1"

/**
 * Global Variables
 * 
 */
int initialAmount = 1000;
int sockfd_client, new_fd_client, sockfd_monitor, new_fd_monitor, serversfd; 
struct sockaddr_in my_addr_client, my_addr_monitor, my_addr_udp; 
struct sockaddr_in client_addr, monitor_addr, serverA_addr, serverB_addr, serverC_addr; 
socklen_t sin_size;
struct sigaction sa;
int yes=1;
char buf[MAXSIZE];
int numbytes;
int currentSerialNum = 0;
map<int, vector<string> > transactions;


void sigchld_handler(int s){
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

vector<string> convert(string s){
    int size = 0;
    string arg1;
    string arg2;
    string arg3;
    istringstream iss(s);
    vector<string> result;

    if(s.length() != 0){
        iss >> size;

        if(size == 1){
            iss >> arg1;
            result.push_back(arg1);
        }
        if(size == 2){
            iss >> arg1 >> arg2;
            result.push_back(arg1);
            result.push_back(arg2);
        }
        if(size == 3){
            iss >> arg1 >> arg2 >> arg3;
            result.push_back(arg1);
            result.push_back(arg2);
            result.push_back(arg3);
        }
    }
    return result;

}

vector<string> convertTrans(vector<string> v, int serialNum){
    vector<string> result;

    result.push_back(to_string(serialNum));

    for(int i = 0; i < v.size(); i++){
        result.push_back(v[i]);
    }

    return result;
}

string decipher(string s){
    string result;
    for(int i = 0; i < s.length(); i++){
        int lowerB = 0;
        int upperB = 0;
        int num = int(s.at(i));

        if(65 <= num && num <= 90){
            lowerB = 65;
            upperB = 90;
        }
        else if(97 <= num && num <= 122){
            lowerB = 97;
            upperB = 122;
        }
        else if(48 <= num && num <= 57){
            lowerB = 48;
            upperB = 57;
        }
        
        if(lowerB != 0){
            int newNum = num-3;
            if(newNum < lowerB){
                int dist = lowerB - newNum;
                newNum = (upperB +1) - dist;
            }
            result += char(newNum);
        }
        else{
            result += char(num);
        }
    }
    return result;
}

string encoder(string s){
    string result;

    for(int i = 0; i < s.length(); i++){
        int lowerB = 0;
        int upperB = 0;
        int num = int(s.at(i));

        if(65 <= num && num <= 90){
            lowerB = 65;
            upperB = 90;
        }
        else if(97 <= num && num <= 122){
            lowerB = 97;
            upperB = 122;
        }
        else if(48 <= num && num <= 57){
            lowerB = 48;
            upperB = 57;
        }
        
        if(lowerB != 0){
            int newNum = num+3;
            if(newNum > upperB){
                int dist = newNum - upperB;
                newNum = (lowerB-1) + dist;
            }
            result += char(newNum);
        }
        else{
            result += char(num);
        }
    }
    return result;
}

void doSockets(){
    if ((sockfd_client = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("client socket");
        exit(1);
    }

    if (setsockopt(sockfd_client,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        perror("client setsockopt");
        exit(1);
    }

    if ((sockfd_monitor = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("monitor socket");
        exit(1);
    }

    if (setsockopt(sockfd_monitor,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        perror("client setsockopt");
        exit(1);
    }

    if((serversfd = socket(PF_INET, SOCK_DGRAM, 0)) == -1){
        perror("socket");
        exit(1);
    }
}

void setPorts(){
    my_addr_client.sin_family = AF_INET; 
    my_addr_client.sin_port = htons(MYTCPCLIENTPORT); 
    my_addr_client.sin_addr.s_addr = inet_addr(localhost); 
    memset(&(my_addr_client.sin_zero), '\0', 8); 

    my_addr_monitor.sin_family = AF_INET; 
    my_addr_monitor.sin_port = htons(MYTCPMONITORPORT); 
    my_addr_monitor.sin_addr.s_addr = inet_addr(localhost); 
    memset(&(my_addr_monitor.sin_zero), '\0', 8); 

    my_addr_udp.sin_family = AF_INET; 
    my_addr_udp.sin_port = htons(MYUDPPORT); 
    my_addr_udp.sin_addr.s_addr = inet_addr(localhost); 
    memset(&(my_addr_udp.sin_zero), '\0', 8); 

    serverA_addr.sin_family = AF_INET; 
    serverA_addr.sin_port = htons(SERVERAUDPPORT); 
    serverA_addr.sin_addr.s_addr = inet_addr(localhost); 
    memset(&(serverA_addr.sin_zero), '\0', 8); 

    serverB_addr.sin_family = AF_INET; 
    serverB_addr.sin_port = htons(SERVERBUDPPORT); 
    serverB_addr.sin_addr.s_addr = inet_addr(localhost); 
    memset(&(serverB_addr.sin_zero), '\0', 8); 

    serverC_addr.sin_family = AF_INET; 
    serverC_addr.sin_port = htons(SERVERCUDPPORT); 
    serverC_addr.sin_addr.s_addr = inet_addr(localhost); 
    memset(&(serverC_addr.sin_zero), '\0', 8); 
}

void doBind(){
    if (::bind(sockfd_client, (struct sockaddr *)&my_addr_client, sizeof(struct sockaddr)) == -1) {
        perror("client bind");
        exit(1);
    }

    if (::bind(sockfd_monitor, (struct sockaddr *)&my_addr_monitor, sizeof(struct sockaddr)) == -1) {
        perror("monitor bind");
        exit(1);
    }

    if (::bind(serversfd, (struct sockaddr *)&my_addr_udp, sizeof(struct sockaddr)) == -1) {
        perror("udp bind");
        exit(1);
    }
}

void doListen(){
    if (listen(sockfd_client, BACKLOG) == -1) {
        perror("client listen");
        exit(1);
    }

    if (listen(sockfd_monitor, BACKLOG) == -1) {
        perror("monitor listen");
        exit(1);
    }
}

void sendToServers(string s){
    if(sendto(serversfd, &s, s.length(), 0, (struct sockaddr *)&serverA_addr, sizeof(struct sockaddr)) == -1){
        perror("sendto");
        exit(1);
    }

    if(sendto(serversfd, &s, s.length(), 0, (struct sockaddr *)&serverB_addr, sizeof(struct sockaddr)) == -1){
        perror("sendto");
        exit(1);
    }
    

    if(sendto(serversfd, &s, s.length(), 0, (struct sockaddr *)&serverC_addr, sizeof(struct sockaddr)) == -1){
        perror("sendto");
        exit(1);
    }
}

void receiveFromServers(char *buf1, char *buf2, char *buf3){
    socklen_t addr_len = sizeof(struct sockaddr);
    if((numbytes = recvfrom(serversfd, buf1, sizeof(buf1), 0, (struct sockaddr *)&my_addr_udp, &addr_len))== -1){
        perror("recvfrom");
        exit(1);
    }
    buf1[numbytes] = '\0';

    if((numbytes = recvfrom(serversfd, buf2, sizeof(buf2), 0, (struct sockaddr *)&my_addr_udp, &addr_len))== -1){
        perror("recvfrom");
        exit(1);
    }
    buf2[numbytes] = '\0';

    if((numbytes = recvfrom(serversfd, buf3, sizeof(buf3), 0, (struct sockaddr *)&my_addr_udp, &addr_len))== -1){
        perror("recvfrom");
        exit(1);
    }
    buf3[numbytes] = '\0';
}

int getBalance(vector<string> v1, vector<string> v2, vector<string> v3){
    int total = 0;
    int recv = 0;
    int sent = 0;

    if(v1.size() != 0){
        recv += stoi(v1[0]);
        sent += stoi(v1[1]);
    }

    if(v2.size() != 0){
        recv += stoi(v2[0]);
        sent += stoi(v2[1]);
    }

    if(v3.size() != 0){
        recv += stoi(v3[0]);
        sent += stoi(v3[1]);
    }
    total = initialAmount + recv - sent;
    return total;
}

void findCurrentSerialNum(){
    int max = 0;
    char buf1[MAXSIZE], buf2[MAXSIZE], buf3[MAXSIZE];
    sendToServers("0");
    receiveFromServers((char*)&buf1, (char*)&buf2, (char*)&buf3);

    int num1 = stoi(buf1);
    int num2 = stoi(buf2);
    int num3 = stoi(buf3);

    if(num1>num2){
        if(num1>num3){
            currentSerialNum = num1;
        }
    }
    else if(num2>num1){
        if(num2>num3){
            currentSerialNum = num2;
        }
    }
    else if(num3> num2){
        if(num3>num1){
            currentSerialNum = num3;
        }
    }
}

void clientCode(){
    char buf1[MAXSIZE], buf2[MAXSIZE], buf3[MAXSIZE];
    bool find1 = true;
    bool find2 = true;
    int bal = 0;

    if ((numbytes=recv(new_fd_client, buf, MAXSIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';

    vector<string> clientInput = convert(buf);

    if(clientInput.size() == 1){
        cout << "The main server received input= " << clientInput[0] 
            << " from the client using TCP over port " << MYTCPCLIENTPORT << endl;
    }
    else{
        cout << "The main server received from " << clientInput[0] << " to transfer " 
            << clientInput[2] << " coins to " << clientInput[1] << " using TCP over port " 
            << MYTCPCLIENTPORT << endl;
    }

    if(clientInput.size() == 1){
        sendToServers(buf);
        cout << "The main server sent a request to server A" << endl;
        cout << "The main server sent a request to server B" << endl;
        cout << "The main server sent a request to server C" << endl;

        receiveFromServers((char*)&buf1, (char*)&buf2, (char*)&buf3);
        cout << "The main server received transactions from Server A using UDP over port " << MYUDPPORT << "." << endl;
        cout << "The main server received transactions from Server B using UDP over port " << MYUDPPORT << "." << endl;
        cout << "The main server received transactions from Server C using UDP over port " << MYUDPPORT << "." << endl;

        vector<string> serverAOutput = convert(buf1);
        vector<string> serverBOutput = convert(buf2);
        vector<string> serverCOutput = convert(buf3);

        string message = to_string(getBalance(serverAOutput, serverBOutput, serverCOutput));
        if (send(new_fd_client, &message, message.length(), 0) == -1){
            perror("send");
        }

        cout << "The main server sent the current balance to the client." << endl;
    } else {
        string s1 = to_string(2) + " " + clientInput[0];
        sendToServers(s1);
        cout << "The main server sent a request to server A" << endl;
        cout << "The main server sent a request to server B" << endl;
        cout << "The main server sent a request to server C" << endl;

        receiveFromServers((char*)&buf1, (char*)&buf2, (char*)&buf3);
        cout << "The main server received the feedback from Server A using UDP over port " << MYUDPPORT << "." << endl;
        cout << "The main server received the feedback from Server B using UDP over port " << MYUDPPORT << "." << endl;
        cout << "The main server received the feedback from Server C using UDP over port " << MYUDPPORT << "." << endl;

        if(buf1[0] == '0' && buf2[0] == '0' && buf3[0] == '0'){
            find1 = false;
        }else{
            vector<string> serverAOutput = convert(buf1);
            vector<string> serverBOutput = convert(buf2);
            vector<string> serverCOutput = convert(buf3);
            bal = getBalance(serverAOutput, serverBOutput, serverCOutput);
        }

        s1 = to_string(2) + " " + clientInput[1];
        sendToServers(s1);

        receiveFromServers((char*)&buf1, (char*)&buf2, (char*)&buf3);

        if(buf1[0] == '0' && buf2[0] == '0' && buf3[0] == '0'){
            find2 = false;
        }
        
        if(find1 == false && find2 == true){
            string message = to_string(21);
            if (send(new_fd_client, &message, message.length(), 0) == -1){
                perror("send");
                exit(1);
            }

        }
        else if(find1 == true && find2 == false){
            string message = to_string(22);
            if (send(new_fd_client, &message, message.length(), 0) == -1){
                perror("send");
                exit(1);
            }

        }
        else if(find1 == false && find2 == false){
            string message = to_string(3);
            if (send(new_fd_client, &message, message.length(), 0) == -1){
                perror("send");
                exit(1);
            }

        }
        else if(stoi(clientInput[2])>bal){
            string message = to_string(1);
            if (send(new_fd_client, &message, message.length(), 0) == -1){
                perror("send");
                exit(1);
            }
        }
        else{
            srand(time(0));
            int ranNum = (rand()%3) + 1;
            currentSerialNum++;
            clientInput.push_back(to_string(currentSerialNum));

            string message = to_string(clientInput.size()) + " " + clientInput[3] + " " + clientInput[0] + " " + clientInput[1] + " " + clientInput[2];

            if(ranNum == 1){
                if(sendto(serversfd, &message, message.length(), 0, (struct sockaddr *)&serverA_addr, sizeof(struct sockaddr)) == -1){
                    perror("sendto");
                    exit(1);
                }
            }
            else if(ranNum == 2){
                if(sendto(serversfd, &message, message.length(), 0, (struct sockaddr *)&serverB_addr, sizeof(struct sockaddr)) == -1){
                    perror("sendto");
                    exit(1);
                }
            }else{
                if(sendto(serversfd, &message, message.length(), 0, (struct sockaddr *)&serverC_addr, sizeof(struct sockaddr)) == -1){
                    perror("sendto");
                    exit(1);
                }
            }

            string getBalOf = to_string(2) + " " + clientInput[0];
            //cout << getBalOf << endl;
            sendToServers(getBalOf);
            receiveFromServers((char*)&buf1, (char*)&buf2, (char*)&buf3);

            vector<string> serverAOutput = convert(buf1);
            vector<string> serverBOutput = convert(buf2);
            vector<string> serverCOutput = convert(buf3);

            message = to_string(0) + " " + to_string(getBalance(serverAOutput, serverBOutput, serverCOutput));
            if (send(new_fd_client, &message, message.length(), 0) == -1){
                perror("send");
                exit(1);
            }
        }
        cout << "The main server sent the result of the transaction to the client." << endl;
    }
}

void monitorCode(){
    if ((numbytes=recv(new_fd_monitor, buf, MAXSIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    buf[numbytes] = '\0';

    if(send(new_fd_monitor, "Hello, world!\n", 14, 0) == -1){
        perror("send");
        exit(1);
    }

    cout << "The main server received a sorted list request from the monitor using TCP over port " << MYTCPMONITORPORT << ".";
}

int main(int argc, char *argv[]){

    doSockets();

    setPorts();

    doBind();

    doListen();

    sa.sa_handler = sigchld_handler; 
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    cout << "The main server is up and running." <<endl;

    while(1) { 
        sin_size = sizeof(struct sockaddr_in);

        if ((new_fd_client = accept(sockfd_client, (struct sockaddr *)&client_addr,&sin_size)) == -1) {
            perror("client accept");
            continue;
        }

        findCurrentSerialNum();

        clientCode();
        close(new_fd_client); 

        if ((new_fd_client = accept(sockfd_client, (struct sockaddr *)&client_addr,&sin_size)) == -1) {
            perror("client accept");
            continue;
        }
        clientCode();
        close(new_fd_client); 

        if ((new_fd_monitor = accept(sockfd_monitor, (struct sockaddr *)&monitor_addr,&sin_size)) == -1) {
            perror("monitor accept");
            continue;
        }
        monitorCode();
        close(new_fd_monitor); 

    }    

    return 0;
}