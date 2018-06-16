//
// Created by heimy4prez on 6/16/18.
//
//
// Created by heimy4prez on 6/16/18.
//

#include <fcntl.h>
#include "Server.h"
#include "Protocol.h"

Server::Server(unsigned short port) {
    numOfActiveSockets = 1; // 1 socket always listening for new clients

    FD_ZERO(this->writeSocketSet); // Init the set of sockets
    FD_ZERO(this->clientSocketSet); // Init the set of sockets
    FD_ZERO(this->excptSocketSet); // Init the set of sockets


    if (ErrorCode::SUCCESS != this->_establish(port)){
        print_fail_connection();
        exit(-1);
    }
    print_connection();
//    this->_get_connection();
}

ErrorCode Server::_closeConnection(){
    close(this->serverSocketClient);
    return ErrorCode::FAIL;
}

ErrorCode Server::_ParseMessage(int socket)
{
    msg_type mtype;
    ASSERT_READ(socket, &mtype, sizeof(msg_type));
    switch (mtype)
    {
        case command_type::CREATE_GROUP:
        {
            std::string groupName(WA_MAX_NAME, 0);
            std::string clientNamesList(WA_MAX_INPUT, 0);
            ASSERT_SUCCESS(_ParseCreateGroup(groupName, clientNamesList), "Failed to parse "
                    "create_group message\r\n");

            ASSERT_SUCCESS(_HandleCreateGroup(groupName, clientNamesList), "Failed to handle "
                    "create_group message\r\n");
            break;
        }
        case command_type::SEND:
        {
            std::string targetName(WA_MAX_NAME, 0);
            std::string message(WA_MAX_MESSAGE, 0);
            ASSERT_SUCCESS(_ParseSendMessage(targetName, message), "Failed to parse "
                    "send_message message\r\n");

            ASSERT_SUCCESS(_HandleSendMessage(targetName, message), "Failed to handle "
                    "send_message message\r\n");
            break;
        }
        case command_type::WHO:
        {
            ASSERT_SUCCESS(_HandleWho(), "Failed to handle who_message\r\n");
            break;
        }
        case command_type::EXIT:
        {
            ASSERT_SUCCESS(_HandleExit(), "Failed to handle exit_message\r\n");
            break;
        }
        default:
        {
            printf("Unrecognized message type\r\n");
            return ErrorCode::FAIL;
        }
    }
    return ErrorCode::SUCCESS;
}

ErrorCode _ParseName(int sock_fd, std::string& clientName){
    ASSERT_READ(sock_fd, &clientName[0], WA_MAX_NAME);
    return ErrorCode::SUCCESS;
}

ErrorCode Server::_ParseCreateGroup(std::string& groupName,
                                    std::string& listOfClientNames) const
{
    name_len nameLen;
    clients_len clientsLen;

    // Parse group-name length
    ASSERT_READ(_create_group_fd, &nameLen, sizeof(name_len));

    // Assert length is legal
    ASSERT((0 <= nameLen &&
            nameLen <= WA_MAX_NAME), "Invalid group name length");

    // Read group name
    ASSERT_READ(_create_group_fd, &groupName[0], nameLen);
    groupName.resize(nameLen);

    // Parse client-names length
    ASSERT_READ(_create_group_fd, &clientsLen, sizeof(clients_len));
    clientsLen = ntohl(clientsLen);

    // Assert length is legal
    ASSERT((0 <= clientsLen &&
            clientsLen <= WA_MAX_MESSAGE),
           "Invalid clients list length");

    // Read client names
    ASSERT_READ(_create_group_fd, &listOfClientNames[0], clientsLen);
    listOfClientNames.resize(clientsLen);

    return ErrorCode::SUCCESS;
}
ErrorCode Server::_ParseSendMessage(std::string& /* OUT */ targetName,
                                    std::string& /* OUT */ message) const {
    name_len nameLen;
    message_len messageLen;

    // Parse group-name length
    ASSERT_READ(_send_fd, &nameLen, sizeof(name_len));

    // Assert length is legal
    ASSERT((0 <= nameLen &&
            nameLen <= WA_MAX_NAME), "Invalid target name length");

    // Read group name
    ASSERT_READ(_send_fd, &targetName[0], nameLen);
    targetName.resize(nameLen);

    // Parse client-names length
    ASSERT_READ(_send_fd, &messageLen, sizeof(message_len));
    messageLen = ntohs(messageLen);

    // Assert length is legal
    ASSERT((0 <= messageLen &&
            messageLen <= WA_MAX_MESSAGE),
           "Invalid message length");

    // Read client names
    ASSERT_READ(_send_fd, &message[0], messageLen);
    message.resize(messageLen);

    return ErrorCode::SUCCESS;
}

