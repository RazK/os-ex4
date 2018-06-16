#include <iostream>
#include "whatsappio.h"
#include "Protocol.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    std::cout << "sizeof enum = " << sizeof(command_type) << std::endl;
    std::cout << "sizeof create_group_message = " << sizeof(CreateGroupMessage) << std::endl;
    std::cout << "create_group_message.type = " << (char)(CreateGroupMessage().mtype + '0') <<
                                                                                       std::endl;

    std::cout << "who.type = " << (char)(WhoMessage().mtype + '0') <<
              std::endl;
    return 0;
}