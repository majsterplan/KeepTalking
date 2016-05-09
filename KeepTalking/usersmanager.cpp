#include "usersmanager.h"

UsersManager::UsersManager()
{
    this->database = QSqlDatabase::addDatabase("QMYSQL");
    this->database.setHostName("localhost");
    this->database.setDatabaseName("keeptalking");
    this->database.setUserName("root");
    this->database.setPassword("");
}

QVector<User *> UsersManager::getUsers()
{
    return this->users;
}

QSqlDatabase * UsersManager::getDatabase()
{
    return &(this->database);
}

void UsersManager::addUser(int descriptor)
{
    this->users.push_back(new User(descriptor));
}

User * UsersManager::findUserByDescriptor(int descriptor)
{
    User *user = NULL;
    for(int i = 0; i < this->users.size(); i++)
        if(this->users.at(i)->getDescriptor() == descriptor)
        {
            user = this->users.at(i);
            break;
        }
    return user;
}
