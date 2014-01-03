#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>
#include <QDebug>

class Message
{
public:
    enum TYPE{SENT, RECIEVED, SERVICE, ERR, UNDEF};

    Message(QString text, TYPE type);

    const QString text();
    const TYPE type();
    const QString html();

private:
    QString mText;
    TYPE mType;
};

#endif // MESSAGE_H
