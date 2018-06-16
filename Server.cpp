//
// Created by heimy4prez on 6/16/18.
//
//
// Created by heimy4prez on 6/16/18.
//

//#include <fcntl.h>
#include <unistd.h>

#include "Server.h"
#include "Protocol.h"

Server::Server(unsigned short port) {
    numOfActiveSockets = 10; // 1 socket always listening for new clients // TODO: Determine actual number

    FD_ZERO(&this->clientSocketSet); // Init the set of sockets

    if (ErrorCode::SUCCESS != this->_establish(port)){
        print_fail_connection();
        exit(-1);
    }
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
    FD_SET(this->serverSocketClient, &this->clientSocketSet);
    FD_SET(STDIN_FILENO, &this->clientSocketSet);

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

ErrorCode Server::_closeConnection(){
    close(this->serverSocketClient);
    return ErrorCode::FAIL;
}

ErrorCode Server::_ParseMessage(const clientWrapper& client)
{
    auto socket = client.sock;
    msg_type mtype;
    ASSERT_READ(socket, &mtype, sizeof(msg_type));
    switch (mtype)
    {
        case command_type::CREATE_GROUP:
        {
            std::string groupName(WA_MAX_NAME, 0);
            std::string clientNamesList(WA_MAX_INPUT, 0);
            ASSERT_SUCCESS(_ParseCreateGroup(client, groupName, clientNamesList), "Failed to parse "
                    "create_group message\r\n");

            ASSERT_SUCCESS(_HandleCreateGroup(client, groupName, clientNamesList), "Failed to handle "
                    "create_group message\r\n");
            break;
        }
        case command_type::SEND:
        {
            std::string targetName(WA_MAX_NAME, 0);
            std::string message(WA_MAX_MESSAGE, 0);
            ASSERT_SUCCESS(_ParseSendMessage(client, targetName, message), "Failed to parse "
                    "send_message message\r\n");

            ASSERT_SUCCESS(_HandleSendMessage(client, targetName, message), "Failed to handle "
                    "send_message message\r\n");
            break;
        }
        case command_type::WHO:
        {
            ASSERT_SUCCESS(_HandleWho(client), "Failed to handle who_message\r\n");
            break;
        }
        case command_type::EXIT:
        {
            ASSERT_SUCCESS(_HandleExit(client), "Failed to handle exit_message\r\n");
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

ErrorCode Server::_ParseName(int client_sock, std::string& /* OUT */ clientName){

    char temp[WA_MAX_NAME];
    bzero(temp, WA_MAX_NAME);

    // Read Name
    ASSERT_READ(client_sock, &temp, WA_MAX_NAME);

    // Resize up to '\0'
    int nameLen = strnlen(temp, WA_MAX_NAME);
    clientName = temp;
    clientName.resize(nameLen);

    return ErrorCode::SUCCESS;
}

ErrorCode Server::_ParseCreateGroup(const clientWrapper& client, std::string& /* OUT */ groupName,
                                    std::string& /* OUT */ listOfClientNames) const
{
    name_len nameLen;
    clients_len clientsLen;

    // Parse group-name length
    ASSERT_READ(client.sock, &nameLen, sizeof(name_len));

    // Assert length is legal
    ASSERT((0 <= nameLen &&
            nameLen <= WA_MAX_NAME), "Invalid group name length");

    // Read group name
    ASSERT_READ(client.sock, &groupName[0], nameLen);
    groupName.resize(nameLen);

    // Parse client-names length
    ASSERT_READ(client.sock, &clientsLen, sizeof(clients_len));
    clientsLen = ntohl(clientsLen);

    // Assert length is legal
    ASSERT((0 <= clientsLen &&
            clientsLen <= WA_MAX_MESSAGE),
           "Invalid clients list length");

    // Read client names
    ASSERT_READ(client.sock, &listOfClientNames[0], clientsLen);
    listOfClientNames.resize(clientsLen);

    return ErrorCode::SUCCESS;
}
ErrorCode Server::_ParseSendMessage(const clientWrapper& client,
                                    std::string& /* OUT */ targetName,
                                    std::string& /* OUT */ message) const {
    name_len nameLen;
    message_len messageLen;

    // Parse group-name length
    ASSERT_READ(client.sock, &nameLen, sizeof(name_len));

    // Assert length is legal
    ASSERT((0 <= nameLen &&
            nameLen <= WA_MAX_NAME), "Invalid target name length");

    // Read group name
    ASSERT_READ(client.sock, &targetName[0], nameLen);
    targetName.resize(nameLen);

    // Parse client-names length
    ASSERT_READ(client.sock, &messageLen, sizeof(message_len));
    messageLen = ntohs(messageLen);

    // Assert length is legal
    ASSERT((0 <= messageLen &&
            messageLen <= WA_MAX_MESSAGE),
           "Invalid message length");

    // Read client names
    ASSERT_READ(client.sock, &message[0], messageLen);
    message.resize(messageLen);

    return ErrorCode::SUCCESS;
}

ErrorCode Server::_HandleCreateGroup(const clientWrapper& client,
                                     const std::string& groupName,
                                     const std::string& listOfClientNames)
{
    std::cout << "Server::_HandleCreateGroup\ngroupName : '"<< groupName << "'\nclientsList : '" <<
                                                                                          listOfClientNames << "'\n\r" << std::endl;
    return ErrorCode::NOT_IMPLEMENTED;
}

ErrorCode Server::_HandleSendMessage(const clientWrapper& client,
                                     const std::string& targetName,
                                     const std::string& message)
{
    std::cout << "Server::_HandleSendMessage\ntarget : '" << targetName << "'\nmessage : '" <<
              message << "'\n\r" << std::endl;
    return ErrorCode::NOT_IMPLEMENTED;
}

ErrorCode Server::_HandleWho(const clientWrapper & client)
{
    print_who_server(client.name);

    std::string allNames;
    for (auto &connectedClient : this->connectedClients) {
        allNames += connectedClient.name + ",";
    }
    allNames = allNames.substr(allNames.length() -1);

    ASSERT_WRITE(client.sock, allNames.c_str(), allNames.length());

    return ErrorCode::SUCCESS;
}

ErrorCode Server::_HandleExit(const clientWrapper& client)
{
    return ErrorCode::NOT_IMPLEMENTED;
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
        if (select(this->numOfActiveSockets+1, &readfds, NULL, NULL, NULL) < 0) {
            print_error("_Run - select", 1);
            exit(-1);
        }
        if (FD_ISSET(this->serverSocketClient, &readfds)) {
            //will also add the client to the clientsfds

            this->_connectNewClient();
        }
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            this->_serverStdInput();
        }
        else {
            //will check each client if it’s serverSocketClient in readfds
            //and then receive a message from him
            for (auto clientIt = this->connectedClients.begin(); clientIt != this->connectedClients.end(); ++clientIt ){
                if (FD_ISSET(clientIt->sock, &readfds)){
//                    this->_HandleIncomingMessage(clientIt->sock);
                    this->_ParseMessage(*clientIt);
                    break;
                }
            }
        }
    }

    return FAIL;
}

void Server::_serverStdInput() {
    std::string command;
    std::getline (std::cin, command);
    if (command == (SERVER_INPUT_EXIT)){
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
    std::string name;
    ASSERT_SUCCESS(_ParseName(t, name), "Parse name failed in run");

    printf("New Client: %s", name);

    connectedClients.emplace_back(clientWrapper{t, name});
    this->numOfActiveSockets ++; //increment the number of clients
    FD_SET(t, &this->clientSocketSet);

}

ErrorCode Server::_HandleIncomingMessage(int socket) {
//    _ParseMessage(socket);
    return FAIL;
}


int main(int argc, char const *argv[]){
    // assert legal usage of a single parameter after prog name
    if (argc != 2){
        print_server_usage();
        exit(-1);
    }

    // read port number and assert legal - ports positive and are bounded by ushort max
    size_t * idx;
    auto port = atoi(argv[1]);

    if (port < 0 or port > USHRT_MAX){
        print_server_usage();
        exit(-1);
    }
    Server server{(unsigned short)port};
    server._Run();

}