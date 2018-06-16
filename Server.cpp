//
// Created by heimy4prez on 6/16/18.
//
//
// Created by heimy4prez on 6/16/18.
//

#include "Server.h"
#include "Protocol.h"

Server::Server(unsigned short port) {
    this->_establish(port);

}

ErrorCode Server::_closeConnection(){
    close(this->s);
    return ErrorCode::FAIL;
}

ErrorCode Server::_ParseMessage(int socket)
{

}

ErrorCode Server::_ParseCreateGroup(CreateGroupMessage& /* OUT */ foo) const
{
    std::string listOfClientNames;
    name_len nameLen;
    group_name groupName;
    clients_len clientsLen;
    client_names clientNames;

    // Parse message
    ASSERT_READ(_create_group_fd, &nameLen, sizeof(name_len));
    ASSERT((0 <= nameLen &&
            nameLen <= WA_MAX_NAME), "Invalid group name length");

    groupName = new char[nameLen];
    ASSERT_READ(_create_group_fd, (void*)groupName, nameLen);

    ASSERT_READ(_create_group_fd, &clientsLen, sizeof(clients_len));

    clientNames = new char[clientsLen];
    ASSERT_READ(_create_group_fd, &clientNames, clientsLen);

    //CreateGroupMessage
    return ErrorCode::SUCCESS;
}
ErrorCode _ParseCreateGroup(CreateGroupMessage& /* OUT */ msg){

}
ErrorCode _ParseSendMessage(SendMessage& /* OUT */ msg) {

}
ErrorCode _ParseWho(WhoMessage /* OUT */ msg) {

}
ErrorCode _ParseExist(ExitMessage /* OUT */ msg) {

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
//
//int main(int argc, char const *argv[]){
//    printf("Opening Server\n");
//
//    Server server{};
//    printf("Closing Server\n");
//
//
//}