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

Client::Client() : _serverPort(), _serverAddress(){
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

ErrorCode Client::_RequestCreateGroup(const std::string& groupName,
                                      const std::string& listOfClientNames)
{
    // Validate arguments
    ASSERT((0 <= groupName.length() &&
            groupName.length() <= WA_MAX_NAME), "Invalid group name length");
    ASSERT((0 <= listOfClientNames.length() &&
            listOfClientNames.length() <= WA_MAX_MESSAGE
                                          - sizeof(command_type)
                                          - sizeof(name_len)
                                          - groupName.length()
                                          - sizeof(clients_len)),
           "Invalid clients list length");

    // Init empty message
    CreateGroupMessage msg; // type = CREATE_GROUP

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
            message.length() <= WA_MAX_MESSAGE
                                          - sizeof(command_type)
                                          - targetName.length()
                                          - sizeof(message_len)),
           "Invalid message length");

    // Init empty message
    SendMessage msg; // type = SEND

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
    WhoMessage msg; // type = WHO

    // Send message
    ASSERT_WRITE(_who_fd, &msg.mtype, sizeof(msg.mtype));

    return ErrorCode::SUCCESS;
}
ErrorCode Client::_RequestExist() const
{
    // Init empty message
    ExitMessage msg; // type = Exit

    // Send message
    ASSERT_WRITE(_exit_fd, &msg.mtype, sizeof(msg.mtype));

    return ErrorCode::SUCCESS;
}