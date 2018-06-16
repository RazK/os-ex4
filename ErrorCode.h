//
// Created by heimy4prez on 6/16/18.
//

#ifndef OSEX4_ERRORCODE_H
#define OSEX4_ERRORCODE_H

typedef enum _ErrorCode{
    FAIL = -1,
    SUCCESS = 0,
    NOT_IMPLEMENTED = 1
} ErrorCode;

#define ASSERTANDEXIT(cond, msg)            \
do {                                        \
    if (cond) {                             \
        std::cerr <<  (msg) << std::endl;   \
        exit(-1);                           \
    }                                       \
} while (0)

#define ASSERTANDRET(cond, msg, retVal)     \
do {                                        \
    if (cond) {                             \
        std::cerr <<  (msg) << std::endl;   \
        return retVal;                      \
    }                                       \
} while (0)

#define ASSERTANDDO(cond, f)                \
do {                                        \
    if (cond) {                             \
        return f();                         \
    }                                       \
} while (0)

#define ASSERTANDEXIT_SUCCESS(f, msg)       ASSERTANDEXIT((ErrorCode::SUCCESS != (f)), msg)
#define ASSERTANDRET_SUCCESS(f, msg)        ASSERTANDRET((ErrorCode::SUCCESS != (f)), msg, ErrorCode::FAIL)
#define ASSERTANDDO_SUCCESS(val, f)         ASSERTANDDO((ErrorCode::SUCCESS != (val)), f)


#define ASSERTANDRET_NOT_NULL(obj, msg)     ASSERTANDRET((nullptr != (obj)), msg, ErrorCode::FAIL)


#endif //OSEX4_ERRORCODE_H
