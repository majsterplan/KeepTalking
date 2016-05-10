#ifndef USER_H
#define USER_H

#include <QString>
#include <QSqlQuery>
#include <QRegularExpression>
#include <QCryptographicHash>
#include <QString>
#include <QVector>
#include "server.h"

enum RegistrationProgress
{
    AKCEPTUJE, LOGIN_UZYWANY, HASLO_ZA_PROSTE
};

enum Status
{
    DOSTEPNY, POGADAJMY, ZAJETY, ZARAZ_WRACAM, NIEWIDOCZNY, NIEDOSTEPNY
};

class User
{
public:
    User();
    User(int descriptor, QString name = "unnamed", Status status = NIEDOSTEPNY, bool loggedIn = false);
    void setName(QString name);
    QString getName();
    void setDescriptor(int descriptor);
    int getDescriptor();
    void setStatus(Status status);
    Status getStatus();
    void setLoggedIn(bool loggedIn);
    bool getLoggedIn();
    RegistrationProgress signup(QString login, QString password);
    bool login(QString login, QString password);
    bool changeStatus(Status status);
    void sendMessage(Server *server, QString message, QVector<int> descriptors);

private:
    QString name;
    int descriptor;
    Status status;
    bool loggedIn;
};

#endif // USER_H
