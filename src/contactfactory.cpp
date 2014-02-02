#include "contactfactory.h"

Contact* ContactFactory::findById(int id){
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

QList<Contact*> ContactFactory::findByName(QString name){
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

QList<Contact*> ContactFactory::findByIp(QHostAddress ip){
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

Contact* ContactFactory::findByKey(QByteArray key){
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

QList<Contact*> ContactFactory::getContactList(){
    QSettings settings;
    QList<Contact*> contactList;
    settings.beginGroup("Contacts");
    QStringList idList = settings.childGroups();
    foreach(QString id, idList)
        contactList.append(ContactFactory::findById(id.toInt()));
    settings.endGroup();
    return contactList;
}
