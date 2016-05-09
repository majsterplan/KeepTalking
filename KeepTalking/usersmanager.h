#ifndef USERSMANAGER_H
#define USERSMANAGER_H

#include <QVector>
#include <QSqlDatabase>
#include "user.h"

class UsersManager
{
public:
    UsersManager();
    QVector<User *> getUsers();
    QSqlDatabase * getDatabase();
    void addUser(int descriptor);
    User * findUserByDescriptor(int descriptor);

private:
    QVector<User *> users;
    QSqlDatabase database;
};

#endif // USERSMANAGER_H
