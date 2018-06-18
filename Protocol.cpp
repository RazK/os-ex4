//
// Created by razkarl on 6/16/18.
//

#include "Protocol.h"

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
    // at least 1 letter...
    return (0 < name.length() && name.length() <= WA_MAX_NAME);
}

std::string getResponse(TaskCase taskCase){
    if (taskCase == TaskCase::SUCCEEDED){
        return TASK_SUCCESSFUL;
    }
    if (taskCase == TaskCase::FAILED){
        return TASK_FAILURE;
    }
    if (taskCase == TaskCase::BUGGED){
        return TASK_BUG;
    }
    return TASK_UNDEFINED;
}

TaskCase bool2TCase(bool valid){
    if (valid){
        return TaskCase::SUCCEEDED;
    }
    return TaskCase::FAILED;
}

void split(const std::string& string,
           const char& delim,
           std::vector<std::string>& result /*OUT */ ){
    std::string token;
    std::istringstream tokenStream(string);
    while(std::getline(tokenStream, token, delim)){
        result.emplace_back(token);
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
    buf = (char *) buf;
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

std::string padMessage(std::string string, int resultSize) {
    auto len = string.length();
    for (int _ = 0; _ < resultSize - len; ++_ ){
        string += PAD;
    }
    return string;
}