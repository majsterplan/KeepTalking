#include "server.h"

Server::Server()
{

}

Server::Server(unsigned short int port)
{
    this->isMessage = false;
    this->port = port;
    this->listeningSocketDescriptor = -1;
    this->maxSocketDescriptor = 0;
    FD_ZERO(&(this->socketsDescriptors));
}

bool Server::createListeningSocket()
{
    if((this->listeningSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return false;
    FD_SET(this->listeningSocketDescriptor, &(this->socketsDescriptors));
    this->maxSocketDescriptor = this->listeningSocketDescriptor;
    return true;
}

bool Server::bindListeningSocketToPort()
{
    this->serverAddress.sin_family = AF_INET;
    this->serverAddress.sin_port = htons(this->port);
    this->serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(&(this->serverAddress.sin_zero), '\0', 8);
    if(bind(this->listeningSocketDescriptor, (struct sockaddr *) &(this->serverAddress), sizeof(struct sockaddr)) == -1)
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
    int newUserDescriptor, addressLength;
    addressLength = sizeof(struct sockaddr_in);
    if((newUserDescriptor = accept(this->listeningSocketDescriptor, (struct sockaddr *) &(this->newClientAddress), &addressLength)) == -1)
        return -1;
    return newUserDescriptor;
}

void Server::closeListeningSocket()
{
    closesocket(this->listeningSocketDescriptor);
}

void Server::closeSocket(int descriptor)
{
    closesocket(descriptor);
}

int Server::getListeningSocketDescriptor()
{
    return this->listeningSocketDescriptor;
}

fd_set Server::getSocketsDescriptors()
{
    return this->socketsDescriptors;
}

int Server::getMaxSocketDescriptor()
{
    return this->maxSocketDescriptor;
}

void Server::addSocketDescriptor(int newSocketDescriptor)
{
    FD_SET(newSocketDescriptor, &(this->socketsDescriptors));
    if(newSocketDescriptor > this->maxSocketDescriptor)
        this->maxSocketDescriptor = newSocketDescriptor;
}

void Server::deleteSocketDescriptor(int uselessSocketsDescriptor)
{
    FD_CLR(uselessSocketsDescriptor, &(this->socketsDescriptors));
    if(this->socketsDescriptors.fd_count > 0)
    {
        this->maxSocketDescriptor = this->socketsDescriptors.fd_array[0];
        for(int i = 1; i < this->socketsDescriptors.fd_count; i++)
            if((int) this->socketsDescriptors.fd_array[i] > this->maxSocketDescriptor)
                this->maxSocketDescriptor = (int) this->socketsDescriptors.fd_array[i];
    }
    else
        this->maxSocketDescriptor = 0;
}

int Server::isSocketSet(int socketDescriptor, fd_set *socketsDescriptors)
{
    return FD_ISSET(socketDescriptor, socketsDescriptors);
}

void Server::checkReadableSockets(fd_set *socektsDescriptors)
{
    select(this->maxSocketDescriptor, socektsDescriptors, NULL, NULL, NULL);
}

int Server::readMessage(int descriptor)
{
    if(!this->isMessage)
        this->tmpBuffer.clear();
    this->isMessage = true;
    char buffer[256] = {0};
    int numberOfBytes;
    numberOfBytes = recv(descriptor, buffer, sizeof(buffer), 0);
    this->tmpBuffer.append(buffer);
    if(this->tmpBuffer.endsWith("\r\n"))
        this->isMessage = false;
    return numberOfBytes;
}

void Server::sendMessage(QString message, int descriptor)
{
    send(descriptor, message.toLocal8Bit().data(), message.length(), 0);
}

QString Server::getMessage()
{
    return this->tmpBuffer;
}

bool Server::getIsMessage()
{
    return this->isMessage;
}
