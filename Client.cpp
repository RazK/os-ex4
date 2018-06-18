//
// Created by heimy4prez on 6/16/18.
//

#include <unistd.h>
#include <fcntl.h>
#include "Client.h"


#include <fstream>
#include <boost/algorithm/string/join.hpp>

//Client::Client() {
//    char * addr{"aqua-81"};
//    int port = 8080;
//    if (ErrorCode::SUCCESS != this->_callSocket(addr, port)){
//        printf("Err in socket calling\n");
//        return ;
//    }
//    write(this->s, "finally, a message", 100);
//    this->_readData(1024);
//
//    printf("Client read: %serverSocketClient", this->buf);
//
//    // TODO: REMOVE THESE LINES
//    this->_sock_fd = open("/cs/+/usr/razkarl/os-ex4/client_test.txt", O_CREAT | O_RDWR | O_NONBLOCK);
//    if (this->_sock_fd == -1)
//    {
//        perror("open failed");
//    }
//    this->_create_group_fd = open("/cs/+/usr/razkarl/os-ex4/create_group.txt", O_CREAT | O_RDWR |
//                                                                               O_NONBLOCK);
//    if (this->_create_group_fd == -1)
//    {
//        perror("open create_group.txt failed");
//    }
//
//    this->_send_fd = open("/cs/+/usr/razkarl/os-ex4/send.txt", O_CREAT | O_RDWR | O_NONBLOCK);
//    if (this->_send_fd == -1)
//    {
//        perror("open send.txt failed");
//    }
//
//    this->_who_fd = open("/cs/+/usr/razkarl/os-ex4/who.txt", O_CREAT | O_RDWR | O_NONBLOCK);
//    if (this->_who_fd == -1)
//    {
//        perror("open who.txt failed");
//    }
//
//    this->_exit_fd = open("/cs/+/usr/razkarl/os-ex4/exit.txt", O_CREAT | O_RDWR | O_NONBLOCK);
//    if (this->_exit_fd == -1)
//    {
//        perror("open exit.txt failed");
//    }
//}
Client::~Client() {}

/* wrapper for short circuit protection against undefined behavior - removes last comma <,> in string */
bool removeLastComma(std::string& str){ str.pop_back(); return true;}

ErrorCode Client::_ClientStdInput(){
    // Read until \n from STDIN
    std::string command;
    std::getline (std::cin,command);

    // Parse command
    command_type commandT; /*OUT*/
    std::string name;  /*OUT*/
    std::string message;  /*OUT*/
    std::vector<std::string> clients; /*OUT*/
    parse_command(command, commandT, name, message, clients);

    // Handle accordingly
    switch (commandT){
        case command_type::CREATE_GROUP:
        {
            const auto &groupName = name;                           // reference for clarity

            // sort clients and make unique - to make sure really is within legal bounds (repeating names?)
            std::sort(clients.begin(), clients.end());
            auto it = std::unique(clients.begin(), clients.end());
            clients.resize((unsigned long)std::distance(clients.begin(), it));

            // Organize all validity term client can recognize under one bool value - valid
            auto valid = (  (groupName != this->name) &&            // group name is unique from my name
                            (!clients.empty()) &&                   // group has at least 1 member (may be me...)
                            (clients.size() <= WA_MAX_GROUP)        // group length doesnt exceed max (50)
                            );

            // Merge clients list into single string
            std::string clientsJoined;
            for (const auto &client : clients){
                valid =     (isValidName(client) &&                         //legal name by alphanumeric and length
                            (client != groupName) &&                        //This member-client's name isn't group name
                            (client != this->name || clients.size() > 1) && //If this client in list then list is at least 2 long
                            (valid));                                       //Don't erase validity so far for all members
                clientsJoined.append(client);
                clientsJoined.append(",");
            }

            valid = ( valid &&                                                          // short circuit
                    (removeLastComma(clientsJoined)) &&                                 // see func docs
                    (ErrorCode::SUCCESS == _RequestCreateGroup(name, clientsJoined)) && // succeeded to request
                    (ErrorCode::SUCCESS == this->_readTaskResponse())                   // got back positive response
            );

            print_create_group(false, valid, this->name, groupName);
            return ErrorCode::SUCCESS;
        }

        case command_type::SEND:
        {
            // Send protocol SEND_MESSAGE message
            auto valid =  (ErrorCode::SUCCESS == this->_RequestSendMessage(name, message) &&
                            ErrorCode::SUCCESS == this->_readTaskResponse()
                    );
            print_send(false, valid, this->name, name, message);
            return ErrorCode::SUCCESS;
        }
        case command_type::EXIT:
        {
            // Send protocol EXIT message
            auto errCode = this->_RequestExit();
            this->_cleanUp();
            print_exit(false, this->name);
            auto exitCode = 0;
            if (errCode != ErrorCode::SUCCESS){
                exitCode = 1;
            }
            exit(exitCode);
        }
        case command_type::WHO:
        {
            // Send protocol WHO message
            return this->_RequestWho();
        }
        default:
        {
            print_invalid_input();
            return ErrorCode::FAIL;
        }
    }
}

