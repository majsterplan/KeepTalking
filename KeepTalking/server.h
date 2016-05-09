#ifndef SERVER_H
#define SERVER_H

#include <winsock.h>
#include <QString>

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
    void closeSocket(int descriptor);
    int getListeningSocketDescriptor();
    fd_set getSocketsDescriptors();
    int getMaxSocketDescriptor();
    void addSocketDescriptor(int newSocketDescriptor);
    void deleteSocketDescriptor(int uselessSocketsDescriptor);
    int isSocketSet(int socketDescriptor, fd_set *socketsDescriptors);
    void checkReadableSockets(fd_set *socektsDescriptors);
    int readMessage(int descriptor);
    void sendMessage(QString message, int descriptor);
    QString getMessage();
    bool getIsMessage();

private:
    unsigned short int port;
    int listeningSocketDescriptor;
    fd_set socketsDescriptors;
    int maxSocketDescriptor;
    struct sockaddr_in serverAddress;
    struct sockaddr_in newClientAddress;
    QString tmpBuffer;
    bool isMessage;
};

#endif // SERVER_H
