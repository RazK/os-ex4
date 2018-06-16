//
// Created by heimy4prez on 6/16/18.
//

#include "Client.h"

ErrorCode Client::_RequestCreateGroup(const std::string& groupName,
                              const std::vector<const std::string&>& listOfClientNames) const
{
    return ErrorCode::NOT_IMPLEMENTED;
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