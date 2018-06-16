//
// Created by heimy4prez on 6/16/18.
//

#include "Server.h"

Server::Server(int port) {

}

ErrorCode Server::_establish(unsigned short port) {
    char myname[WA_MAX_NAME + 1];
    int s;
    struct sockaddr_in sa;
    struct hostent *hp;

    //hostnet initialization
    if (ErrorCode::Success != gethostname(myname, WA_MAX_NAME)) {
        printf("yo");
    }
    hp = gethostbyname(myname);
    if (hp == nullptr) {
        return (ErrorCode::FAIL);
    }

    //sockaddrr_in initialization
    memset(&sa, 0, sizeof(struct sockaddr_in));
    sa.sin_family = hp->h_addrtype;

    /* this is our host address */
    memcpy(&sa.sin_addr, hp->h_addr, hp->h_length);

    /* this is our port number */
    sa.sin_port = htons(port);
}
