#include "usersmanager.h"

UsersManager::UsersManager()
{
    this->database = QSqlDatabase::addDatabase("QMYSQL");
    this->database.setHostName("localhost");
    this->database.setDatabaseName("keeptalking");
    this->database.setUserName("root");
    this->database.setPassword("");
}

QVector<User *> UsersManager::getUsers(bool loggedIn)
{
    QVector<User *> users;
    if(!loggedIn)
        users = this->users;
    else
    {
        for(int i = 0; i < this->users.size(); i++)
            if(this->users.at(i)->getLoggedIn() == loggedIn)
                users.append(this->users.at(i));
    }
    return users;
}

QSqlDatabase * UsersManager::getDatabase()
{
    return &(this->database);
}

void UsersManager::addUser(int descriptor)
{
    this->users.push_back(new User(descriptor));
}

void UsersManager::removeUser(int descriptor)
{
    for(int i = 0; i < this->users.size(); i++)
        if(this->users.at(i)->getDescriptor() == descriptor)
        {
            User *user = this->users.at(i);
            this->users.remove(i);
            delete user;
            break;
        }
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

QVector<User *> UsersManager::findUsersByName(QString name)
{
    QVector<User *> users;
    for(int i = 0; i < this->users.size(); i++)
    {
        if(this->users.at(i)->getName() == name)
            users.append(this->users.at(i));
    }
    return users;
}
