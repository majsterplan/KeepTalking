#ifndef CONVERSATIONSMANAGER_H
#define CONVERSATIONSMANAGER_H

#include <QVector>
#include "conversation.h"

class Conversation;

class ConversationsManager
{
public:
    ConversationsManager();
    QVector<Conversation *> getConversations();
    void addConversation(QString name);
    Conversation * findConversationByName(QString name);

private:
    QVector<Conversation *> conversations;
};

#endif // CONVERSATIONSMANAGER_H
