//
// Created by heimy4prez on 6/16/18.
//

#ifndef OSEX4_SERVER_H
#define OSEX4_SERVER_H


#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <unistd.h>                 // gethostname
#include <netdb.h>                  // gethostbyname



#include "whatsappio.h"
#include "ErrorCode.h"
#include "Protocol.h"

const int maxNumConnected = 10;



class Server{
public:
    Server();

    Server(unsigned short port);

    ~Server() = default;

private:
    // fields
//    int socketfd, newsockfd, port;
//    socklen_t clilen;
//    char buffer[256];
//
//    struct sockaddr_in serv_addr, cli_addr;

    char myname[WA_MAX_NAME + 1];
    int s;
    struct sockaddr_in sa;
    struct sockaddr_in cli_addr;
    struct hostent *hp;
    socklen_t clilen;

    //methods
    ErrorCode _establish(unsigned short port);
    ErrorCode _closeConnection();

    //struct sockaddr_in serv_addr, cli_addr;

// TODO: RazK: Resume private after testing
public:
    int _create_group_fd;
    int _send_fd;
    int _who_fd;
    int _exit_fd;


    ErrorCode _Run(); // loops with select : calls _HandleIncomingMessage on modified sockets
    ErrorCode _HandleIncomingMessage(int socket); // Calls the parser on relevent socket

// TODO: RazK: Resume private after testing
public:
    ErrorCode _ParseMessage(int socket); // TODO: RazK: Remve after debug (or change
    // signature)

    ErrorCode _ParseCreateGroup(std::string& /* OUT */ groupName,
                                std::string& /* OUT */ listOfClientNames) const;
    ErrorCode _ParseSendMessage(std::string& /* OUT */ targetName,
                                std::string& /* OUT */ message) const;

    ErrorCode _HandleCreateGroup(const std::string& groupName,
                                 const std::string& listOfClientNames);
    ErrorCode _HandleSendMessage(const std::string& targetName,
                                 const std::string& message);
    ErrorCode _HandleWho();
    ErrorCode _HandleExit();

    int _get_connection();
};

#endif //OSEX4_SERVER_H
