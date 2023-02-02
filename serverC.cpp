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
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <vector>

using namespace std;

#define MYUDPPORT 23264
#define SERVERMPORT 24264
#define localhost "127.0.0.1"
#define MAXSIZE 2048

/**
 * Global variables
 * 
 */
map<int, vector<string> > transactions;
unordered_map<string, int > received;
unordered_map<string, int > sent;

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

void readIn(){
    fstream block1File("block3.txt");
    string line;
    int serialNum;
    string name1;
    string name2;
    string amount;

    if(block1File.is_open()){
        while (getline(block1File, line))
        {
            if(line.length() != 0){
                istringstream iss(line);
                vector<string> temp;
                iss >> serialNum >> name1 >> name2 >> amount;

                temp.push_back(name1);
                temp.push_back(name2);
                temp.push_back(amount);

                sent[name1]+=stoi(decipher(amount));
                received[name2]+=stoi(decipher(amount));
                transactions[serialNum] = temp;
            }
        }
        
    }
}

vector<string> convert(string s){
    int size = 0;
    string arg1;
    string arg2;
    string arg3;
    istringstream iss(s);
    vector<string> result;

    iss >> size;

    if(size == 1 || size == 2){
        iss >> arg1;
        result.push_back(arg1);
    }
    if(size == 3){
        iss >> arg2 >> arg3;
        result.push_back(arg2);
        result.push_back(arg3);
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

void store(string buf){
    int size;
    int serialNum;
    string name1;
    string name2;
    string amount;
    istringstream iss(buf);
    vector<string> temp;
    iss >> size >> serialNum >> name1 >> name2 >> amount;

    temp.push_back(name1);
    temp.push_back(name2);
    temp.push_back(amount);

    sent[encoder(name1)]+=stoi(amount);
    received[encoder(name2)]+=stoi(amount);
    transactions[serialNum] = temp;
}

int main(int argc, char *argv[]){
    readIn();

    int sockfd;
    struct sockaddr_in serverM_addr, my_addr;
    int numbytes;
    char buf[MAXSIZE];
    socklen_t addr_len = sizeof(struct sockaddr);

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        perror("socket");
        exit(1);
    }

    serverM_addr.sin_family = AF_INET;
    serverM_addr.sin_port = htons(SERVERMPORT);
    serverM_addr.sin_addr.s_addr = inet_addr(localhost);
    memset(&(serverM_addr.sin_zero), '\0', 8);

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(MYUDPPORT);
    my_addr.sin_addr.s_addr = inet_addr(localhost);
    memset(&(my_addr.sin_zero), '\0', 8);

    if (::bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        exit(1);
    }

    cout << "The ServerC is up and running using UDP on port " << MYUDPPORT << "." << endl;

    while(1){
        string message = "";
        if((numbytes = recvfrom(sockfd, buf, MAXSIZE-1, 0, (struct sockaddr*)&my_addr, &addr_len)) == -1){
            perror("recvfrom");
            continue;
        }

        

        buf[numbytes] = '\0';

        if(buf[0] == '0'){
            int num = 0;

            for(map<int, vector<string> >::iterator it = transactions.begin(); it != transactions.end(); it++){
                num = it->first;
            }

            string message = to_string(num);
            if((numbytes = sendto(sockfd, &message, message.length(), 0, (struct sockaddr*)&serverM_addr, sizeof(struct sockaddr))) == -1){
                perror("sendto");
                exit(1);
            }

        }
        else if(buf[0] == '4'){
            store(buf);
        }else{
            cout << "The ServerC received a request from the Main Server." << endl;
            vector<string> v = convert(buf);
        
            if(v.size() == 1){
                string s = encoder(v[0]);
                int num = 0;
                string recv = "";
                string trans = "";
                if(received.find(s) != received.end()){
                    num = 2;
                    recv = to_string(received[s]);
                }else{
                    recv = "0";
                }
                if(sent.find(s) != sent.end()){
                    num = 2;
                    trans = to_string(sent[s]);
                }else{
                    trans = "0";
                }

                message = to_string(num) + " " + recv + " " + trans;
            }

            if((numbytes = sendto(sockfd, &message, message.length(), 0, (struct sockaddr*)&serverM_addr, sizeof(struct sockaddr))) == -1){
                perror("sendto");
                continue;
            }

            cout << "The ServerC finished sending the response to the Main Server." << endl;
        }

        
    }

    return 0;
}