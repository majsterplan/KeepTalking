#ifndef COMMANDBUILDER_H
#define COMMANDBUILDER_H

#include <QString>
#include <QStringList>

class CommandBuilder
{
public:
    CommandBuilder();
    QString build(QString code, QStringList parameteres = {});
};

#endif // COMMANDBUILDER_H
