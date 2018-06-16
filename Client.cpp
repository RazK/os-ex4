//
// Created by heimy4prez on 6/16/18.
//

#include <unistd.h>
#include <fcntl.h>
#include "Client.h"
#include "Protocol.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <netinet/in.h>

Client::Client() {
    char * addr{"aqua-81"};
    int port = 8080;
    if (ErrorCode::SUCCESS != this->_callSocket(addr, port)){
        printf("Err in socket calling\n");
        return ;
    }
    write(this->s, "finally, a message", 100);
    this->_readData(1024);

    printf("Client read: %s", this->buf);

    // TODO: REMOVE THESE LINES
    this->_sock_fd = open("/cs/+/usr/razkarl/os-ex4/client_test.txt", O_CREAT | O_RDWR | O_NONBLOCK);
    if (this->_sock_fd == -1)
    {
        perror("open failed");
    }

    this->_create_group_fd = open("/cs/+/usr/razkarl/os-ex4/create_group.txt", O_CREAT | O_RDWR |
                                                                              O_NONBLOCK);
    if (this->_sock_fd == -1)
    {
        perror("open create_group.txt failed");
    }

    this->_send_fd = open("/cs/+/usr/razkarl/os-ex4/send.txt", O_CREAT | O_RDWR | O_NONBLOCK);
    if (this->_sock_fd == -1)
    {
        perror("open send.txt failed");
    }

    this->_who_fd = open("/cs/+/usr/razkarl/os-ex4/who.txt", O_CREAT | O_RDWR | O_NONBLOCK);
    if (this->_sock_fd == -1)
    {
        perror("open who.txt failed");
    }

    this->_exit_fd = open("/cs/+/usr/razkarl/os-ex4/exit.txt", O_CREAT | O_RDWR | O_NONBLOCK);
    if (this->_sock_fd == -1)
    {
        perror("open exit.txt failed");
    }
}
Client::~Client() {}


ErrorCode Client::_callSocket(char *hostname, unsigned short port) {
    printf("HOST %s\n", hostname);
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
    return ErrorCode::SUCCESS;
}

int Client::_readData(int n) {
    int bcount;
    /* counts bytes read */
    ssize_t br;
    /* bytes read this pass */
    bcount = 0;
    br = 0;
    while (bcount < n) { /* loop until full buffer */
        br = read(this->s, this->buf, (size_t)n - bcount);
        if ((br > 0)){
            bcount += br;
            this->buf += br; //Todo: why add val to char *?
        }
        if (br < 1) {
            return(-1);
        }
    }
    return(bcount);
}

ErrorCode Client::_RequestCreateGroup(const std::string& groupName,
                                      const std::string& listOfClientNames)
{
    // Validate arguments
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
    msg.clientsLen = listOfClientNames.length(); //htonl(listOfClientNames.length());
    msg.clientNames = listOfClientNames.c_str();

    // Send message
    ASSERT_WRITE(_create_group_fd, &msg.mtype, sizeof(msg.mtype));
    ASSERT_WRITE(_create_group_fd, &msg.nameLen, sizeof(msg.nameLen));
    ASSERT_WRITE(_create_group_fd, msg.groupName, groupName.length());
    ASSERT_WRITE(_create_group_fd, &msg.clientsLen, sizeof(msg.clientsLen));
    ASSERT_WRITE(_create_group_fd, msg.clientNames, listOfClientNames.length());

    return ErrorCode::SUCCESS;
}
ErrorCode Client::_RequestSendMessage(const std::string& targetName, const std::string& message) const
{
    // Validate arguments
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
    msg.messageLen = htonl(message.length());
    msg.msg = message.c_str();

    // Send message
    ASSERT_WRITE(_send_fd, &msg.mtype, sizeof(msg.mtype));
    ASSERT_WRITE(_send_fd, &msg.nameLen, sizeof(msg.nameLen));
    ASSERT_WRITE(_send_fd, msg.targetName, targetName.length());
    ASSERT_WRITE(_send_fd, &msg.messageLen, sizeof(msg.messageLen));
    ASSERT_WRITE(_send_fd, msg.msg, message.length());

    return ErrorCode::SUCCESS;
}
ErrorCode Client::_RequestWho() const
{
    // Init empty message
    WhoMessage msg; // msg_type = WHO

    // Send message
    ASSERT_WRITE(_who_fd, &msg.mtype, sizeof(msg.mtype));

    return ErrorCode::SUCCESS;
}
ErrorCode Client::_RequestExist() const
{
    // Init empty message
    ExitMessage msg; // msg_type = Exit

    // Send message
    ASSERT_WRITE(_exit_fd, &msg.mtype, sizeof(msg.mtype));

    return ErrorCode::SUCCESS;
}

//int main(int argc, char const *argv[]){
//
//    Client client{};
//
//}

