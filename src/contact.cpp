#include "contact.h"

Contact::Contact(QString name, QString ip, quint16 port, QByteArray key, int id){
    mId = id;
    mName = name;
    mIp = ip;
    mPort = port;
    mKey = key;
}

Contact Contact::findById(int id){
    quint16 cPort;
    QString cId, cName, cIp;
    QByteArray cKey;
    QSettings settings;
    settings.beginGroup("Contacts");
    foreach(cId, settings.childGroups()){
        if(cId.toInt() == id){
            cName = settings.value(cId + "/name").toString();
            cIp = settings.value(cId + "/ip").toString();
            cPort = settings.value(cId + "/port").toUInt();
            cKey = settings.value(cId + "/key").toByteArray();
            break;
        }
    }
    settings.endGroup();
    return Contact(cName, cIp, cPort, cKey, cId.toInt());
}

Contact Contact::findByName(QString name){
    quint16 cPort;
    QString cId, cName, cIp;
    QByteArray cKey;
    QSettings settings;
    settings.beginGroup("Contacts");
    foreach(cId, settings.childGroups()){
        cName = settings.value(cId + "/name").toString();
        if(cName.compare(name) == 0){
            cIp = settings.value(cId + "/ip").toString();
            cPort = settings.value(cId + "/port").toUInt();
            cKey = settings.value(cId + "/key").toByteArray();
            break;
        }
    }
    settings.endGroup();
    return Contact(cName, cIp, cPort, cKey, cId.toInt());
}

Contact Contact::findByIp(QString ip){
    quint16 cPort;
    QString cId, cName, cIp;
    QByteArray cKey;
    QSettings settings;
    settings.beginGroup("Contacts");
    foreach(cId, settings.childGroups()){
        cIp = settings.value(cId + "/ip").toString();
        if(cIp.compare(ip) == 0){
            cName = settings.value(cId + "/name").toString();
            cPort = settings.value(cId + "/port").toUInt();
            cKey = settings.value(cId + "/key").toByteArray();
            break;
        }
    }
    settings.endGroup();
    return Contact(cName, cIp, cPort, cKey, cId.toInt());
}

Contact Contact::findByKey(QByteArray key){
    quint16 cPort;
    QString cId, cName, cIp;
    QByteArray cKey;
    QSettings settings;
    settings.beginGroup("Contacts");
    foreach(cId, settings.childGroups()){
        cKey = settings.value(cId + "/key").toByteArray();
        if(cKey == key){ //TODO: test use cases
            cName = settings.value(cId + "/name").toString();
            cIp = settings.value(cId + "/ip").toString();
            cPort = settings.value(cId + "/port").toUInt();
            break;
        }
    }
    settings.endGroup();
    return Contact(cName, cIp, cPort, cKey, cId.toInt());
}

QList<Contact> Contact::getContactList(){
    QSettings settings;
    QList<Contact> contactList;
    settings.beginGroup("Contacts");
    QStringList idList = settings.childGroups();
    foreach(QString id, idList)
        contactList.append(Contact::findById(id.toInt()));
    settings.endGroup();
    return contactList;
}

int Contact::getId() const{
    return mId;
}

QString Contact::getName() const{
    return mName;
}

QString Contact::getIp() const{
    return mIp;
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

void Contact::setIp(QString ip){
    mIp = ip;
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
    settings.setValue(id + "/ip", mIp);
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