ErrorCode Server::_HandleCreateGroup(const std::string& groupName,
                             const std::string& listOfClientNames)
{
    std::cout << "Server::_HandleCreateGroup\ngroupName : '"<< groupName << "'\nclientsList : '" <<
                                                                                          listOfClientNames << "'\n\r" << std::endl;
    return ErrorCode::NOT_IMPLEMENTED;
}

ErrorCode Server::_HandleSendMessage(const std::string& targetName,
                             const std::string& message)
{
    std::cout << "Server::_HandleSendMessage\ntarget : '" << targetName << "'\nmessage : '" <<
              message << "'\n\r" << std::endl;
    return ErrorCode::NOT_IMPLEMENTED;
}

ErrorCode Server::_HandleWho()
{
    return ErrorCode::NOT_IMPLEMENTED;
}

ErrorCode Server::_HandleExit()
{
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
    this->serverSocketClient = socket(AF_INET, SOCK_STREAM, 0);
    if(this->serverSocketClient < 0){
        printf("err whilst creating socket in server establish connection");
        return ErrorCode::FAIL;

    }

    // add that socket to the set for select
    FD_SET(this->serverSocketClient, this->clientSocketSet);
    FD_SET(STDIN_FILENO, this->clientSocketSet);

    if (bind(this->serverSocketClient , (struct sockaddr *)&(this->sa) , sizeof(struct sockaddr_in)) < 0){
        printf("Err in bind");
        return ErrorCode::FAIL;

    }


    /* max # of queued connects */
    if (listen(this->serverSocketClient, maxNumConnected) < 0){
        printf("failed to listen in server establish connection");
        return ErrorCode::FAIL;
    }

    return ErrorCode::SUCCESS;
}

int Server::_get_connection() {
//    int t= accept(this->serverSocketClient,NULL, NULL); /* socket of connection */
    clilen = sizeof(cli_addr);
    int t = accept(this->serverSocketClient, (struct sockaddr *) &cli_addr, &clilen);

    if (t < 0){
        return -1;
    }
    return t;
}


ErrorCode Server::_Run() {
//    struct timeval interval{0, 100};
    bool stillRunning = true;
    while (stillRunning){
        auto readfds = this->clientSocketSet; //TODO: need to make a copy here..
        if (select(this->numOfActiveSockets+1, readfds, NULL, NULL, NULL) < 0) {
            print_error("_Run - select", 1);
            exit(-1);
        }
        if (FD_ISSET(this->serverSocketClient, readfds)) {
            //will also add the client to the clientsfds

            this->_connectNewClient();
        }
        if (FD_ISSET(STDIN_FILENO, readfds)) {
            this->_serverStdInput();
        }
        else {
            //will check each client if it’s serverSocketClient in readfds
            //and then receive a message from him
            for (auto i = this->connectedClients.begin(); i != this->connectedClients.end(); ++i ){
                if (FD_ISSET(i->sock, readfds)){
                    this->_HandleIncomingMessage(i->sock);
                    break;
                }
            }
        }
    }


    auto s = select(this->numOfActiveSockets, this->clientSocketSet, this->writeSocketSet, this->excptSocketSet, NULL);

    if (ErrorCode::FAIL == s){

    }
    if (s == this->serverSocketClient){ //

    }

    return FAIL;
}

Server::Server() {
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

    // TODO: REMOVE THESE LINES
    this->_create_group_fd = open("/cs/+/usr/razkarl/os-ex4/create_group.txt", O_CREAT | O_RDWR |
                                                                               O_NONBLOCK);
    if (this->_create_group_fd == -1)
    {
        perror("open create_group.txt failed");
    }

    this->_send_fd = open("/cs/+/usr/razkarl/os-ex4/send.txt", O_CREAT | O_RDWR | O_NONBLOCK);
    if (this->_send_fd == -1)
    {
        perror("open send.txt failed");
    }

    this->_who_fd = open("/cs/+/usr/razkarl/os-ex4/who.txt", O_CREAT | O_RDWR | O_NONBLOCK);
    if (this->_who_fd == -1)
    {
        perror("open who.txt failed");
    }

    this->_exit_fd = open("/cs/+/usr/razkarl/os-ex4/exit.txt", O_CREAT | O_RDWR | O_NONBLOCK);
    if (this->_exit_fd == -1)
    {
        perror("open exit.txt failed");
    }
}


void Server::_serverStdInput() {
    std::string command;
    std::getline (stdin, command);
    if (command == (EXIT)){
        print_exit();
        //TODO: exit gracefully... not like a baffoon
        exit(0);
    }

}

void Server::_connectNewClient() {
    auto t = _get_connection();
    if (t < 0){
        print_error("_Run - connectNewClient", 2);
        exit(-1);
    }


    //TODO: get the name before this step
    connectedClients.emplace_back(clientWrapper{t, "noname"});
    this->numOfActiveSockets ++; //increment the number of clients
    FD_SET(t, this->clientSocketSet);

}

ErrorCode Server::_HandleIncomingMessage(int socket) {
    _ParseMessage(socket);
    return FAIL;
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