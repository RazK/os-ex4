//
// Created by heimy4prez on 6/16/18.
//
//
// Created by heimy4prez on 6/16/18.
//

#include "Server.h"
#include "Protocol.h"

Server::Server(unsigned short port) {
    if (ErrorCode::SUCCESS != this->_establish(port)){
        print_fail_connection();
        return ;
    }
    print_connection();
    this->_get_connection();
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
ErrorCode Server::_ParseWho(WhoMessage /* OUT */ msg) const{
    return ErrorCode::NOT_IMPLEMENTED;

}

ErrorCode Server::_ParseSendMessage(SendMessage & /* OUT */ msg) const {
    return ErrorCode::NOT_IMPLEMENTED;
}

ErrorCode Server::_ParseExist(ExitMessage /* OUT */ msg) const {
    return ErrorCode::NOT_IMPLEMENTED;

}


ErrorCode Server::_establish(unsigned short port) {

    //hostnet initialization
    if (gethostname(myname, WA_MAX_NAME) < 0){
        printf("gethostname failure in server establish connection");
        return ErrorCode::FAIL;
    }
//    printf("MY HOST NAME: %s \n", myname);

    this->hp = gethostbyname(myname);

    if (this->hp == nullptr){
        printf("got nullptr for gethostbyname in server establish connection");
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

//Server::Server() {
//    if (ErrorCode::SUCCESS != this->_establish(8080)){
//        printf("failed to establish connection");
//    }
//
//    char buffer[256];
//
//    int newsockfd = this->_get_connection();
//
//    if (newsockfd >= 0){
//        send(newsockfd, "Hello, world!\n", 13, 0);
//
//        bzero(buffer,256);
//
//        int n = read(newsockfd,buffer,255);
//        if (n < 0) {
//            printf("ERROR reading from socket");
//        }
//        printf("Here is the message: %s\n",buffer);
//
//        close(newsockfd);
//        close(this->s);
//    } else {
//        printf("Negative communication socket\n");
//        return ;
//    }
//
//
//}

Server::~Server() {

}


//int main(int argc, char const *argv[]){
//    // assert legal usage of a single parameter after prog name
//    if (argc != 2){
//        print_server_usage();
//        exit(-1);
//    }
//
//    // read port number and assert legal - ports positive and are bounded by ushort max
//    size_t * idx;
//    auto port = std::stoi(argv[1], idx, 10 /* base 10 */);
//
//    if (port < 0 or port > USHRT_MAX){
//        print_server_usage();
//        exit(-1);
//    }
//    Server server{(unsigned short)port};
//
//}