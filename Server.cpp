//
// Created by heimy4prez on 6/16/18.
//
//
// Created by heimy4prez on 6/16/18.
//

//#include <fcntl.h>
#include "Server.h"


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

bool compareClientWrapper(const clientWrapper& cw1, const clientWrapper& cw2){
    return cw1.name < cw2.name;
}

ErrorCode Server::_HandleCreateGroup(const clientWrapper& client,
                                     const std::string& groupName,
                                     const std::string& listOfClientNames)
{
    // emplace in groupmembers the results of parsing the list of clients
    std::vector<std::string> groupMembers;
    split(listOfClientNames, ',', groupMembers);

    // add the calling client to group, just in case not in it.
    groupMembers.emplace_back(client.name);

    // make unique and clip
    std::sort(groupMembers.begin(), groupMembers.end());
    auto it = std::unique(groupMembers.begin(), groupMembers.end());
    groupMembers.resize((unsigned long)std::distance(groupMembers.begin(), it));

    //also sort the list of clients this server has connected to, for good measure and runtime
    std::sort(this->connectedClients.begin(), this->connectedClients.end(), compareClientWrapper);

    // logic for valid group member list
    bool valid = (isValidName(groupName) &&                         // legal group name
        (! groupMembers.empty()) &&                                 // non empty member set
        (groupMembers.size() <= WA_MAX_GROUP) &&                    // group not too big
        (groupMembers != std::vector<std::string>{client.name}) &&  // group isn't singleton creator
        (this->groups.count(groupName) == 0 ) &&                    // unique group name
        (this->_isClientList(groupMembers)) &&                      // All are clients
        (!this->_isClient(groupName))                               // This group name not in use as client name
    );


    if (valid){
        std::vector<clientWrapper> wrappedGroup;
        for (const auto &name : groupMembers) {
            wrappedGroup.emplace_back(this->_getClient(name));
        }
        this->groups.emplace(std::pair<std::string, std::vector<clientWrapper>>(groupName, wrappedGroup)); //TODO: maybe should clientWrapper rather than string?
    }

    print_create_group(true, valid, client.name, listOfClientNames);

    //TODO: force response in client side
    return ErrorCode::SUCCESS; //invalid does not mean not success
}

ErrorCode Server::_HandleSendMessage(const clientWrapper& client,
                                     const std::string& targetName,
                                     const std::string& message)
{
    bool valid = (
            isValidName(client.name) &&                     // client is legal name -- superfluous
            _isClient(client.name) &&                       // client is recognized -- superfluous
            isValidName(targetName) &&                      // target has legal name
            _isClient(targetName)                           // client is recognized
                                                            // TODO: valid message?
    );

    if (valid){
        auto targetClientW = this->_getClient(targetName);
        ASSERT(nullptr != &targetClientW, "Got unexpected nullptr in handle message" );
            //TODO: flush message to the target using the targetClientW
    }

    print_send(true, valid, client.name, targetName, message);
    //TODO: force response in client side

    return ErrorCode::SUCCESS; //invalid does not mean not success
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
    bool legal = (this->_isClient(client.name));            // reality check.. should be true if no bug


    if (legal){
        auto iter = this->connectedClients.begin();
        for (iter ; iter != this->connectedClients.end(); ++iter) {
            if ((*iter).name == client.name){
                break;
            }
        }
        ASSERT(iter != this->connectedClients.end(), "Got unexpected iter in handle exit");
        this->connectedClients.erase(iter);               // rm from clients
        for (auto &group : this->groups){
            auto iter2 = group.second.begin();    // iterate over group name vector (in second)
            for (iter2 ; iter2 != group.second.end(); ++iter2) {
                if ((*iter2).name == client.name){
                    break;
                }
                // Here, iter2 may be .end because we cant be sure if client in group
            }

            if (iter2 != group.second.end()){
                group.second.erase(iter2);                // rm from group
            }
            if (group.second.size() == 1){                  // last one in a group is a rotten egg
                // todo: is a group of only one member considered valid after erasing?
            }
            if (group.second.empty()){                  // empty group is super rotten
                groups.erase(group.first);
            }

        }
    }
    return ErrorCode::NOT_IMPLEMENTED;
}

int Server::_getConnection() {
//    int t= accept(this->serverSocketClient,NULL, NULL); /* socket of connection */
    clilen = sizeof(cli_addr);
    int t = accept(this->serverSocketClient, (struct sockaddr *) &cli_addr, &clilen);

    if (t < 0){
        return -1;
    }
    return t;
}


ErrorCode Server::_run() {
//    struct timeval interval{0, 100};
    bool stillRunning = true;
    while (stillRunning){
        auto readfds = this->clientSocketSet;
        if (select(this->numOfActiveSockets+1, &readfds, NULL, NULL, NULL) < 0) {
            print_error("_run - select", 1);
            exit(-1);
        }
        if (FD_ISSET(this->serverSocketClient, &readfds)) {
            //will also add the client to the clientsfds

            this->_HandleNewClient();
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
        for (const auto &connectedClient : this->connectedClients) {
            //todo: for connectedClient to exit(1)
        }
        //TODO: exit gracefully... not like a baffoon
        exit(0);
    }

}

void Server::_HandleNewClient() {
    auto t = _getConnection();
    if (t < 0){
        print_error("_run - connectNewClient", 2);
        exit(-1);
    }
    std::string name;
    ASSERT_SUCCESS(_ParseName(t, name), "Parse name failed in _run");

    printf("New Client: %s", name);

    if (!(this->_isGroup(name) || this->_isClient(name))){
        connectedClients.emplace_back(clientWrapper{t, name});
        this->numOfActiveSockets ++; //increment the number of clients
        FD_SET(t, &this->clientSocketSet); //todo: remove?
        print_connection_server(name); // successful
        // todo: force client to print connection
    } else {  // this name is already in use
        //todo: force client to call print_dup_connection and exit
        //todo: who?
    }



}

bool Server::_isClient(const std::string& name) const{
    for (const auto &connectedClient : this->connectedClients) {
        if (connectedClient.name == name){ // found client that matches
            return true;
        }
    }
    return false;
}

bool Server::_isClientList(const std::vector<std::string>& names) const{
    for (const auto &name : names) {
        if (!this->_isClient(name)){ // found client that matches
            return false;
        }
    }
    return true;
}

const clientWrapper Server::_getClient(const std::string &name) const{
    for (const auto &client : this->connectedClients) {
        if (client.name == name){
            return client;
        }
    }
    return clientWrapper{NULL, nullptr};
}

ErrorCode Server::_HandleIncomingMessage(int socket) {
//    _ParseMessage(socket);
    return FAIL;
}

bool Server::_isGroup(const std::string &name) const {
    for (const auto &group : this->groups) {
        if (group.first == name){
            return true;
        }
    }
    return false;
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
    server._run();

}