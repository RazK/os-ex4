//
// Created by heimy4prez on 6/16/18.
//

#ifndef OSEX4_MESSAGES_H
#define OSEX4_MESSAGES_H

#include <string>
#include <vector>
#include <map>


#define PRINT_MSG(Message) printf(MESSAGES[Message])


enum class Message{
    SEND_ERROR,
    SEND_SUCCESS,
    GROUP_ERROR,
    GROUP_SUCCESS


};



const std::map<const Message, const char *> MESSAGES{
        (Message::SEND_ERROR, ""),
        (Message::SEND_SUCCESS, ""),
        (Message::GROUP_ERROR, ""),
        (Message::GROUP_SUCCESS, "")


};

#endif //OSEX4_MESSAGES_H
