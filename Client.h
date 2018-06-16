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

#include "Messages.h"
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
//    std::

    //methods

    ErrorCode _creatGroup(const std::string groupName, const std::string listOfClientNames);
    ErrorCode _send(const std::string targetName, const std::string message);

    bool _uniqueName(std::string newName) const;



};

#endif //OSEX4_CLIENT_H
