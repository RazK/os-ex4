//
// Created by razkarl on 6/16/18.
//

#ifndef OSEX4_PROTOCOL_H
#define OSEX4_PROTOCOL_H

#include <cstdint>
#include "whatsappio.h"

struct Protocol {
    command_type type;
    union {

    };
};

/** 'CREATE_GROUP' message:
 * +------+----------+------------+-------------+--------------+
 * |   4  |     1    |  name_len  |      4      |  clients_len |
 * +------+----------+------------+-------------+--------------+
 * | type | name_len | group_name | clients_len | client_names |
 * +------+----------+------------+-------------+--------------+
*/

typedef uint8_t name_len;
typedef const char* group_name;
typedef uint64_t clients_len;
typedef const char* client_names;

typedef struct _CreateGroupMessage{
    command_type type = command_type::CREATE_GROUP;
    name_len nameLen;
    group_name groupName;
    clients_len clientsLen;
    client_names clientNames;
} CreateGroupMessage;


/** 'SEND' Message:
 * +------+----------+-------------+-------------+--------------+
 * |   4  |     1    |   name_len  |      1      |  message_len |
 * +------+----------+-------------+-------------+--------------+
 * | type | name_len | target_name | message_len |    message   |
 * +------+----------+-------------+-------------+--------------+
*/

typedef const char* target_name;
typedef uint8_t message_len;
typedef const char* message;

typedef struct _SendMessage{
    command_type type = command_type::SEND;
    name_len nameLen;
    target_name targetName;
    message_len messageLen;
    message message;
} SendMessage;


/** 'WHO' Message:
 * +------+
 * |   4  |
 * +------+
 * | type |
 * +------+
*/

typedef struct _WhoMessage{
    command_type type = command_type::WHO;
} WhoMessage;


/** 'EXIT' Message:
 * +------+
 * |   4  |
 * +------+
 * | type |
 * +------+
*/

typedef struct _ExitMessage{
    command_type type = command_type::EXIT;
} ExitMessage;


#endif //OSEX4_PROTOCOL_H
