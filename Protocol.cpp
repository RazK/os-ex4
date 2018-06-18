//
// Created by razkarl on 6/16/18.
//

#include "Protocol.h"
#include <unistd.h>
#include <sstream>

int max(int a, int b){
    return ((a) >= (b) ? (a) : (b));
}

bool isValidName(const std::string& name){
    for(int i = 0; i < name.length(); i++) {
        auto curr = name[i];
        if (!(('0' <= curr && curr <= '9') ||
              ('a' <= curr && curr <= 'z') ||
              ('A' <= curr && curr <= 'Z'))) {
            return false;
        }
    }
    return true;
}

void split(const std::string& string,
           const char& delim,
           std::vector<std::string> result/*OUT */ ){
    std::string token;
    std::istringstream tokenStream(string);
    while(std::getline(tokenStream, token, delim)){
        result.push_back(token);
    }
}

bool isValidList(const std::vector<std::string>& names){

    for (auto iter = names.begin(); iter != names.end(); ++iter){
        if (! isValidName(*iter)){ //found invalid name
            return false;
        }
    }
    return true;
}

int _readData(int socket, void * buf , size_t n){
    int bcount;
    /* counts bytes read */
    ssize_t br;
    /* bytes read this pass */
    bcount = 0;
    br = 0;
    memset(buf, 0, n);
    while (bcount < n) { /* loop until full buffer */
        br = read(socket, buf, (size_t) n - bcount);
        if ((br > 0)) {
            bcount += br;
            buf += br; //Todo: why add val to char *?
        }
        if (br < 1) {
            return (br);
        }
    }
    return (bcount);
}


int readFromSocket(int socket, std::string &outbuf, int count){
    int bcount;
    /* counts bytes read */
    ssize_t br;
    /* bytes read this pass */
    bcount = 0;
    br = 0;
    outbuf = "";
    while (bcount < count) { /* loop until full buffer */
        char next = 0;
        br = read(socket, &next, (size_t)1);
        if ((br > 0)) {
            bcount += br;
            outbuf += next;
        }
        if (br < 1) {
            return (-1);
        }
    }
    return (bcount);
}