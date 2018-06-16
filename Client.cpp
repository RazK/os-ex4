//
// Created by heimy4prez on 6/16/18.
//

#include "Client.h"
#include "Protocol.h"

ErrorCode Client::_RequestCreateGroup(const std::string& groupName,
                                      const std::string& listOfClientNames) const
{
    // Init empty message
    CreateGroupMessage msg; // type = CREATE_GROUP
    bzero(&msg, sizeof(msg));

    // Build message
    msg.nameLen = groupName.length();
    ASSERT(msg.nameLen <= WA_MAX_NAME);
    msg.groupName = groupName.c_str();
    msg.clientsLen = listOfClientNames.length();
    msg.clientNames = listOfClientNames.c_str();

    // Send message

}
ErrorCode Client::_RequestSendMessage(const std::string& targetName, const std::string& message) const
{
    return ErrorCode::NOT_IMPLEMENTED;
}
ErrorCode Client::_RequestWho() const
{
    return ErrorCode::NOT_IMPLEMENTED;
}
ErrorCode Client::_RequestExist() const
{
    return ErrorCode::NOT_IMPLEMENTED;
}