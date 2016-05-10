#include "conversationsmanager.h"

ConversationsManager::ConversationsManager()
{

}

QVector<Conversation *> ConversationsManager::getConversations()
{
    return this->conversations;
}

void ConversationsManager::addConversation(QString name)
{
    this->conversations.push_back(new Conversation(name));
}

Conversation * ConversationsManager::findConversationByName(QString name)
{
    Conversation *conversation = NULL;
    for(int i = 0 ; i < this->conversations.size(); i++)
        if(this->conversations.at(i)->getName() == name)
            conversation = this->conversations.at(i);
    return conversation;
}
