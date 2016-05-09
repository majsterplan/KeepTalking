#include "commandbuilder.h"

CommandBuilder::CommandBuilder()
{
}

QString CommandBuilder::build(QString code, QStringList parameteres)
{
    QString command;
    command.append(code);
    for(int i = 0; i < parameteres.size(); i++)
        command.append("#" + parameteres.at(i));
    command.append("\r\n");
    return command;
}
