//
// Created by heimy4prez on 6/16/18.
//
//


#include "Server.h"

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
        print_error("gethostname", errno);
        exit(1);
    }

    this->host = gethostbyname(myname);
    if (this->host == nullptr){
        print_error("gethostbyname", h_errno);
        exit(1);
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
        print_error("socket", errno);
        return ErrorCode::FAIL;
    }

    // add that socket to the set for select
    FD_SET(this->welcomeClientsSocket, &this->openSocketsSet);
    FD_SET(STDIN_FILENO, &this->openSocketsSet);

    if (bind(this->welcomeClientsSocket , (struct sockaddr *)&(this->serve_addr) , sizeof(struct sockaddr_in)) < 0){
        print_error("bind", errno);
        return ErrorCode::FAIL;

    }


    /* max # of queued connects */
    if (listen(this->welcomeClientsSocket, maxNumConnected) < 0){
        print_error("listen", errno);
        return ErrorCode::FAIL;
    }

    return ErrorCode::SUCCESS;
}

ErrorCode Server::_closeConnection(int socket){
    FD_CLR(socket, &this->openSocketsSet);
    if (0 != close(socket)){
        print_error("close", errno);
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

    auto read = _readData(socket, &mtype, sizeof(msg_type));

    if (read < 0){
        print_error("read", errno);
        return ErrorCode::FAIL;
    }

    if(0 == read)
    {
        //printf("%s disconnected.\r\n", client.name.c_str()); // https://stackoverflow
        // .com/questions/2416944/can-read-function-on-a-connected-socket-return-zero-bytes
        return this->_HandleExit(client);
    }

    switch (mtype)
    {
        case command_type::CREATE_GROUP:
        {
            std::string groupName(WA_MAX_NAME, 0);
            std::string clientNamesList(WA_MAX_INPUT, 0);
            bool success = (SUCCESS == _ParseCreateGroup(client, groupName, clientNamesList) &&
                            SUCCESS == _HandleCreateGroup(client, groupName, clientNamesList));

            print_create_group(true, success, client.name, groupName);
            break;
        }
        case command_type::SEND:
        {
            std::string targetName(WA_MAX_NAME, 0);
            std::string message(WA_MAX_MESSAGE, 0);
            bool success = (SUCCESS == _ParseSendMessage(client, targetName, message) &&
                            SUCCESS == _HandleSendMessage(client, targetName, message));
            print_send(true, success, client.name, targetName, message);
            break;
        }
        case command_type::WHO:
        {
            bool success = (SUCCESS == _HandleWho(client));
            print_who_server(client.name);
            break;
        }
        case command_type::EXIT:
        {
            bool success = (SUCCESS == _HandleExit(client));
            print_exit(true, client.name);
            break;
        }
        default:
        {
            //print_error("Unrecognized message type\r\n");
            return ErrorCode::FAIL;
        }
    }
    return ErrorCode::SUCCESS;
}

ErrorCode Server::_ParseName(int client_sock, std::string& /* OUT */ clientName){
    std::string temp;  // = "" is redundant

    // Read Name
    if (WA_MAX_NAME != readFromSocket(client_sock, temp, WA_MAX_NAME)){
        return ErrorCode::FAIL;
    };

    // Resize up to '\0'
    int nameLen = strnlen(temp.c_str(), WA_MAX_NAME);
    clientName = temp;
    clientName.resize(nameLen);

    // Assert name is legal
    CHECK_N_RET(isValidName(clientName), "BUG: Invalid client name found in server - client must catch "
            "this first", ErrorCode::FAIL);

    return ErrorCode::SUCCESS;
}

ErrorCode Server::_ParseCreateGroup(const clientWrapper& client, std::string& /* OUT */ groupName,
                                    std::string& /* OUT */ listOfClientNames) const
{
    name_len nameLen;
    clients_len clientsLen;

    // Parse group-name length
    CHECK_READ_RET(client.sock, &nameLen, sizeof(name_len));

    // Assert length is legal
    HOPE((0 <= nameLen &&
            nameLen <= WA_MAX_NAME), "Invalid group name length");

    // Read group name
    CHECK_READ_RET(client.sock, &groupName[0], nameLen);
    groupName.resize(nameLen);

    // Parse client-names length
    CHECK_READ_RET(client.sock, &clientsLen, sizeof(clients_len));
    clientsLen = ntohl(clientsLen);

    // Assert length is legal
    HOPE((0 <= clientsLen &&
            clientsLen <= WA_MAX_MESSAGE),
           "Invalid clients list length");

    // Read client names
    CHECK_READ_RET(client.sock, &listOfClientNames[0], clientsLen);
    listOfClientNames.resize(clientsLen);

    return ErrorCode::SUCCESS;
}

ErrorCode Server::_ParseSendMessage(const clientWrapper& client,
                                    std::string& /* OUT */ targetName,
                                    std::string& /* OUT */ message) const {
    name_len nameLen;
    message_len messageLen;

    // Parse group-name length
    CHECK_READ_RET(client.sock, &nameLen, sizeof(name_len));

    // Assert length is legal
    HOPE((0 <= nameLen &&
            nameLen <= WA_MAX_NAME), "Invalid target name length");

    // Read group name
    CHECK_READ_RET(client.sock, &targetName[0], nameLen);
    targetName.resize(nameLen);

    // Parse client-names length
    CHECK_READ_RET(client.sock, &messageLen, sizeof(message_len));

    // Parse message length
    messageLen = ntohs(messageLen);

    // Assert length is legal
    HOPE((0 <= messageLen &&
            messageLen <= WA_MAX_MESSAGE),
           "Invalid message length");

    // Read client names
    CHECK_READ_RET(client.sock, &message[0], messageLen);
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
    ErrorCode status = ErrorCode::BUG;

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
    // TODO: Shimmy: this seems to reject valid groups, separate giant condition to several so
    // debuggable
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
        status = ErrorCode::SUCCESS;
    }
    else{
        status = ErrorCode::FAIL;
    }

    auto response = getResponse(bool2TCase(valid));
    CHECK_WRITE_RET(client.sock, response.c_str(), TASK_RESP_SIZE);

    return status; //invalid does not mean not success
}

