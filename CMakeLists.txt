cmake_minimum_required(VERSION 3.8)
project(OSEx4)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp )
set(CLIENT_SOURCE_FILES whatsappClient.cpp whatsappClient.hpp ErrorCode.hpp whatsappio.cpp whatsappio.hpp Protocol.hpp Protocol.cpp)
set(SERVER_SOURCE_FILES whatsappServer.cpp whatsappServer.hpp ErrorCode.hpp whatsappio.cpp whatsappio.hpp Protocol.hpp Protocol.cpp)

#add_executable(testMain ${SOURCE_FILES} )

#add_executable(OSEx4 ${SOURCE_FILES} ${CLIENT_SOURCE_FILES} ${SERVER_SOURCE_FILES} )

add_executable(whatsappClient ${CLIENT_SOURCE_FILES} )

add_executable(whatsappServer ${SERVER_SOURCE_FILES})