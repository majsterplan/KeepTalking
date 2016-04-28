#ifndef SERVER_H
#define SERVER_H

#include <winsock.h>
#include <QDebug>

class Server
{
public:
    Server();
    Server(unsigned short int port);
    bool createListeningSocket();
    bool bindListeningSocketToPort();
    bool startListening();
    int acceptPendingConnection();
    void closeListeningSocket();

private:
    unsigned short int port;
    int listeningSocketDescriptor;
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    int addressLength;
};

#endif // SERVER_H
