#include "contact.h"

Contact::Contact(QObject *parent):
    QObject(parent),
    mId(getNextAvailableID())
{
}

Contact::Contact(int id, QString name, QString host, quint16 port, QByteArray key, QObject *parent):
    QObject(parent),
    mId(id),
    mName(name),
    mHost(host),
    mPort(port),
    mKey(key)
{
}

int Contact::getId() const{
    return mId;
}

QString Contact::getName() const{
    return mName;
}

QString Contact::getHost() const{
    return mHost;
}

quint16 Contact::getPort() const{
    return mPort;
}

QByteArray Contact::getKey() const{
    return mKey;
}

void Contact::setId(int id){
    mId = id;
}

void Contact::setName(QString name){
    mName = name;
}

void Contact::setHost(QString host){
    mHost = host;
}

void Contact::setPort(quint16 port){
    mPort = port;
}

void Contact::setKey(QByteArray key){
    mKey = key;
}

void Contact::save(){
    QString id = QString::number(mId);
    QSettings settings;
    settings.beginGroup("Contacts");
    settings.setValue(id + "/name", mName);
    settings.setValue(id + "/host", mHost);
    settings.setValue(id + "/port", QString::number(mPort));
    settings.setValue(id + "/key", mKey);
    settings.endGroup();
}

void Contact::erase(){
    QSettings settings;
    settings.beginGroup("Contacts");
    if(settings.childGroups().contains(QString::number(mId)))
        settings.remove(QString::number(mId));
    settings.endGroup();
}

int Contact::getNextAvailableID(){
    QSettings settings;
    settings.beginGroup("Contacts");
    QStringList idList = settings.childGroups();
    int freeId = 0;
    while(idList.contains(QString::number(freeId))){
        freeId++;
    }
    return freeId;
    settings.endGroup();
}
