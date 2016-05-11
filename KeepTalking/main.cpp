#include <QCoreApplication>
#include <iostream>
#include <winsock.h>
#include "server.h"
#include "usersmanager.h"
#include "conversationsmanager.h"
#include "commandparser.h"
#include "commandbuilder.h"

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
    ConversationsManager conversationsManager;
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
                        User *user = usersManager.findUserByDescriptor(descriptor);
                        if(user != NULL)
                        {
                            if(user->getLoggedIn())
                                user->leaveAllConversations(&conversationsManager);
                            usersManager.removeUser(descriptor);
                            QString code = "LISTA_UZYTKOWNIKOW";
                            QStringList parameters;
                            QVector<User *> others = usersManager.getUsers(true);
                            parameters.append(QString::number(others.size()));
                            for(int i = 0; i < others.size(); i++)
                                parameters.append({others.at(i)->getName(), QString::number(static_cast<int>(others.at(i)->getStatus()))});
                            CommandBuilder commandBuilder;
                            QString messageToOthers = commandBuilder.build(code, parameters);
                            for(int i = 0; i < others.size(); i++)
                            {
                                server.sendMessage(messageToOthers, others.at(i)->getDescriptor());
                            }
                        }
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
                                        QString code = "LISTA_KONWERSACJI";
                                        QStringList parameters;
                                        QVector<Conversation *> conversations = conversationsManager.getConversations();
                                        parameters.append(QString::number(conversations.size()));
                                        for(int i = 0; i < conversations.size(); i ++)
                                            parameters.append(conversations.at(i)->getName());
                                        QString message = commandBuilder.build(code, parameters);
                                        server.sendMessage(message, descriptor);
                                        responseCode = "POTWIERDZENIE";
                                    }
                                }
                                else if(requestCode == "REJESTRACJA" || requestCode == "AUTORYZACJA")
                                {
                                    bool readyToContinue = false;
                                    QSqlDatabase *database = usersManager.getDatabase();
                                    if(!database->isOpen())
                                    {
                                        database->open();
                                        if(!database->isOpen())
                                        {
                                            responseCode = "ERROR";
                                            responseParameters.append("BLAD_BAZY_DANYCH");
                                        }
                                        else
                                            readyToContinue = true;
                                    }
                                    else
                                        readyToContinue = true;
                                    if(readyToContinue)
                                    {
                                        User *user = usersManager.findUserByDescriptor(descriptor);
                                        if(user != NULL)
                                        {
                                            if(!user->getLoggedIn())
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
                                                    {
                                                        QString code = "LISTA_UZYTKOWNIKOW";
                                                        QStringList parameters;
                                                        QVector<User *> others = usersManager.getUsers(true);
                                                        parameters.append(QString::number(others.size()));
                                                        for(int i = 0; i < others.size(); i++)
                                                            parameters.append({others.at(i)->getName(), QString::number(static_cast<int>(others.at(i)->getStatus()))});
                                                        QString messageToOthers = commandBuilder.build(code, parameters);
                                                        for(int i = 0; i < others.size(); i++)
                                                        {
                                                            server.sendMessage(messageToOthers, others.at(i)->getDescriptor());
                                                        }
                                                        responseCode = "POTWIERDZENIE";
                                                    }
                                                }
                                            }
                                            else
                                            {
                                                responseCode = "ERROR";
                                                responseParameters.append("BLAD_UZYTKOWNIKA");
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
                                else if(requestCode == "USTAW_STATUS")
                                {
                                    User *user = usersManager.findUserByDescriptor(descriptor);
                                    if(user != NULL)
                                    {
                                        if(user->getLoggedIn())
                                        {
                                            bool success = user->changeStatus(static_cast<Status>(requestParameters.at(0).toInt()));
                                            if(!success)
                                                responseCode = "ODRZUCENIE";
                                            else
                                            {
                                                QString code = "ZMIANA_STATUSU";
                                                QStringList parameters = {user->getName(), QString::number(static_cast<int>(user->getStatus()))};
                                                QString message = commandBuilder.build(code, parameters);
                                                QVector<User *> others = usersManager.getUsers(true);
                                                for(int i = 0; i < others.size(); i++)
                                                    server.sendMessage(message, others.at(i)->getDescriptor());
                                                responseCode = "POTWIERDZENIE";
                                            }
                                        }
                                        else
                                        {
                                            responseCode = "ERROR";
                                            responseParameters.append("BLAD_UZYTKOWNIKA");
                                        }
                                    }
                                    else
                                    {
                                        responseCode = "ERROR";
                                        responseParameters.append("BLAD_UZYTKOWNIKA");
                                    }
                                }
                                else if(requestCode == "WIADOMOSC")
                                {
                                    User *user = usersManager.findUserByDescriptor(descriptor);
                                    if(user != NULL)
                                    {
                                        if(user->getLoggedIn())
                                        {
                                            QString code = "WIADOMOSC";
                                            QStringList parameters;
                                            QString message;
                                            QVector<User *> receivers;
                                            if(requestParameters.at(1).isEmpty())
                                            {
                                                parameters.append({user->getName(), "", requestParameters.at(2)});
                                                message = commandBuilder.build(code, parameters);
                                                receivers = usersManager.findUsersByName(requestParameters.at(0));
                                            }
                                            else
                                            {
                                                Conversation *conversation = conversationsManager.findConversationByName(requestParameters.at(1));
                                                bool readyToContinue = false;
                                                if(conversation != NULL)
                                                {
                                                    bool userInConversation = conversation->isUserInConversation(user->getDescriptor());
                                                    if(!userInConversation)
                                                        responseCode = "ODRZUCENIE";
                                                    else
                                                        readyToContinue = true;
                                                }
                                                else
                                                {
                                                    conversationsManager.addConversation(requestParameters.at(1));
                                                    conversation = conversationsManager.findConversationByName(requestParameters.at(1));
                                                    conversation->addUser(user);
                                                    QString extraCode = "LISTA_KONWERSACJI";
                                                    QStringList extraParameters;
                                                    QVector<Conversation *> conversations = conversationsManager.getConversations();
                                                    extraParameters.append(QString::number(conversations.size()));
                                                    for(int i = 0; i < conversations.size(); i ++)
                                                        extraParameters.append(conversations.at(i)->getName());
                                                    QString extraMessage = commandBuilder.build(extraCode, extraParameters);
                                                    QVector<User *> others = usersManager.getUsers();
                                                    for(int i = 0; i < others.size(); i++)
                                                        server.sendMessage(extraMessage, others.at(i)->getDescriptor());
                                                    readyToContinue = true;
                                                }
                                                if(readyToContinue)
                                                {
                                                    parameters.append({user->getName(), conversation->getName(), requestParameters.at(2)});
                                                    message = commandBuilder.build(code, parameters);
                                                    receivers = usersManager.findUsersByName(requestParameters.at(0));
                                                    QVector<User *> newUsersInConversation = usersManager.findUsersByName(requestParameters.at(0));
                                                    for(int i = 0; i < newUsersInConversation.size(); i++)
                                                        conversation->addUser(newUsersInConversation.at(i));
                                                    receivers = conversation->getUsers();
                                                }
                                            }
                                            if(receivers.size() > 0)
                                            {
                                                QVector<int> descriptors;
                                                for(int i = 0; i < receivers.size(); i++)
                                                    if(receivers.at(i)->getDescriptor() != user->getDescriptor())
                                                        descriptors.append(receivers.at(i)->getDescriptor());
                                                user->sendMessage(&server, message, descriptors);
                                                responseCode = "POTWIERDZENIE";
                                            }
                                            else
                                                responseCode = "ODRZUCENIE";
                                        }
                                        else
                                        {
                                            responseCode = "ERROR";
                                            responseParameters.append("BLAD_UZYTKOWNIKA");
                                        }
                                    }
                                    else
                                    {
                                        responseCode = "ERROR";
                                        responseParameters.append("BLAD_UZYTKOWNIKA");
                                    }
                                }
                                else if(requestCode == "KTO_W_KONWERSACJI")
                                {
                                    Conversation *conversation = conversationsManager.findConversationByName(requestParameters.at(0));
                                    if(conversation != NULL)
                                    {
                                        responseCode = "LISTA_W_KONWERSACJI";
                                        QVector<User *> usersInConversation = conversation->getUsers();
                                        responseParameters.append(QString::number(usersInConversation.size()));
                                        for(int i = 0; i < usersInConversation.size(); i++)
                                            responseParameters.append(usersInConversation.at(i)->getName());
                                    }
                                    else
                                    {
                                        responseCode = "ERROR";
                                        responseParameters.append("BLAD_KONWERSACJI");
                                    }
                                }
                                else if(requestCode == "DOLACZ")
                                {
                                    Conversation *conversation = conversationsManager.findConversationByName(requestParameters.at(0));
                                    if(conversation != NULL)
                                    {
                                        User *user = usersManager.findUserByDescriptor(descriptor);
                                        if(user != NULL)
                                        {
                                            if(user->getLoggedIn())
                                            {
                                                bool success = user->joinConversation(conversation);
                                                if(success)
                                                    responseCode = "POTWIERDZENIE";
                                                else
                                                    responseCode = "ODRZUCENIE";
                                            }
                                            else
                                            {
                                                responseCode = "ERROR";
                                                responseParameters.append("BLAD_UZYTKOWNIKA");
                                            }
                                        }
                                        else
                                        {
                                            responseCode = "ERROR";
                                            responseParameters.append("BLAD_UZYTKOWNIKA");
                                        }
                                    }
                                    else
                                    {
                                        responseCode = "ERROR";
                                        responseParameters.append("BLAD_KONWERSACJI");
                                    }
                                }
                                else if(requestCode == "ZREZYGNUJ")
                                {
                                    Conversation *conversation = conversationsManager.findConversationByName(requestParameters.at(0));
                                    if(conversation != NULL)
                                    {
                                        User *user = usersManager.findUserByDescriptor(descriptor);
                                        if(user != NULL)
                                        {
                                            if(user->getLoggedIn())
                                            {
                                                bool success = user->leaveConversation(conversation);
                                                if(success)
                                                    responseCode = "POTWIERDZENIE";
                                                else
                                                    responseCode = "ODRZUCENIE";
                                            }
                                            else
                                            {
                                                responseCode = "ERROR";
                                                responseParameters.append("BLAD_UZYTKOWNIKA");
                                            }
                                        }
                                        else
                                        {
                                            responseCode = "ERROR";
                                            responseParameters.append("BLAD_UZYTKOWNIKA");
                                        }
                                    }
                                    else
                                    {
                                        responseCode = "ERROR";
                                        responseParameters.append("BLAD_KONWERSACJI");
                                    }
                                }
                                else if(requestCode == "BYWAJ")
                                {
                                    User *user = usersManager.findUserByDescriptor(descriptor);
                                    if(user != NULL)
                                    {
                                        if(user->getLoggedIn())
                                        {
                                            user->leaveAllConversations(&conversationsManager);
                                            user->logout();
                                        }
                                        else
                                        {
                                            responseCode = "ERROR";
                                            responseParameters.append("BLAD_UZYTKOWNIKA");
                                        }
                                    }
                                    else
                                    {
                                        responseCode = "ERROR";
                                        responseParameters.append("BLAD_UZYTKOWNIKA");
                                    }
                                }
                            }
                            response = commandBuilder.build(responseCode, responseParameters);
                            if(response != "\r\n")
                                server.sendMessage(response, descriptor);
                            if(response.endsWith("\r\n"))
                                responseToShow = response.mid(0, response.length() - 2);
                            else
                                responseToShow = response;
                            if(!responseToShow.isEmpty())
                                cout << server.getListeningSocketDescriptor() << "(serwer) odpowiada: " << responseToShow.toStdString() << "." << endl;
                        }
                    }
                }
            }
        }
    }
    return a.exec();
}
