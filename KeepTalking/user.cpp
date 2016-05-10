#include "user.h"

User::User()
{

}

User::User(int descriptor, QString name, Status status, bool loggedIn)
{
    this->descriptor = descriptor;
    this->name = name;
    this->status = status;
    this->loggedIn = loggedIn;
}

void User::setName(QString name)
{
    this->name = name;
}

QString User::getName()
{
    return this->name;
}

void User::setDescriptor(int descriptor)
{
    this->descriptor = descriptor;
}

int User::getDescriptor()
{
    return this->descriptor;
}

void User::setStatus(Status status)
{
    this->status = status;
}

Status User::getStatus()
{
    return this->status;
}

void User::setLoggedIn(bool loggedIn)
{
    this->loggedIn = loggedIn;
}

bool User::getLoggedIn()
{
    return this->loggedIn;
}

RegistrationProgress User::signup(QString login, QString password)
{
    QSqlQuery checkUpLogin;
    checkUpLogin.exec("SELECT login FROM users WHERE login = '" + login + "'");
    if(checkUpLogin.next())
        return LOGIN_UZYWANY;
    QRegularExpression validPassword("^(?=.*\\d)(?=.*[A-Z]).{6,}$");
    if(!validPassword.match(password).hasMatch())
        return HASLO_ZA_PROSTE;
    QSqlQuery creationOfAccount;
    creationOfAccount.exec("INSERT INTO users(id, login, password) VALUES (NULL, '" + login + "', '" + QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex() + "')");
    return AKCEPTUJE;
}

bool User::login(QString login, QString password)
{
    QSqlQuery validationUserData;
    validationUserData.exec("SELECT login FROM users WHERE login = '" + login + "' AND password = '" + password + "'");
    if(!validationUserData.next())
        return false;
    this->name = validationUserData.value("login").toString();
    this->status = DOSTEPNY;
    this->loggedIn = true;
    return true;
}

bool User::changeStatus(Status status)
{
    if(!(status >= DOSTEPNY && status <= NIEDOSTEPNY))
       return false;
    this->status = status;
    return true;
}

void User::sendMessage(Server *server, QString message, QVector<int> descriptors)
{
    for(int i = 0; i < descriptors.size(); i++)
        server->sendMessage(message, descriptors.at(i));
}

bool User::joinConversation(Conversation *conversation)
{
    if(conversation->isUserInConversation(this->descriptor))
        return false;
    conversation->addUser(this);
    return true;
}

bool User::leaveConversation(Conversation *conversation)
{
    if(!conversation->isUserInConversation(this->descriptor))
        return false;
}
