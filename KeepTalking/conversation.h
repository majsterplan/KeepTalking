#ifndef CONVERSATION_H
#define CONVERSATION_H

#include <QString>
#include <QVector>
#include "user.h"

class Conversation
{
public:
    Conversation();
    Conversation(QString name);
    void setName(QString name);
    QString getName();
    QVector<User *> getUsers();
    void addUser(User *user);
    bool isUserInConversation(int descriptor);

private:
    QString name;
    QVector<User *> users;
};

#endif // CONVERSATION_H
