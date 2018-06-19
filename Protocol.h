//
// Created by razkarl on 6/16/18.
//

#ifndef OSEX4_PROTOCOL_H
#define OSEX4_PROTOCOL_H

#include <cstdint>
#include "whatsappio.h"
#include <unistd.h>
#include <sstream>

#define TASK_SUCCESSFUL  ("SUCC")
#define TASK_FAILURE     ("FAIL")
#define TASK_USED_NAME   ("USED")
#define TASK_BUG         ("BUGG")
#define TASK_UNDEFINED   ("UNDF")
#define TASK_EXIT        ("EXIT")
#define TASK_RESP_SIZE   (4)

const int WA_MAX_FLUSH = WA_MAX_NAME + 2 /* ": " */ + WA_MAX_MESSAGE + 2 /* "\r\n" */;

const char PAD = '#';

const std::string EXIT_INDICATOR = "EXITEXITEXITEXITEXITEXITEXITEXITEXITEXIT";


int max(int a, int b);

typedef enum _TaskCase{
    FAILED = -1,
    SUCCEEDED = 0,
    BUGGED = 2
} TaskCase;

struct clientWrapper{
    int sock;
    std::string name;
    bool operator==(const clientWrapper& other)const{ return this->name == other.name;};
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
    name_len nameLen{};
    group_name groupName{};
    clients_len clientsLen{};
    client_names clientNames{};

    _CreateGroupMessage() = default;

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
    name_len nameLen{};
    target_name targetName{};
    message_len messageLen{};
    message msg{};
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

/* Returns if name is legal:
 *  1. all alphanumeric
 *  2. length of at least 1
 *  3. doesnt exceed max length (30)
 *  */
bool isValidName(const std::string& name);

int _readData(int socket, void * buf , size_t n);
int readFromSocket(int socket, std::string &outbuf, int count);

void split(const std::string& string,
           const char& delim,
           std::vector<std::string>& result/*OUT */ );

bool isValidList(const std::vector<std::string>& names);

std::string padMessage(std::string string, int resultSize);

std::string getResponse(TaskCase taskCase);
TaskCase bool2TCase(bool valid);


#endif //OSEX4_PROTOCOL_H
