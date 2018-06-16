cmake_minimum_required(VERSION 3.8)
project(OSEx4)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp )
set(CLIENT_SOURCE_FILES Client.cpp Client.h ErrorCode.h whatsappio.cpp whatsappio.h Protocol.h Protocol.cpp)
set(SERVER_SOURCE_FILES Server.cpp Server.h ErrorCode.h whatsappio.cpp whatsappio.h Protocol.h Protocol.cpp)

#add_executable(testMain ${SOURCE_FILES} )

#add_executable(OSEx4 ${SOURCE_FILES} ${CLIENT_SOURCE_FILES} ${SERVER_SOURCE_FILES} )

add_executable(whatsappClient ${CLIENT_SOURCE_FILES} )

add_executable(whatsappServer ${SERVER_SOURCE_FILES})