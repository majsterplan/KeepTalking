#include "conversation.h"

Conversation::Conversation()
{

}

Conversation::Conversation(QString name)
{
    this->name = name;
}

void Conversation::setName(QString name)
{
    this->name = name;
}

QString Conversation::getName()
{
    return this->name;
}

QVector<User *> Conversation::getUsers()
{
    return this->users;
}

void Conversation::addUser(User *user)
{
    this->users.push_back(user);
}

bool Conversation::isUserInConversation(int descriptor)
{
    bool userInConversation = false;
    for(int i = 0; i < this->users.size(); i++)
        if(this->users.at(i)->getDescriptor() == descriptor)
        {
            userInConversation = true;
            break;
        }
    return userInConversation;
}
