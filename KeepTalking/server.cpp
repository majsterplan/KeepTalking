#include "server.h"

Server::Server()
{

}

Server::Server(unsigned short int port)
{
    this->port = port;
    this->listeningSocketDescriptor = -1;
    this->addressLength = sizeof(struct sockaddr_in);
}

bool Server::createListeningSocket()
{
    if((this->listeningSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return false;
    return true;
}

bool Server::bindListeningSocketToPort()
{
    this->serverAddress.sin_family = AF_INET;
    this->serverAddress.sin_port = htons(this->port);
    this->serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(&(this->serverAddress.sin_zero), '\0', 8);
    if(bind(this->listeningSocketDescriptor, (struct sockaddr *) &(this->serverAddress),
            sizeof(struct sockaddr)) == -1)
    {
        char yes = '1';
        if(setsockopt(this->listeningSocketDescriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
            return false;
        return true;
    }
    return true;
}

bool Server::startListening()
{
    if(listen(this->listeningSocketDescriptor, 10) == -1)
        return false;
    return true;
}

int Server::acceptPendingConnection()
{
    int newUserDescriptor;
    if((newUserDescriptor = accept(this->listeningSocketDescriptor, (struct sockaddr *) &(this->clientAddress),
                                   &(this->addressLength))) == -1)
        return -1;
    return newUserDescriptor;
}

void Server::closeListeningSocket()
{
    closesocket(this->listeningSocketDescriptor);
}
