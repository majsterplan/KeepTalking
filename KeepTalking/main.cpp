#include <QCoreApplication>
#include <iostream>
#include <winsock.h>
#include "server.h"
#include "usersmanager.h"
#include "commandparser.h"
#include "commandbuilder.h"
#include <QDebug>

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    // initialize WINSOCK
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cout << "WSAStartup zakonczone porazka." << endl;
        return -1;
    }
    cout << "WSAStartup zakonczone sukcesem." << endl;
    Server server(1025);
    cout << "Port 1025..." << endl;
    if(!server.createListeningSocket())
    {
        cout << "Problem z utworzeniem gniazda." << endl;
        return -1;
    }
    cout << "Gniazdo utworzone." << endl;
    if(!server.bindListeningSocketToPort())
    {
        cout << "Problem przypisania gniazda do portu." << endl;
        return -1;
    }
    if(!server.bindListeningSocketToPort())
    {
        cout << "Problem przypisania gniazda do portu." << endl;
        return -1;
    }
    cout << "Gniazdo przypisane do portu." << endl;
    if(!server.startListening())
    {
        cout << "Problem z rozpoczęciem nasłuchiwania." << endl;
        return -1;
    }
    cout << "Slucham..." << endl;
    UsersManager usersManager;
    while(true)
    {
        fd_set socketsDescriptors = server.getSocketsDescriptors();
        server.checkReadableSockets(&socketsDescriptors);
        for(int descriptor = 0; descriptor <= server.getMaxSocketDescriptor(); descriptor++)
        {
            if(server.isSocketSet(descriptor, &socketsDescriptors) > 0)
            {
                if(descriptor == server.getListeningSocketDescriptor())
                {
                    int newSocketDescriptor;
                    if((newSocketDescriptor = server.acceptPendingConnection()) == -1)
                        cout << "Problem akceptacji połączenia." << endl;
                    else
                    {
                        server.addSocketDescriptor(newSocketDescriptor);
                        cout << "Mamy nowe polaczenie. Deskryptor nowego gniazda to " << newSocketDescriptor << "." << endl;
                    }
                }
                else
                {
                    int receivedBytes = 0;
                    receivedBytes = server.readMessage(descriptor);
                    if(receivedBytes == 0)
                    {
                        server.closeSocket(descriptor);
                        server.deleteSocketDescriptor(descriptor);
                        cout << "Gniazdo o deskryptorze " << descriptor << " odlaczone." << endl;
                    }
                    else if(receivedBytes == -1)
                        cout << "Problemy z odczytem." << endl;
                    else
                    {
                        if(!server.getIsMessage())
                        {
                            QString request = server.getMessage();
                            QString requestToShow = "";
                            QString requestCode = "";
                            QStringList requestParameters;
                            QString response = "";
                            QString responseToShow = "";
                            QString responseCode = "";
                            QStringList responseParameters;
                            if(request.endsWith("\r\n"))
                                requestToShow = request.mid(0, request.length() - 2);
                            else
                                requestToShow = request;
                            cout << descriptor << "(klient) mowi: " << requestToShow.toStdString() << "." << endl;
                            CommandParser commandParser;
                            CommandBuilder commandBuilder;
                            bool validCommand = commandParser.parse(request, requestCode, requestParameters);
                            if(!validCommand)
                            {
                                responseCode = "ERROR";
                                responseParameters.append("BLAD_KOMENDY");
                            }
                            else
                            {
                                if(requestCode == "POTRZASANIE")
                                {
                                    if(requestParameters.at(0).toInt() != CommandParser::getProtocolVersion())
                                        responseCode = "ODRZUCENIE";
                                    else
                                    {
                                        usersManager.addUser(descriptor);
                                        responseCode = "POTWIERDZENIE";
                                    }
                                }
                                else if(requestCode == "REJESTRACJA" || requestCode == "AUTORYZACJA")
                                {
                                    QSqlDatabase *database = usersManager.getDatabase();
                                    if(!database->isOpen())
                                        database->open();
                                    if(!database->isOpen())
                                    {
                                        responseCode = "ERROR";
                                        responseParameters.append("BLAD_BAZY_DANYCH");
                                    }
                                    else
                                    {
                                        User *user = usersManager.findUserByDescriptor(descriptor);
                                        if(user != NULL && !user->getLoggedIn())
                                        {
                                            if(requestCode == "REJESTRACJA")
                                            {
                                                RegistrationProgress progress = user->signup(requestParameters.at(0), requestParameters.at(1));
                                                responseCode = "ODP_REJESTRACJA";
                                                responseParameters.append(QString::number(static_cast<int>(progress)));
                                            }
                                            else
                                            {
                                                bool success = user->login(requestParameters.at(0), requestParameters.at(1));
                                                if(!success)
                                                    responseCode = "ODRZUCENIE";
                                                else
                                                    responseCode = "POTWIERDZENIE";
                                            }
                                        }
                                        else
                                        {
                                            responseCode = "ERROR";
                                            responseParameters.append("BLAD_UZYTKOWNIKA");
                                        }
                                        database->close();
                                    }
                                }
                                else if(requestCode == "ZMIANA_STATUSU")
                                {
                                    /*User *user = usersManager.findUserByDescriptor(descriptor);
                                    user->setStatus(static_cast<Status>(parameters.at(0).toInt()));
                                    QVector<User *> users = usersManager.getUsers();
                                    QStringList responseParameteres;
                                    for(int i = 0; i < users.size(); i++)
                                        responseParameteres.append({users.at(i)->getName(), QString::number(static_cast<int>(users.at(i)->getStatus()))});*/
                                }
                            }
                            response = commandBuilder.build(responseCode, responseParameters);
                            server.sendMessage(response, descriptor);
                            if(response.endsWith("\r\n"))
                                responseToShow = response.mid(0, response.length() - 2);
                            else
                                responseToShow = response;
                            cout << server.getListeningSocketDescriptor() << "(serwer) odpowiada: " << responseToShow.toStdString() << "." << endl;
                        }
                    }
                }
            }
        }
    }
    return a.exec();
}
