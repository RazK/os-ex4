//
// Created by heimy4prez on 6/16/18.
//

#ifndef OSEX4_CLIENT_H
#define OSEX4_CLIENT_H


#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "ErrorCode.h"




class Client{
public:
    //ctors & dtors
    Client(const std::string clientName, const std::string serverAddress, const std::string serverPort );
    ~Client();

private:
    //fields
    const std::string _clientName;
    const std::string _serverAddress;
    const unsigned int _serverPort;

    //methods
    ErrorCode _RequestCreateGroup(const std::string& groupName,
                                  const std::vector<const std::string&>& listOfClientNames) const;
    ErrorCode _RequestSendMessage(const std::string& targetName, const std::string& message) const;
    ErrorCode _RequestWho() const;
    ErrorCode _RequestExist() const;

    bool _uniqueName(std::string newName) const;

};

#endif //OSEX4_CLIENT_H
