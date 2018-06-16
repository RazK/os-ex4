//
// Created by heimy4prez on 6/16/18.
//

#include "Client.h"


ErrorCode Client::_callSocket(char *hostname, unsigned short port) {
    printf("HOST %s\n", hostname);
    this->hp= gethostbyname (hostname);
    if (this->hp == nullptr) {
        printf("failed to get host by name");
        return ErrorCode::FAIL;
    }
    memset(&sa,0,sizeof(sa));
    memcpy((char *)&this->sa.sin_addr , this->hp->h_addr , this->hp->h_length);
    this->sa.sin_family = this->hp->h_addrtype;
    this->sa.sin_port = htons((u_short)port);

    this->s = socket(this->hp->h_addrtype, SOCK_STREAM, 0);
    if (this->s < 0) {
        printf("client failed in socket\n");
        return ErrorCode::FAIL;
    }

    if (connect(this->s, (struct sockaddr *)&this->sa , sizeof(this->sa)) < 0) {
        close(this->s);
        printf("client failed in connect\n");

        return ErrorCode::FAIL;
    }
    return ErrorCode::SUCCESS;
}

int Client::_readData(int n) {
    int bcount;
    /* counts bytes read */
    ssize_t br;
    /* bytes read this pass */
    bcount = 0;
    br = 0;
    while (bcount < n) { /* loop until full buffer */
        br = read(this->s, this->buf, (size_t)n - bcount);
        if ((br > 0)){
            bcount += br;
            this->buf += br; //Todo: why add val to char *?
        }
        if (br < 1) {
            return(-1);
        }
    }
    return(bcount);
}

ErrorCode Client::_RequestCreateGroup(const std::string& groupName,
                              const std::vector<const std::string&>& listOfClientNames) const
{
    return ErrorCode::NOT_IMPLEMENTED;
}
ErrorCode Client::_RequestSendMessage(const std::string& targetName, const std::string& message) const
{
    return ErrorCode::NOT_IMPLEMENTED;
}
ErrorCode Client::_RequestWho() const
{
    return ErrorCode::NOT_IMPLEMENTED;
}
ErrorCode Client::_RequestExist() const
{
    return ErrorCode::NOT_IMPLEMENTED;
}

Client::Client():
_serverPort(0)

{
    char * addr{"aqua-81"};
    int port = 8080;
    if (ErrorCode::SUCCESS != this->_callSocket(addr, port)){
        printf("Err in socket calling\n");
        return ;
    }
    write(this->s, "finally, a message", 100);
    this->_readData(1024);

    printf("Client read: %s", this->buf);

}

int main(int argc, char const *argv[]){

    Client client{};

}

