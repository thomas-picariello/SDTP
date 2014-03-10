#include "contact.h"

Contact::Contact(QObject *parent):
    QObject(parent),
    mId(0),
    mPort(0)
{}

Contact::Contact(int id, QString name, QString host, quint16 port, QByteArray key, QObject *parent):
    QObject(parent),
    mId(id),
    mName(name),
    mHost(host),
    mPort(port),
    mKey(key)
{}

int Contact::getId() const{
    return mId;
}

QString Contact::getHost() const{
    return mHost;
}

QByteArray Contact::getKey() const{
    return mKey;
}

QString Contact::getName() const{
    return mName;
}

quint16 Contact::getPort() const{
    return mPort;
}

void Contact::setHost(QString host){
    mHost = host;
}

void Contact::setKey(QByteArray key){
    mKey = key;
}

void Contact::setId(int id){
    mId = id;
}

void Contact::setName(QString name){
    mName = name;
}

void Contact::setPort(quint16 port){
    mPort = port;
}
