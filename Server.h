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
#include <map>


#include "whatsappio.h"
#include "ErrorCode.h"
#include "Protocol.h"

const int maxNumConnected = 10;

#define SERVER_INPUT_EXIT "EXIT"



class Server{
public:
//    Server();

    explicit Server(unsigned short port);

    ~Server() = default;

private:
    // fields

    char myname[WA_MAX_NAME + 1];
    int serverSocketClient;
    fd_set  clientSocketSet;
//    fd_set * writeSocketSet;
//    fd_set * excptSocketSet;

    std::map <std::string , std::vector <clientWrapper> > groups;
    struct sockaddr_in sa;
    struct sockaddr_in cli_addr;
    struct hostent *hp;
    socklen_t clilen;

    std::vector<clientWrapper> connectedClients;

    //methods
    ErrorCode _establish(unsigned short port);
    ErrorCode _closeConnection();

    //struct sockaddr_in serv_addr, cli_addr;

// TODO: RazK: Resume private after testing
public:
    ErrorCode _run(); // loops with select : calls _HandleIncomingMessage on modified sockets
    ErrorCode _HandleIncomingMessage(int socket); // Calls the parser on relevent socket

// TODO: RazK: Resume private after testing
public:
    ErrorCode _ParseMessage(const clientWrapper & client); // TODO: RazK: Remve after debug (or change
    // signature)
    ErrorCode _ParseName(int client_sock,
                         std::string& /* OUT */ clientName);
    ErrorCode _ParseCreateGroup(const clientWrapper& client,
                                std::string& /* OUT */ groupName,
                                std::string& /* OUT */ listOfClientNames) const;
    ErrorCode _ParseSendMessage(const clientWrapper& client,
                                std::string& /* OUT */ targetName,
                                std::string& /* OUT */ message) const;
    ErrorCode _HandleCreateGroup(const clientWrapper& client,
                                 const std::string& groupName,
                                 const std::string& listOfClientNames);
    ErrorCode _HandleSendMessage(const clientWrapper& client,
                                 const std::string& targetName,
                                 const std::string& message);
    ErrorCode _HandleWho(const clientWrapper& client);
    ErrorCode _HandleExit(const clientWrapper& client);

    int _getConnection();

    int numOfActiveSockets;

    void _serverStdInput();

    void _connectNewClient();

    bool Server::_isClient(const std::string& name) const;

    bool Server::_isClientList(const std::vector<std::string>& names) const;

    void _handleClientRequest();
};

#endif //OSEX4_SERVER_H
