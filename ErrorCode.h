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

#define ASSERT(cond, msg)                   \
do {                                        \
    if (cond) {                             \
        std::cerr <<  (msg) << std::endl;   \
        exit(-1);                           \
    }                                       \
} while (0)

#define ASSERT_SUCCESS(f, msg)  ASSERT((ErrorCode::SUCCESS != (f)), msg)

#endif //OSEX4_ERRORCODE_H
