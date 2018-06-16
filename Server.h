//
// Created by heimy4prez on 6/16/18.
//

#ifndef OSEX4_SERVER_H
#define OSEX4_SERVER_H


#include <cstdio>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_GROUP_LEN 50;
#define MAX_MSG_LEN 256;
#define MAX_NAME_LEN 30;


class Server{

    Server(int port);

    ~Server();

private:
    // fields
    int socketfd, newsockfd, port;
    socklen_t clilen;
    char buffer[256];

    struct sockaddr_in serv_addr, cli_addr;


};

#endif //OSEX4_SERVER_H
