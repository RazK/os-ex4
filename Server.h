//
// Created by heimy4prez on 6/16/18.
//

#ifndef OSEX4_SERVER_H
#define OSEX4_SERVER_H


#include <cstdio>
#include <cstdlib>
#include <string>
#include <unistd.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
#include <unistd.h>   // gethostname, gethostbyname

#include "whatsappio.h"
#include "ErrorCode.h"
#include "Protocol.h"

class Server{

    Server(int port);

    ~Server();

private:
    // fields
    int socketfd, newsockfd, port;
    socklen_t clilen;
    char buffer[256];

    //struct sockaddr_in serv_addr, cli_addr;
    int _create_group_fd;
    int _send_fd;
    int _who_fd;
    int _exit_fd;

    ErrorCode Server::_ParseMessage(int socket); // TODO: RazK: Remve after debug (or change
    // signature)

    ErrorCode _ParseCreateGroup(CreateGroupMessage& /* OUT */ msg) const;
    ErrorCode _ParseSendMessage(SendMessage& /* OUT */ msg) const;
    ErrorCode _ParseWho(WhoMessage /* OUT */ msg) const;
    ErrorCode _ParseExist(ExitMessage /* OUT */ msg) const;
};

#endif //OSEX4_SERVER_H
