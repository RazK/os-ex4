//
// Created by heimy4prez on 6/16/18.
//

#include "Server.h"

int establish(unsigned short portnum) {
    char myname[WA_MAX_NAME+1];
    int s;
    struct sockaddr_in sa;
    struct hostent *hp;
//hostnet initialization
    gethostname(myname, WA_MAX_NAME);
    hp = gethostbyname(myname);
    if (hp == NULL)
        return(-1);
//sockaddrr_in initlization
    memset(&sa, 0, sizeof(struct sockaddr_in));
    sa.sin_family = hp->h_addrtype;
/* this is our host address */
    memcpy(&sa.sin_addr, hp->h_addr, hp->h_length);
/* this is our port number */
    sa.sin_port= htons(portnum);