bool Server::isClientInGroup(const clientWrapper &clientW, const std::string &groupName){
    auto target = std::find(this->groups[groupName].begin(), this->groups[groupName].end(),
                            clientW);
    return !(target == this->groups[groupName].end());
}

ErrorCode Server::_HandleSendMessage(const clientWrapper& sender,
                                     const std::string& targetName,
                                     const std::string& message)
{
    bool messageSent = false;
    bool valid = (
            isValidName(sender.name) &&                         // sender is legal name -- superfluous
            _isClient(sender.name) &&                           // sender is recognized -- superfluous
            isValidName(targetName) &&                          // target has legal name
            !message.empty() &&                                 // non empty message
            !(sender.name == targetName)                        // not messaging self
                                                                // TODO: valid message?
    );

    if (valid) {
        if (_isGroup(targetName)) { // Target is a GROUP
            // Make sure sender is in group
            if (!isClientInGroup(sender, targetName)) {
                valid = false;
            } else {
                // Send message to all group members (except self)
                messageSent = true;
                for (const auto &groupMember : this->groups[targetName]) {
                    if ((groupMember.name != sender.name)) {
                        auto temp = sender.name + ": ";
                        messageSent = messageSent && (SUCCESS == this->_flushToClient(groupMember,
                                                                                      (sender.name +
                                                                                       ": " +
                                                                                       message),
                                                                                      true));
                    }
                }
            }
        } else if (_isClient(targetName)) { // Target is a CLIENT
            const auto &targetClientW = this->_getClient(targetName);
            messageSent = (SUCCESS ==
                           this->_flushToClient(targetClientW, (sender.name + ": " + message),
                                                true));
        }
    }

    auto response = getResponse(bool2TCase(valid && messageSent));

    CHECK_WRITE_RET(sender.sock, response.c_str(), TASK_RESP_SIZE);
    return (messageSent && valid ? ErrorCode::SUCCESS : ErrorCode::FAIL);
}

ErrorCode Server::_flushToClient(const clientWrapper &client, const std::string &string, bool endl) const{
    std::string message = string;
    if (endl){
        message += "\r\n";
    }
    // resize to standard message size - 256 + 2 (\n\r)
    message.resize(WA_MAX_FLUSH, 0);

    CHECK_WRITE_RET(client.sock, message.c_str(), message.length());
    return ErrorCode::SUCCESS;

}


