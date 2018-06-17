//
// Created by heimy4prez on 6/16/18.
//
//
// Created by heimy4prez on 6/16/18.
//

//#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include "Server.h"
#include "Protocol.h"

#define NUM_OF_ALWAYS_OPEN_SOCKETS 3 // STDIN, STDOUT, STDERR

Server::Server(unsigned short port) {
    FD_ZERO(&this->openSocketsSet); // Init the set of sockets

    if (ErrorCode::SUCCESS != this->_establish(port)){
        print_fail_connection();
        exit(-1);
    }
}


int Server::_configFDSets(){
    int max_fd = STDIN_FILENO;

    // Zero the set of sockets
    FD_ZERO(&this->openSocketsSet);

    // Init the set of sockets
    FD_SET(STDIN_FILENO, &this->openSocketsSet);                   // STDIN
    FD_SET(this->welcomeClientsSocket, &this->openSocketsSet);       // New connections socket
    max_fd = max(STDIN_FILENO, welcomeClientsSocket);
    for (auto sock_fd : this->connectedClients) {
        FD_SET(sock_fd.sock, &this->openSocketsSet);               // Each connection socket
        max_fd = max(sock_fd.sock, max_fd);
    }

    return max_fd;
}

ErrorCode Server::_establish(unsigned short port) {

    //hostnet initialization
    if (gethostname(myname, WA_MAX_NAME) < 0){
        printf("gethostname failure in server establish connection");
        return ErrorCode::FAIL;
    }
//    printf("MY HOST NAME: %connectedServer \n", myname);

    this->host = gethostbyname(myname);
    if (this->host == nullptr){
        printf("got nullptr for gethostbyname in server establish connection");
        return ErrorCode::FAIL;
    }

    //sockaddrr_in initialization
    memset(&(this->serve_addr), 0, sizeof(struct sockaddr_in));
    //this->serve_addr.sin_family = this->host->h_addrtype;
    this->serve_addr.sin_family = AF_INET;

    /* this is our host address */
    //memcpy(&this->serve_addr.sin_addr, this->host->h_addr, this->host->h_length);
    this->serve_addr.sin_addr.s_addr = INADDR_ANY;

    /* this is our port number */
    this->serve_addr.sin_port = htons(port);

    /* create socket */
    this->welcomeClientsSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(this->welcomeClientsSocket < 0){
        printf("err whilst creating socket in server establish connection");
        return ErrorCode::FAIL;

    }

    // add that socket to the set for select
    FD_SET(this->welcomeClientsSocket, &this->openSocketsSet);
    FD_SET(STDIN_FILENO, &this->openSocketsSet);

    if (bind(this->welcomeClientsSocket , (struct sockaddr *)&(this->serve_addr) , sizeof(struct sockaddr_in)) < 0){
        printf("Err in bind");
        return ErrorCode::FAIL;

    }


    /* max # of queued connects */
    if (listen(this->welcomeClientsSocket, maxNumConnected) < 0){
        printf("failed to listen in server establish connection");
        return ErrorCode::FAIL;
    }

    return ErrorCode::SUCCESS;
}

ErrorCode Server::_closeConnection(int socket){
    FD_CLR(socket, &this->openSocketsSet);
    if (0 != close(socket)){
        perror("Failed to close socket");
        return ErrorCode::FAIL;
    }
    auto testIfRemove = [socket](clientWrapper cur){return cur.sock == socket;};
    this->connectedClients.erase(std::remove_if(this->connectedClients.begin(),
                                                this->connectedClients.end(),
                                                testIfRemove),
                                 this->connectedClients.end());
    return ErrorCode::SUCCESS;
}

ErrorCode Server::_ParseMessage(const clientWrapper& client)
{
    auto socket = client.sock;
    msg_type mtype = _command_type::INVALID;
    if(0 == _readData(socket, &mtype, sizeof(msg_type)))
    {
        printf("Read 0 bytes from socket - closed on the other side (client disconnected?)\r\n"); // https://stackoverflow.com/questions/2416944/can-read-function-on-a-connected-socket-return-zero-bytes
        return this->_closeConnection(socket);
    }
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

    //char temp[WA_MAX_NAME];
    //bzero(temp, WA_MAX_NAME);
    std::string temp = "";

    // Read Name
    //ASSERT_READ(client_sock, &temp, WA_MAX_NAME);
    readFromSocket(client_sock, temp, WA_MAX_NAME);

    // Resize up to '\0'
    int nameLen = strnlen(temp.c_str(), WA_MAX_NAME);
    clientName = temp;
    clientName.resize(nameLen);

    // Assert name is legal
    ASSERT(isValidName(clientName), "Invalid client name characters");

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

    // Assert name is legal
    ASSERT(isValidName(groupName), "Invalid group name characters");

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
    // Assert name is legal
    ASSERT(isValidName(targetName), "Invalid target name characters");

    // Parse message length
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
//    int newConnection= accept(this->welcomeClientsSocket,NULL, NULL); /* socket of connection */
    int newConnection = accept(this->welcomeClientsSocket, nullptr, nullptr);

    if (newConnection < 0){
        return -1;
    }
    return newConnection;
}


ErrorCode Server::_Run() {
    while (true){
        int max_fd = this->_configFDSets();
        auto readfds = this->openSocketsSet; //TODO: need to make a copy here..
        if (select(max_fd+1, &readfds, nullptr, nullptr, nullptr) < 0) {
            print_error("_Run - select", 1);
            exit(-1);
        }
        if (FD_ISSET(this->welcomeClientsSocket, &readfds)) {
            //will also add the client to the clientsfds
            this->_connectNewClient();
            continue;
        }
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            this->_serverStdInput();
            continue;
        }
        else {
            //will check each client if it’connectedServer welcomeClientsSocket in readfds
            //and then receive a message from him
            for (auto client : this->connectedClients){
                if (FD_ISSET(client.sock, &readfds)){
//                    this->_HandleIncomingMessage(client->sock);
                    this->_ParseMessage(client);
                    break;
                }
            }
            continue;
        }
    }
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

    printf("New Client: %s\r\n", name.c_str());

    connectedClients.emplace_back(clientWrapper{t, name});
    //FD_SET(t, &this->openSocketsSet);
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