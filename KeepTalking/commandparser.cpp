#include "commandparser.h"

int CommandParser::protocolVersion = 1;
QMap<QString, QVector<QString>> CommandParser::commandsValidationData = {{"POTRZASANIE", {"int"}}, {"REJESTRACJA", {"QString", "QString"}}, {"AUTORYZACJA", {"QString", "QString"}}, {"WIADOMOSC", {"QString", "QString", "QString"}}, {"USTAW_STATUS", {"int"}}, {"KTO_W_KONWERSACJI", {"QString"}}, {"DOLACZ", {"QString"}}, {"ZREZYGNUJ", {"QString"}}, {"BYWAJ", {}}};

CommandParser::CommandParser()
{
}

bool CommandParser::parse(QString command, QString &code, QStringList &parameteres)
{
    if(command.endsWith("\r\n"))
        command = command.mid(0, command.length() - 2);
    QStringList dividedCommand = command.split("#");
    QString tmpCode = dividedCommand.at(0);
    if(CommandParser::commandsValidationData.find(tmpCode) == CommandParser::commandsValidationData.end())
        return false;
    code = tmpCode;
    QVector<QString> types = CommandParser::commandsValidationData.value(code);
    if(types.size() != dividedCommand.size() - 1)
        return false;
    for(int i = 0; i < types.size(); i++)
    {
        if(types.at(i) == "int")
        {
            bool validConversion;
            dividedCommand.at(1 + i).toInt(&validConversion);
            if(!validConversion)
                return false;
        }
        parameteres.append(dividedCommand.at(1 + i));
    }
    return true;
}

int CommandParser::getProtocolVersion()
{
    return CommandParser::protocolVersion;
}
