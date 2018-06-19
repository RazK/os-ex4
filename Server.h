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


#include <unistd.h>
#include <algorithm>

#include "Protocol.h"


const int maxNumConnected = 10;

#define SERVER_INPUT_EXIT "EXIT"



class Server{
public:
//    Server();

    explicit Server(unsigned short port);

    ~Server();

private:
    // fields

    char myname[WA_MAX_NAME + 1];
    int welcomeClientsSocket;
    fd_set  openSocketsSet;
//    fd_set * writeSocketSet;
//    fd_set * excptSocketSet;

    std::map <std::string , std::vector <clientWrapper> > groups;
    struct sockaddr_in serve_addr;
    struct hostent *host;

    std::vector<clientWrapper> connectedClients;

    //methods
    ErrorCode _establish(unsigned short port);
    ErrorCode _closeConnection(int socket);

    //struct sockaddr_in serv_addr, cli_addr;

// TODO: RazK: Resume private after testing
public:
    ErrorCode _Run(); // loops with select : calls _HandleIncomingMessage on modified sockets
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

    int _configFDSets();

    int _getConnection();

    void _serverStdInput();

    void _HandleNewClient();

    void _cleanUp();

    ErrorCode _flushToClient(const clientWrapper &client, const std::string &string, bool endl) const;

    /*
    * Description: Return if name is a group this server has initiated.
    */
    bool _isGroup(const std::string& name) const;
    /*
     * Description: Return if name is a client this server has connected to.
    */
    bool _isClient(const std::string& name) const;

    /*
     * Description: Return if given list of names is recognized as connected clients in its entirety
     */
    bool _isClientList(const std::vector<std::string>& names) const;

    /*
    *  Description: Return the wrapper object that fits this name, if exists. null is doesnt.
    */
    const clientWrapper _getClient(const std::string &name) const;

//    void _handleClientRequest();

};

#endif //OSEX4_SERVER_H
