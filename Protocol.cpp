//
// Created by razkarl on 6/16/18.
//

#include "Protocol.h"
#include <unistd.h>



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

int _readData(int socket, void * buf , size_t n){
    int bcount;
    /* counts bytes read */
    ssize_t br;
    /* bytes read this pass */
    bcount = 0;
    br = 0;
    while (bcount < n) { /* loop until full buffer */
        br = read(socket, buf, (size_t) n - bcount);
        if ((br > 0)) {
            bcount += br;
            buf += br; //Todo: why add val to char *?
        }
        if (br < 1) {
            return (-1);
        }
    }
    return (bcount);
}