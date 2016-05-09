#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H

#include <QMap>
#include <QVector>
#include <QString>
#include <QStringList>

class CommandParser
{
public:
    CommandParser();
    bool parse(QString command, QString &code, QStringList &parameteres);
    static int getProtocolVersion();

private:
    static int protocolVersion;
    static QMap<QString, QVector<QString>> commandsValidationData; // command code - parameteres types
};

#endif // COMMANDPARSER_H
