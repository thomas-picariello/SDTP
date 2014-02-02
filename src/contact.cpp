#include "contact.h"

Contact::Contact(int id, QString name, QString hostName, QHostAddress ip, quint16 port, QByteArray key){
    mId = id;
    mName = name;
    mHostName = hostName;
    mIp = ip;
    mPort = port;
    mKey = key;
}

Contact* Contact::findById(int id){
    quint16 cPort;
    QString cId, cName, cHostName;
    QHostAddress cIp;
    QByteArray cKey;
    QSettings settings;
    settings.beginGroup("Contacts");
    foreach(cId, settings.childGroups()){
        if(cId.toInt() == id){
            cName = settings.value(cId + "/name").toString();
            cHostName = settings.value(cId + "/hname").toString();
            cIp = QHostAddress(settings.value(cId + "/ip").toString());
            cPort = settings.value(cId + "/port").toUInt();
            cKey = settings.value(cId + "/key").toByteArray();
            break;
        }
    }
    settings.endGroup();
    return new Contact(cId.toInt(),cName, cHostName, cIp, cPort, cKey);
}

QList<Contact*> Contact::findByName(QString name){
    QList<Contact*> matchList;
    QString cId, cName;
    QSettings settings;
    settings.beginGroup("Contacts");
    foreach(cId, settings.childGroups()){
        cName = settings.value(cId + "/name").toString();
        if(cName.compare(name) == 0){
            QString cHostName = settings.value(cId + "/hname").toString();
            QHostAddress cIp = QHostAddress(settings.value(cId + "/ip").toString());
            quint16 cPort = settings.value(cId + "/port").toUInt();
            QByteArray cKey = settings.value(cId + "/key").toByteArray();
            matchList.append(new Contact(cId.toInt(),cName, cHostName, cIp, cPort, cKey));
        }
    }
    settings.endGroup();
    return matchList;
}

QList<Contact*> Contact::findByIp(QHostAddress ip){
    QList<Contact*> matchList;
    QString cId;
    QHostAddress cIp;
    QSettings settings;
    settings.beginGroup("Contacts");
    foreach(cId, settings.childGroups()){
        cIp = QHostAddress(settings.value(cId + "/ip").toString());
        if(cIp == ip){
            QString cName = settings.value(cId + "/name").toString();
            QString cHostName = settings.value(cId + "/hname").toString();
            quint16 cPort = settings.value(cId + "/port").toUInt();
            QByteArray cKey = settings.value(cId + "/key").toByteArray();
            matchList.append(new Contact(cId.toInt(),cName, cHostName, cIp, cPort, cKey));
        }
    }
    settings.endGroup();
    return matchList;
}

Contact* Contact::findByKey(QByteArray key){
    quint16 cPort;
    QString cId, cName, cHostName;
    QHostAddress cIp;
    QByteArray cKey;
    QSettings settings;
    settings.beginGroup("Contacts");
    foreach(cId, settings.childGroups()){
        cKey = settings.value(cId + "/key").toByteArray();
        if(cKey == key){ //TODO: test
            cName = settings.value(cId + "/name").toString();
            cHostName = settings.value(cId + "/hname").toString();
            cIp = QHostAddress(settings.value(cId + "/ip").toString());
            cPort = settings.value(cId + "/port").toUInt();
            break;
        }
    }
    settings.endGroup();
    return new Contact(cId.toInt(),cName, cHostName, cIp, cPort, cKey);
}

QList<Contact*> Contact::getContactList(){
    QSettings settings;
    QList<Contact*> contactList;
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

QString Contact::getHostName() const{
    return mHostName;
}

QHostAddress Contact::getIpAddress() const{
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

void Contact::setHostName(QString hostName){
    //TODO: retrieve and store Ip
    mHostName = hostName;
}

void Contact::setIpAddress(QHostAddress ip){
    //TODO: retrieve and store host name
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
    settings.setValue(id + "/hname", mHostName);
    settings.setValue(id + "/ip", mIp.toString());
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
