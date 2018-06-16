#include <iostream>
#include "whatsappio.h"
#include "Protocol.h"
#include "Client.h"

int main() {
//    std::cout << "Hello, World!" << std::endl;
//    std::cout << "sizeof enum = " << sizeof(command_type) << std::endl;
//    std::cout << "sizeof create_group_message = " << sizeof(CreateGroupMessage) << std::endl;
//    std::cout << "create_group_message.type = " << (char)(CreateGroupMessage().mtype + '0') <<
//                                                                                       std::endl;
//    std::cout << "who.type = " << (char)(WhoMessage().mtype + '0') <<
//              std::endl;
//
//    Client testClient;
//    testClient._RequestCreateGroup("abcdef", "cyber,raz,shimmy");
//    testClient._RequestSendMessage("target", "hey target i'm raz");
//    testClient._RequestWho();
//    testClient._RequestExist();
    char* foo = "1234\00056789";
    std::string bar = "123\000123";
    std::cout<< foo << std::endl;
    std::cout << strlen(foo) << std::endl;
    std::cout<< bar << std::endl;

    return 0;
}