ErrorCode Client::_ParseMessageFromServer(){
    char message[WA_MAX_MESSAGE+1];
    bzero(message, WA_MAX_MESSAGE+1);
    int read = _readData(this->connectedServer, message, WA_MAX_MESSAGE);
    if (read == 0){
        printf("Server disconnected unexpectedly.\r\n");
        exit(1);
        return ErrorCode::FAIL;
    }
    else if (read != WA_MAX_MESSAGE){
        printf("Read from server socket failed.\r\n");
        exit(-1);
    }

    printf("%s\r\n", message);
    return ErrorCode::SUCCESS;
}

ErrorCode Client::_callSocket(const char *hostname, unsigned short port) {
//    printf("HOST %connectedServer \n", hostname);
    this->host= gethostbyname (hostname);
    if (this->host == nullptr) {
        printf("failed to get host by name");
        return ErrorCode::FAIL;
    }
    memset(&serv_addr,0,sizeof(serv_addr));
    memcpy((char *)&this->serv_addr.sin_addr , this->host->h_addr , this->host->h_length);
    this->serv_addr.sin_family = this->host->h_addrtype;
    this->serv_addr.sin_port = htons((u_short)port);

    this->connectedServer = socket(this->host->h_addrtype, SOCK_STREAM, 0);
    if (this->connectedServer < 0) {
        printf("client failed in socket\n");
        return ErrorCode::FAIL;
    }

    if (connect(this->connectedServer, (struct sockaddr *)&this->serv_addr , sizeof(this->serv_addr)) < 0) {
        close(this->connectedServer);
        printf("Failed to connect to server. (Is server running? Correct address and port?)\n\r");

        return ErrorCode::FAIL;
    }

    ASSERT_SUCCESS(_TellName(this->name), "Failed in tellname in connection");

    auto responseCode = this->_readTaskResponse();
    if (responseCode == ErrorCode::SUCCESS){
        print_connection();
    }
    else if (responseCode == ErrorCode::FAIL){
        print_fail_connection();
        exit(1);
    }
    ASSERT_SUCCESS(responseCode, ("Bug: Client got unexpected response from server while connecting."));
    return responseCode;
}


ErrorCode Client::_readTaskResponse() const{
    char response [TASK_RESP_SIZE];
    bzero(response, TASK_RESP_SIZE);
    _readData(this->connectedServer, &response, TASK_RESP_SIZE);
//    ASSERT_READ(this->connectedServer, response, TASK_RESP_SIZE);

    std::string temp {response};

    if (TASK_SUCCESSFUL == temp){
        return ErrorCode::SUCCESS;
    }
    else if (TASK_FAILURE == temp){
        return ErrorCode::FAIL;
    }
    return ErrorCode::BUG;

}



ErrorCode Client::_TellName(const std::string& myName){
    // Assert valid name
    ASSERT(isValidName(myName), ("Tried to tell invalid name %s", myName));

    // Pad name with zeros up to WA_MAX_NAME
    std::string maxName(myName);
    maxName.resize(WA_MAX_NAME, 0);
    const char* cstr = maxName.c_str();

    // Send padded name to host
    ASSERT_WRITE(this->connectedServer, cstr, WA_MAX_NAME);

    return ErrorCode::SUCCESS;
}