ErrorCode Server::_HandleWho(const clientWrapper & client)
{
    // prints are expected sorted
    std::sort(this->connectedClients.begin(), this->connectedClients.end(), compareClientWrapper);

    std::string allNames;
    for (auto &connectedClient : this->connectedClients) {
        allNames += connectedClient.name + ",";
    }
    allNames.pop_back();
    int i = 0;
    for (i ; i < ((long)allNames.length() - (long)WA_MAX_MESSAGE); i += WA_MAX_MESSAGE ){
        // write a slice of 256 at a time, w/o endline
        this->_flushToClient(client, allNames.substr((unsigned long)i, WA_MAX_MESSAGE ), false);
    }
    // once more for last slice, and send endline.
    this->_flushToClient(client, allNames.substr((unsigned long)i, WA_MAX_MESSAGE ), true);

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
            if (group.second.size() == 1 ||
                group.second.empty()){                  // empty group is super rotten
                groups.erase(group.first);
            }
        }
        return ErrorCode::SUCCESS;
    }
    return ErrorCode::BUG;
}

int Server::_getConnection() {
//    int newConnection= accept(this->welcomeClientsSocket,NULL, NULL); /* socket of connection */
    int newConnection = accept(this->welcomeClientsSocket, nullptr, nullptr);

    if (newConnection < 0){
        print_error("accept", newConnection);
        return -1;
    }
    return newConnection;
}


ErrorCode Server::_Run() {
    while (true){
        int max_fd = this->_configFDSets();
        auto readfds = this->openSocketsSet;
        if (select(max_fd+1, &readfds, nullptr, nullptr, nullptr) < 0) {
            print_error("select", errno);
        }
        if (FD_ISSET(this->welcomeClientsSocket, &readfds)) {
            //will also add the client to the clientsfds
            this->_HandleNewClient();
            continue;
        }
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            this->_serverStdInput();
            continue;
        }
        else {
            //will check each client if it’s serverSocketClient in readfds
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
        this->_cleanUp();
        exit(0);
    } else {
        print_invalid_input();
    }

}

void Server::_cleanUp(){
    for (const auto &connectedClient : this->connectedClients) {
        this->_flushToClient(connectedClient, EXIT_INDICATOR, false);
        close(connectedClient.sock);
    }
    close(this->welcomeClientsSocket);
}

ErrorCode Server::_HandleNewClient() {
    bool success = false;
    auto t = _getConnection();
    if (t < 0){
        //print_error("_Run - connectNewClient", 2);
        return ErrorCode::FAIL;
    }
    std::string name;
    success = (ErrorCode::SUCCESS == _ParseName(t, name));

    if (success) {
        if (!(this->_isGroup(name) || this->_isClient(name))) {
            connectedClients.emplace_back(clientWrapper{t, name});
            print_connection_server(name); // successful
            CHECK_WRITE_RET(t, TASK_SUCCESSFUL, TASK_RESP_SIZE);
        } else {  // this name is already in use
            CHECK_WRITE_RET(t, TASK_USED_NAME, TASK_RESP_SIZE);
        }
    } else {
        CHECK_WRITE_RET(t, TASK_FAILURE, TASK_RESP_SIZE);
    }
    return ErrorCode::SUCCESS;
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
    return clientWrapper{-1, ""};
}

ErrorCode Server::_HandleIncomingMessage(int socket) {
//    _ParseMessage(socket);
    return ErrorCode::FAIL;
} //todo: deprecated

bool Server::_isGroup(const std::string &name) const {
    for (const auto &group : this->groups) {
        if (group.first == name){
            return true;
        }
    }
    return false;
}

Server::~Server() {
    _cleanUp();

}


int main(int argc, char const *argv[]){
    // assert legal usage of a single parameter after prog name
    if (argc != 2){
        print_server_usage();
        exit(1);
    }

    // read port number and assert legal - ports positive and are bounded by ushort max
    size_t * idx;
    auto port = atoi(argv[1]);

    if (port < 0 or port > USHRT_MAX){
        print_server_usage();
        exit(1);
    }
    Server server{(unsigned short)port};
    print_connection();
    server._Run();

}