//
// Created by heimy4prez on 6/16/18.
//
//
// Created by heimy4prez on 6/16/18.
//

#include "Server.h"
#include "Protocol.h"

Server::Server(unsigned short port) {
    ASSERTANDDO_SUCCESS(this->_establish(port), );

}

ErrorCode Server::_closeConnection(){
    close(this->s);
    return ErrorCode::FAIL;
}

ErrorCode Server::_establish(unsigned short port) {


    //hostnet initialization
    ASSERTANDRET_SUCCESS(gethostname(myname, WA_MAX_NAME),
                         "gethostname failure in server establish connection");

    this->hp = gethostbyname(myname);

    ASSERTANDRET_NOT_NULL(this->hp, "got nullptr for gethostbyname in server establish connection");

    //sockaddrr_in initialization
    memset(&(this->sa), 0, sizeof(struct sockaddr_in));
    this->sa.sin_family = this->hp->h_addrtype;

    /* this is our host address */
    memcpy(this->&sa.sin_addr, this->hp->h_addr, this->hp->h_length);

    /* this is our port number */
    this->sa.sin_port = htons(port);

    /* create socket */
    ASSERTANDRET_SUCCESS((this->s = socket(AF_INET, SOCK_STREAM, 0)),
                         "err whilst creating socket in server establish connection");

    ASSERTANDDO((bind(this->s , (struct sockaddr *)&(this->sa) , sizeof(struct sockaddr_in)) < 0),
                "err whilst binding socket in server establish connection",
                _closeConnection);

    /* max # of queued connects */
    ASSERTANDRET_SUCCESS(listen(this->s, maxNumConnected), "failed to listen in server establish connection");

    return ErrorCode::SUCCESS;
}

ErrorCode Server::_ParseMessage(int socket);

ErrorCode Server::_ParseCreateGroup(CreateGroupMessage& /* OUT */ msg) const
{
    std::string listOfClientNames;

    // Parse message
    ASSERT_READ(_create_group_fd, &msg.nameLen, sizeof(msg.nameLen));
    ASSERT((0 <= msg.nameLen &&
            msg.nameLen <= WA_MAX_NAME), "Invalid group name length");

    msg.groupName = new char[msg.nameLen];
    ASSERT_READ(_create_group_fd, msg.groupName, msg.nameLen);

    ASSERT_READ(_create_group_fd, &msg.clientsLen, sizeof(msg.clientsLen));
    ASSERT_READ(_create_group_fd, msg.clientNames, listOfClientNames.length());

    return ErrorCode::SUCCESS;
}
ErrorCode _ParseCreateGroup(CreateGroupMessage& /* OUT */ msg) const;
ErrorCode _ParseSendMessage(SendMessage& /* OUT */ msg) const;
ErrorCode _ParseWho(WhoMessage /* OUT */ msg) const;
ErrorCode _ParseExist(ExitMessage /* OUT */ msg) const;
#include "Server.h"

Server::Server(unsigned short port) {

}

ErrorCode Server::_closeConnection(){
    close(this->s);
    return ErrorCode::FAIL;
}

ErrorCode Server::_establish(unsigned short port) {

    //hostnet initialization
    if (gethostname(myname, WA_MAX_NAME) < 0){
        printf("gethostname failure in server establish connection");
        return ErrorCode::FAIL;
    }
    printf("MY HOST NAME: %s \n", myname);

    this->hp = gethostbyname(myname);

    printf("MY HP NAME:  %s \n", this->hp->h_name);


    if (this->hp == nullptr){
        printf("got nullptr for gethostbyname in server establish connection");
        printf(myname);
        return ErrorCode::FAIL;
    }

    //sockaddrr_in initialization
    memset(&(this->sa), 0, sizeof(struct sockaddr_in));
    this->sa.sin_family = this->hp->h_addrtype;

    /* this is our host address */
    memcpy(&this->sa.sin_addr, this->hp->h_addr, this->hp->h_length);

    /* this is our port number */
    this->sa.sin_port = htons(port);

    /* create socket */
    this->s = socket(AF_INET, SOCK_STREAM, 0);
    if(this->s < 0){
        printf("err whilst creating socket in server establish connection");
        printf(myname);
        return ErrorCode::FAIL;

    }

    if (bind(this->s , (struct sockaddr *)&(this->sa) , sizeof(struct sockaddr_in)) < 0){
        printf("Err in bind");
        return ErrorCode::FAIL;

    }


    /* max # of queued connects */
    if (listen(this->s, maxNumConnected) < 0){
        printf("failed to listen in server establish connection");
        return ErrorCode::FAIL;
    }

    return ErrorCode::SUCCESS;
}

int Server::_get_connection() {
//    int t= accept(this->s,NULL, NULL); /* socket of connection */
    clilen = sizeof(cli_addr);
    int t = accept(this->s, (struct sockaddr *) &cli_addr, &clilen);

    if (t < 0){
        return -1;
    }
    return t;
}

Server::Server() {
    if (ErrorCode::SUCCESS != this->_establish(8080)){
        printf("failed to establish connection");
    }

    char buffer[256];

    int newsockfd = this->_get_connection();

    if (newsockfd >= 0){
        send(newsockfd, "Hello, world!\n", 13, 0);

        bzero(buffer,256);

        int n = read(newsockfd,buffer,255);
        if (n < 0) {
            printf("ERROR reading from socket");
        }
        printf("Here is the message: %s\n",buffer);

        close(newsockfd);
        close(this->s);
    } else {
        printf("Negative communication socket\n");
        return ;
    }


}

int main(int argc, char const *argv[]){
    printf("Opening Server\n");

    Server server{};
    printf("Closing Server\n");


}