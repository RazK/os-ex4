//
// Created by razkarl on 6/16/18.
//

#include "Protocol.h"

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