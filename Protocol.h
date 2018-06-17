//
// Created by razkarl on 6/16/18.
//

#ifndef OSEX4_PROTOCOL_H
#define OSEX4_PROTOCOL_H

#include <cstdint>
#include "whatsappio.h"

#define max(a,b) ((a) >= (b) ? (a) : (b))

struct clientWrapper{
    int sock;
    std::string name;
};


/** 'CREATE_GROUP' message:
 * +----------+----------+------------+-------------+--------------+
 * |     1    |     1    |  name_len  |      4      |  clients_len |
 * +----------+----------+------------+-------------+--------------+
 * | msg_type | name_len | group_name | clients_len | client_names |
 * +----------+----------+------------+-------------+--------------+
*/

typedef uint8_t msg_type;
typedef uint8_t name_len;
typedef const char* group_name;
typedef uint64_t clients_len;
typedef const char* client_names;

typedef struct _CreateGroupMessage{
    msg_type mtype = command_type::CREATE_GROUP;
    name_len nameLen;
    group_name groupName;
    clients_len clientsLen;
    client_names clientNames;

    _CreateGroupMessage(){}

    _CreateGroupMessage(name_len nameLen,
    group_name groupName,
    clients_len clientsLen,
    client_names clientNames) : mtype(CREATE_GROUP), nameLen(nameLen), groupName(groupName),
                                clientsLen(clientsLen), clientNames(clientNames){}
} CreateGroupMessage;


/** 'SEND' Message:
 * +----------+----------+-------------+-------------+--------------+
 * |     1    |     1    |   name_len  |      2      |  message_len |
 * +----------+----------+-------------+-------------+--------------+
 * | msg_type | name_len | target_name | message_len |    message   |
 * +----------+----------+-------------+-------------+--------------+
*/

typedef const char* target_name;
typedef uint16_t message_len;
typedef const char* message;

typedef struct _SendMessage{
    msg_type mtype = command_type::SEND;
    name_len nameLen;
    target_name targetName;
    message_len messageLen;
    message msg;
} SendMessage;


/** 'WHO' Message:
 * +----------+
 * |     1    |
 * +----------+
 * | msg_type |
 * +----------+
*/

typedef struct _WhoMessage{
    msg_type mtype = command_type::WHO;
} WhoMessage;


/** 'EXIT' Message:
 * +----------+
 * |     1    |
 * +----------+
 * | msg_type |
 * +----------+
*/

typedef struct _ExitMessage{
    msg_type mtype = command_type::EXIT;
} ExitMessage;


bool isValidName(const std::string& name);

int _readData(int socket, void * buf , size_t n);
int readFromSocket(int socket, std::string &outbuf, int count);

void split(const std::string& string,
           const char& delim,
           std::vector<std::string> result/*OUT */ );

bool isValidList(const std::vector<std::string>& names);


#endif //OSEX4_PROTOCOL_H
