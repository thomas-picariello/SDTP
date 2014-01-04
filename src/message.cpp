#include "message.h"

Message::Message(QString text, TYPE type = UNDEF): mText(text), mType(type)
{
}

const QString Message::text(){
    return mText;
}

const Message::TYPE Message::type(){
    return mType;
}
