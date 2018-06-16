cmake_minimum_required(VERSION 3.8)
project(OSEx4)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp )
set(CLIENT_SOURCE_FILES Client.cpp Client.h ErrorCode.h)
set(SERVER_SOURCE_FILES Server.cpp Server.h ErrorCode.h)

add_executable(testMain ${SOURCE_FILES} )


add_executable(OSEx4 ${SOURCE_FILES} ${CLIENT_SOURCE_FILES} ${SERVER_SOURCE_FILES} cmake-build-debug/Protocol.cpp cmake-build-debug/Protocol.h)

add_executable(whatsappClient ${CLIENT_SOURCE_FILES} )

add_executable(whatsappServer ${SERVER_SOURCE_FILES})