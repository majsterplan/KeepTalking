#ifndef USERSMANAGER_H
#define USERSMANAGER_H

#include <QVector>
#include <QSqlDatabase>
#include "user.h"

class UsersManager
{
public:
    UsersManager();
    QVector<User *> getUsers(bool loggedIn = false);
    QSqlDatabase * getDatabase();
    void addUser(int descriptor);
    void removeUser(int descriptor);
    User * findUserByDescriptor(int descriptor);
    QVector<User *> findUsersByName(QString name);

private:
    QVector<User *> users;
    QSqlDatabase database;
};

#endif // USERSMANAGER_H
