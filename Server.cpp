//
// Created by heimy4prez on 6/16/18.
//

#include "Server.h"
#include "Protocol.h"

ErrorCode Server::_ParseMessage(int socket);

ErrorCode Server::_ParseCreateGroup(CreateGroupMessage& /* OUT */ msg) const
{
    std::string listOfClientNames;

    // Parse message
    ASSERT_READ(_create_group_fd, &msg.nameLen, sizeof(msg.nameLen));
    ASSERT((0 <= msg.nameLen &&
            msg.nameLen <= WA_MAX_NAME), "Invalid group name length");

    msg.groupName = new char[msg.nameLen];
    ASSERT_READ(_create_group_fd, msg.groupName, msg.nameLen);

    ASSERT_READ(_create_group_fd, &msg.clientsLen, sizeof(msg.clientsLen));
    ASSERT_READ(_create_group_fd, msg.clientNames, listOfClientNames.length());

    return ErrorCode::SUCCESS;
}
ErrorCode _ParseCreateGroup(CreateGroupMessage& /* OUT */ msg) const;
ErrorCode _ParseSendMessage(SendMessage& /* OUT */ msg) const;
ErrorCode _ParseWho(WhoMessage /* OUT */ msg) const;
ErrorCode _ParseExist(ExitMessage /* OUT */ msg) const;