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


#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <fstream>
#include <cstdio>
#include <netinet/in.h>

#include "Protocol.h"
#include "ErrorCode.h"
#include "whatsappio.h"


class Client{
public:
    //ctors & dtors
//    Client();
    Client(const std::string clientName, const std::string serverAddress, const int serverPort );
    ~Client();

private:
//    //fields
//    const std::string _clientName;
//    const std::string _serverAddress;
//    const unsigned int _serverPort;
//    int _sock_fd;
//    int _create_group_fd;
//    int _send_fd;
//    int _who_fd;
//    int _exit_fd;

    struct sockaddr_in serv_addr;
    struct hostent *host;
    fd_set  openSocketsSet;

    int connectedServer;

    char* buf;

    std::string name;

    //methods
public: // TODO: Raz: Resume private after done debugging
    ErrorCode _ClientStdInput();
    ErrorCode _ParseMessageFromServer();

    ErrorCode _TellName(const std::string& myName);
    ErrorCode _RequestCreateGroup(const std::string& groupName,
                                  const std::string& listOfClientNames);
    ErrorCode _RequestSendMessage(const std::string& targetName, const std::string& message) const;
    ErrorCode _RequestWho() const;
    ErrorCode _RequestExist() const;

    ErrorCode _Run() ;

    int _configFDSets();

    ErrorCode _callSocket(const char *hostname, unsigned short port);
};

#endif //OSEX4_CLIENT_H
