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
#include <cstring>                  // memset, memcpy


#include <unistd.h>                 // gethostname
#include <netdb.h>                  // gethostbyname
#include <sys/socket.h>
#include <cstdlib>


#include "ErrorCode.h"
#include "whatsappio.h"




class Client{
public:
    //ctors & dtors
    Client();
    Client(const std::string clientName, const std::string serverAddress, const std::string serverPort );
    ~Client() = default;

private:
    //fields
    const std::string _clientName;
    const std::string _serverAddress;
    const unsigned int _serverPort;

    struct sockaddr_in sa;
    struct hostent *hp;
    int s;

    char* buf;

    //methods
    ErrorCode _RequestCreateGroup(const std::string& groupName,
                                  const std::vector<const std::string&>& listOfClientNames) const;
    ErrorCode _RequestSendMessage(const std::string& targetName, const std::string& message) const;
    ErrorCode _RequestWho() const;
    ErrorCode _RequestExist() const;

    bool _uniqueName(std::string newName) const;

    ErrorCode _callSocket(char *hostname, unsigned short port);

    int _readData(int n);
};

#endif //OSEX4_CLIENT_H