ErrorCode Client::_RequestCreateGroup(const std::string& groupName,
                                      const std::string& listOfClientNames)
{
    // Reality check - case should have already been caught
    ASSERT((0 <= listOfClientNames.length() &&
            listOfClientNames.length() <= WA_MAX_INPUT),
           "Invalid clients list length");

    // Init empty message
    CreateGroupMessage msg; // msg_type = CREATE_GROUP

    // Build message
    msg.nameLen = (name_len) groupName.length();
    msg.groupName = groupName.c_str();
    msg.clientsLen = htonl((uint32_t)listOfClientNames.length()); //todo: raz- check validity of converting to uint32 not uint64, as defined in protocol
    msg.clientNames = listOfClientNames.c_str();

    // Send message
    ASSERT_WRITE(this->connectedServer, &msg.mtype, sizeof(msg.mtype));
    ASSERT_WRITE(this->connectedServer, &msg.nameLen, sizeof(msg.nameLen));
    ASSERT_WRITE(this->connectedServer, msg.groupName, groupName.length());
    ASSERT_WRITE(this->connectedServer, &msg.clientsLen, sizeof(msg.clientsLen));
    ASSERT_WRITE(this->connectedServer, msg.clientNames, listOfClientNames.length());

    return ErrorCode::SUCCESS;
}
ErrorCode Client::_RequestSendMessage(const std::string& targetName, const std::string& message) const
{
    // Validate arguments
    // Assert valid name
    ASSERT(isValidName(targetName), ("Attempted to send message to target with invalid "
            "characters %s",
            targetName));
    ASSERT((0 <= targetName.length() &&
            targetName.length() <= WA_MAX_NAME), "Invalid target name length");
    ASSERT((0 <= message.length() &&
            message.length() <= WA_MAX_MESSAGE),
           "Invalid message length");

    // Init empty message
    SendMessage msg; // msg_type = SEND

    // Build message
    msg.nameLen = targetName.length();
    msg.targetName = targetName.c_str();
    msg.messageLen = htons((uint16_t)message.length());
    msg.msg = message.c_str();

    // Send message
    ASSERT_WRITE(this->connectedServer, &msg.mtype, sizeof(msg.mtype));
    ASSERT_WRITE(this->connectedServer, &msg.nameLen, sizeof(msg.nameLen));
    ASSERT_WRITE(this->connectedServer, msg.targetName, targetName.length());
    ASSERT_WRITE(this->connectedServer, &msg.messageLen, sizeof(msg.messageLen));
    ASSERT_WRITE(this->connectedServer, msg.msg, message.length());

    return ErrorCode::SUCCESS;
}
ErrorCode Client::_RequestWho() const
{
    // Init empty message
    WhoMessage msg; // msg_type = WHO

    // Send message
    ASSERT_WRITE(this->connectedServer, &msg.mtype, sizeof(msg.mtype));

    return ErrorCode::SUCCESS;
}
ErrorCode Client::_RequestExit() const
{
    // Init empty message
    ExitMessage msg; // msg_type = Exit

    // Send message
    ASSERT_WRITE(this->connectedServer, &msg.mtype, sizeof(msg.mtype));

    return ErrorCode::SUCCESS;
}

int Client::_configFDSets(){
    // Zero the set of sockets
    FD_ZERO(&this->openSocketsSet);

    // Init the set of sockets
    FD_SET(STDIN_FILENO, &this->openSocketsSet);                   // STDIN
    FD_SET(this->connectedServer, &this->openSocketsSet);       // New connections socket

    return max(STDIN_FILENO, this->connectedServer);
}

ErrorCode Client::_Run() {

    while (true){
        int max_fd = this->_configFDSets();
        auto readfds = this->openSocketsSet; //TODO: need to make a copy here..
        if (select(max_fd+1, &readfds, nullptr, nullptr, nullptr) < 0) {
            print_error("_Run - select", 1);
            exit(-1);
        }
        if (FD_ISSET(this->connectedServer, &readfds)) {
            //will also add the client to the clientsfds
            this->_ParseMessageFromServer();
            continue;
        }
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            this->_ClientStdInput();
            continue;
        }
    }
    return FAIL;
}

Client::Client(const std::string clientName, const std::string serverAddress, const unsigned short serverPort) {
    this->name = clientName;

    if (ErrorCode::SUCCESS != this->_callSocket(serverAddress.c_str(), serverPort)){
        printf("Client failed to open socket with the server.\n");
        exit(-1);
    }
}

void Client::_cleanUp() {
    close(this->connectedServer);
}

int main(int argc, char const *argv[]){
    if (argc != 4){
        print_client_usage();
        exit(-1);
    }
    std::string name = argv[1];
    std::string addr = argv[2]; //TODO: inet to a   -- for ip adr

    unsigned short port =  std::stoi(argv[3], nullptr, 10); //todo unsigned short

    Client client{name, addr, port};


    client._Run();
}

