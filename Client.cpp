//
// Created by heimy4prez on 6/16/18.
//

#include "Client.h"


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


ErrorCode Client::_callSocket(const char *hostname, unsigned short port) {
//    printf("HOST %s \n", hostname);
    this->hp= gethostbyname (hostname);
    if (this->hp == nullptr) {
        printf("failed to get host by name");
        return ErrorCode::FAIL;
    }
    memset(&sa,0,sizeof(sa));
    memcpy((char *)&this->sa.sin_addr , this->hp->h_addr , this->hp->h_length);
    this->sa.sin_family = this->hp->h_addrtype;
    this->sa.sin_port = htons((u_short)port);

    this->s = socket(this->hp->h_addrtype, SOCK_STREAM, 0);
    if (this->s < 0) {
        printf("client failed in socket\n");
        return ErrorCode::FAIL;
    }

    if (connect(this->s, (struct sockaddr *)&this->sa , sizeof(this->sa)) < 0) {
        close(this->s);
        printf("client failed in connect\n");

        return ErrorCode::FAIL;
    }
    print_connection();

    ASSERT_SUCCESS(_TellName(this->name), "Failed in tellname in connection");
    return ErrorCode::SUCCESS;
}




ErrorCode Client::_TellName(const std::string& myName){
    // Assert valid name
    ASSERT(isValidName(myName), ("Tried to tell invalid name %s", myName));

    // Pad name with zeros up to WA_MAX_NAME
    std::string maxName(myName);
    maxName.resize(WA_MAX_NAME, 0);

    // Send padded name to host
    ASSERT_WRITE(this->s, &maxName, WA_MAX_NAME);

    return ErrorCode::SUCCESS;
}


ErrorCode Client::_RequestCreateGroup(const std::string& groupName,
                                      const std::string& listOfClientNames)
{
    // Validate arguments
    // Assert valid name
    ASSERT(isValidName(groupName), ("Attempted to create group with invalid characters %s",
            groupName));
    ASSERT((0 <= groupName.length() &&
            groupName.length() <= WA_MAX_NAME), "Invalid group name length");
    ASSERT((0 <= listOfClientNames.length() &&
            listOfClientNames.length() <= WA_MAX_INPUT),
           "Invalid clients list length");

    // Init empty message
    CreateGroupMessage msg; // msg_type = CREATE_GROUP

    // Build message
    msg.nameLen = groupName.length();
    msg.groupName = groupName.c_str();
    msg.clientsLen = htonl(listOfClientNames.length()); //htonl(listOfClientNames.length());
    msg.clientNames = listOfClientNames.c_str();

    // Send message
    ASSERT_WRITE(this->s, &msg.mtype, sizeof(msg.mtype));
    ASSERT_WRITE(this->s, &msg.nameLen, sizeof(msg.nameLen));
    ASSERT_WRITE(this->s, msg.groupName, groupName.length());
    ASSERT_WRITE(this->s, &msg.clientsLen, sizeof(msg.clientsLen));
    ASSERT_WRITE(this->s, msg.clientNames, listOfClientNames.length());

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
    msg.messageLen = htons(message.length());
    msg.msg = message.c_str();

    // Send message
    ASSERT_WRITE(this->s, &msg.mtype, sizeof(msg.mtype));
    ASSERT_WRITE(this->s, &msg.nameLen, sizeof(msg.nameLen));
    ASSERT_WRITE(this->s, msg.targetName, targetName.length());
    ASSERT_WRITE(this->s, &msg.messageLen, sizeof(msg.messageLen));
    ASSERT_WRITE(this->s, msg.msg, message.length());

    return ErrorCode::SUCCESS;
}
ErrorCode Client::_RequestWho() const
{
    // Init empty message
    WhoMessage msg; // msg_type = WHO

    // Send message
    ASSERT_WRITE(this->s, &msg.mtype, sizeof(msg.mtype));

    return ErrorCode::SUCCESS;
}
ErrorCode Client::_RequestExist() const
{
    // Init empty message
    ExitMessage msg; // msg_type = Exit

    // Send message
    ASSERT_WRITE(this->s, &msg.mtype, sizeof(msg.mtype));

    return ErrorCode::SUCCESS;
}

ErrorCode Client::_Run() {

    return FAIL;
}

Client::Client(const std::string clientName, const std::string serverAddress, const int serverPort) {
    this->name = clientName;

    if (ErrorCode::SUCCESS != this->_callSocket(serverAddress.c_str(), serverPort)){
        printf("Err in socket calling\n");
        return ;
    }


}

int main(int argc, char const *argv[]){
    if (argc != 4){
        print_client_usage();
        exit(-1);
    }
    std::string name = argv[1];
    std::string addr = argv[2]; //TODO: inet to a   -- for ip adr

    int port =  std::stoi(argv[3], nullptr, 10);

    Client client{name, addr, port};


    client._Run();
}

