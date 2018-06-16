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
    if (ErrorCode::SUCCESS != this->_establish(port)){
        print_fail_connection();
    }
    this->_get_connection();
    print_connection();
//    print_connection_server();
}

ErrorCode Server::_closeConnection(){
    close(this->s);
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

ErrorCode Server::_ParseName(int sock_fd, std::string& /* OUT */ clientName){
    // Read Name
    ASSERT_READ(sock_fd, &clientName[0], WA_MAX_NAME);

    // Resize up to '\0'
    int nameLen = strnlen(clientName.c_str(), WA_MAX_NAME);
    clientName.resize(nameLen);

    return ErrorCode::SUCCESS;
}

ErrorCode Server::_ParseCreateGroup(std::string& /* OUT */ groupName,
                                    std::string& /* OUT */ listOfClientNames) const
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


//
//int main(int argc, char const *argv[]){
//    printf("Opening Server\n");
//
//    Server server{};
//    printf("Closing Server\n");
//
//
//}