#include <QCoreApplication>
#include <QDebug>
#include <iostream>
#include <winsock.h>
#include "server.h"

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    // initialize WINSOCK
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cout << "WSAStartup zakonczone porazka..." << endl;
        return -1;
    }
    cout << "WSAStartup zakonczone sukcesem..." << endl;
    Server server(1025);
    cout << "Port 1025..." << endl;
    if(!server.createListeningSocket())
    {
        cout << "Problem z utworzeniem gniazda..." << endl;
        return -1;
    }
    cout << "Gniazdo utworzone..." << endl;
    if(!server.bindListeningSocketToPort())
    {
        cout << "Problem przypisania gniazda do portu..." << endl;
        return -1;
    }
    if(!server.bindListeningSocketToPort())
    {
        cout << "Problem przypisania gniazda do portu..." << endl;
        return -1;
    }
    cout << "Gniazdo przypisane do portu..." << endl;
    if(!server.startListening())
    {
        cout << "Problem z rozpoczęciem nasłuchiwania.." << endl;
        return -1;
    }
    cout << "Slucham..." << endl;
    while(true)
    {
        int newUserDescriptor;
        if((newUserDescriptor = server.acceptPendingConnection()) == -1)
            cout << "Problem akceptacji połączenia..." << endl;
        cout << "Mamy nowe polaczenie. Deskryptor nowego gniazda to " << newUserDescriptor << "..." << endl;
    }
    return a.exec();
}
