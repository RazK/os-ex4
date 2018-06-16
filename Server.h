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
#include <climits>                  // max of unsigned short

#include <sys/select.h>             // set and select



#include "whatsappio.h"
#include "ErrorCode.h"
#include "Protocol.h"

const int maxNumConnected = 10;

#define EXIT ("EXIT")



class Server{
public:
//    Server();

    explicit Server(unsigned short port);

    ~Server();

private:
    // fields
//    int socketfd, newsockfd, port;
//    socklen_t clilen;
//    char buffer[256];
//
//    struct sockaddr_in serv_addr, cli_addr;

    char myname[WA_MAX_NAME + 1];
    int serverSocketClient;
    fd_set * clientSocketSet;
    fd_set * writeSocketSet;
    fd_set * excptSocketSet;


    struct sockaddr_in sa;
    struct sockaddr_in cli_addr;
    struct hostent *hp;
    socklen_t clilen;

    std::vector<clientWrapper> connectedClients;

    //methods
    ErrorCode _establish(unsigned short port);
    ErrorCode _closeConnection();

    //struct sockaddr_in serv_addr, cli_addr;
    int _create_group_fd;
    int _send_fd;
    int _who_fd;
    int _exit_fd;


    ErrorCode _Run(); // loops with select : calls _HandleIncomingMessage on modified sockets
    ErrorCode _HandleIncomingMessage(int socket); // Calls the parser on relevent socket

    ErrorCode _ParseMessage(int socket); // TODO: RazK: Remve after debug (or change
    // signature)

    ErrorCode _ParseCreateGroup(std::string& /* OUT */ groupName,
                                std::string& /* OUT */ listOfClientNames) const;
    ErrorCode _ParseSendMessage(std::string& /* OUT */ targetName,
                                std::string& /* OUT */ message) const;
    ErrorCode _ParseWho() const;
    ErrorCode _ParseExist() const;

    ErrorCode _HandleCreateGroup(const std::string& groupName,
                                 const std::string& listOfClientNames);
    ErrorCode _HandleSendMessage(const std::string& targetName,
                                 const std::string& message);
    ErrorCode _HandleWho() const;
    ErrorCode _HandleExit() const;

    int _get_connection();

    int numOfActiveSockets;

    void _serverStdInput();

    void _connectNewClient();

    void _handleClientRequest();
};

#endif //OSEX4_SERVER_H
