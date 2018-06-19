//
// Created by heimy4prez on 6/16/18.
//

#ifndef OSEX4_ERRORCODE_H
#define OSEX4_ERRORCODE_H
#include <iostream>
#include "Protocol.h"

typedef enum _ErrorCode{
    FAIL = -1,
    SUCCESS = 0,
    NOT_IMPLEMENTED = 1,
    BUG = 2
} ErrorCode;

#ifdef DEBUG
    #define ASSERT(cond, msg)                       \
        do {                                        \
            if (!(cond)) {                          \
            std::cout << msg << std::endl;          \
            exit(1);                                \
            }                                       \
        } while (0)

    #define CHECK_N_RET(cond, msg, retVal)          \
        do {                                        \
            if (!(cond)) {                          \
                std::cout << msg << std::endl;      \
                return retVal;                      \
            }                                       \
        } while (0)

    #define CHECK_N_DO(cond, f)                     \
        do {                                        \
            if (!(cond)) {                          \
                return f();                         \
            }                                       \
        } while (0)

    #define HOPE(wish, farewell)                    \
        do {                                        \
            if(!(wish)){                              \
                std::cerr << farewell << std::endl; \
            }                                       \
        } while (0)

#else
    #define ASSERT(cond, msg)                       \
        do {                                        \
            if (!(cond)) {                          \
                exit(1);                            \
            }                                       \
        } while (0)

    #define CHECK_N_RET(cond, msg, retVal)          \
            do {                                    \
                if (!(cond)) {                      \
                    return retVal;                  \
                }                                   \
            } while (0)

    #define CHECK_N_DO(cond, f)                     \
            do {                                    \
                if (!(cond)) {                      \
                    return f();                     \
                }                                   \
            } while (0)

    #define HOPE(wish, farewell)                \
        do {                                    \
            if(!(wish)){                          \
            }                                   \
        }                                       \
        while (0)
#endif

#define CHECK_WRITE_RET(fd, buf, count)        CHECK_N_RET((count) == write(fd, buf, count), "Write error", ErrorCode::FAIL)
#define CHECK_READ_RET(fd, buf, count)         CHECK_N_RET((count) == _readData(fd, buf, count),"Read error", ErrorCode::FAIL)
//#define ASSERT_SUCCESS(f, msg)              ASSERT((ErrorCode::SUCCESS == (f)), msg)

#endif //OSEX4_ERRORCODE_H
