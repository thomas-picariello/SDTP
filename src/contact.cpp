#include "contact.h"

Contact::Contact(QObject *parent):
    QObject(parent),
    mId(0),
    mPort(0)
{}

Contact::Contact(int id, QString name, QStringList hostsList, quint16 port, QByteArray key, QObject *parent):
    QObject(parent),
    mId(id),
    mName(name),
    mHostsList(hostsList),
    mPort(port),
    mKey(key)
{}

QStringList Contact::getHostsList() const{
    return mHostsList;
}

uint Contact::getId() const{
    return mId;
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

QStringList* Contact::hostsList(){
    return &mHostsList;
}

void Contact::setHostsList(QStringList hostsList){
    mHostsList = hostsList;
}

void Contact::setId(uint id){
    mId = id;
}

void Contact::setKey(QByteArray key){
    mKey = key;
}

void Contact::setName(QString name){
    mName = name;
}

void Contact::setPort(quint16 port){
    mPort = port;
}
