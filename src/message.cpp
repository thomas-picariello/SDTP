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

const QString Message::html(){
    QString html, align, color;

    switch(mType){
    case SENT:
        align = "right";
        color = "blue";
        break;
    case RECIEVED:
        align = "left";
        color = "green";
        break;
    case ERR:
        align = "center";
        color = "red";
        break;
    default:
        align = "center";
        color = "gray";
        break;
    }
    html.append("<div style='align:");
    html.append(align);
    html.append(";color:");
    html.append(color);
    html.append("'>");
    html.append(mText);
    html.append("</div>");
    qDebug()<<html;
    return html;
}
