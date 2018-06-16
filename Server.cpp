//
// Created by heimy4prez on 6/16/18.
//

#include "Server.h"

Server::Server(int port) {


}

ErrorCode Server::_establish(unsigned short port) {


    //hostnet initialization
    ASSERT_SUCCESS(gethostname(myname, WA_MAX_NAME), "gethostname failure in server establish connection");

    this->hp = gethostbyname(myname);

    ASSERT_NOT_NULL(this->hp, "got nullptr for gethostbyname in server establish connection", ErrorCode::FAIL);

    //sockaddrr_in initialization
    memset(this->&sa, 0, sizeof(struct sockaddr_in));
    this->sa.sin_family = this->hp->h_addrtype;

    /* this is our host address */
    memcpy(this->&sa.sin_addr, this->hp->h_addr, this->hp->h_length);

    /* this is our port number */
    this->sa.sin_port = htons(port);
}
