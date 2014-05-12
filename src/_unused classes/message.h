/**
 * @brief Message represent a message with a type.
 *
 * The type property should be used to properly display or treat the message.
 * Once created neither the content nor the type od the message can be changed.
 **/

#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>
#include <QDebug>

class Message
{
public:
    /**
     * @brief The TYPE enum lists the different message types
     */
    enum TYPE{SENT, RECIEVED, SERVICE, ERR, UNDEF};

    /**
     * @brief Default constructor of class, the text and the type of the message are set here.
     * @param text the text of the message
     * @param type the type of the message
     */
    Message(QString text, TYPE type);

    /**
     * @brief Returns the text of the message
     * @return the text of the message
     */
    const QString text();

    /**
     * @brief Returns the type of the message
     * @return the type of the message
     */
    const TYPE type();

private:
    QString mText;
    TYPE mType;
};

#endif // MESSAGE_H
